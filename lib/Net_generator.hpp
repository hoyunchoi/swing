#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "CSV.hpp"
#include "Networks.hpp"
#include "linearAlgebra.hpp"
#include "pcg_random.hpp"

/*
    Structure to generate swing equation.
    Osillators are at specific network with adjacency matrix A
    d^2x/dt^2 + beta dx/dt = omega + K sum Asin(x'-x)

    Change the equation to two coupled first order differential equation
    v = dx/dt
    dv/dt + beta v = omega + K sum Asin(x'-x)

    Use 4-th order Runge-Kutta method to solve two equations
*/

namespace Swing {
struct Net_generator {
  protected:
    //* Member variables
    double dt;

    //* Network variables
    unsigned m_size;
    std::vector<std::set<unsigned>> m_adjacency;

    //* Swing equation parameters
    double m_friction; // beta
    double m_coupling; // K

    //* Oscillator variables
    std::vector<double> m_natural_frequency_list; //omega
    std::vector<double> m_theta_list;             // x
    std::vector<double> m_v_list;                 //v

    //* Random variables
    pcg32 m_random_engine;
    std::normal_distribution<double> m_natural_frequency_distribution; // N(0, 1)
    std::uniform_real_distribution<double> m_theta_distribution;       // U(-PI, PI)
    std::uniform_real_distribution<double> m_v_distribution;           // U(-1, 1)

    //* Variable for save history
    std::vector<std::vector<double>> m_theta_history;

    //* Member functions
  public:
    Net_generator() {}
    Net_generator(const Network<unsigned>&, const double&, const double&, const int&);
    void run(const unsigned&, const double&);
    void save(const std::string&) const;

  private:
    const std::vector<double> dtheta_dt(const std::vector<double>&) const;
    const std::vector<double> dv_dt(const std::vector<double>&, const std::vector<double>&) const;
    void RK4_step(const double&);
};

Net_generator::Net_generator(const Network<unsigned>& t_network,
                             const double& t_friction,
                             const double& t_coupling,
                             const int& t_random_engine_seed)
    : m_friction(t_friction),
      m_coupling(t_coupling) {
    //* Network
    m_size = t_network.size;
    m_adjacency = t_network.adjacency;

    //* Random variables
    t_random_engine_seed == -1 ? m_random_engine.seed((std::random_device())()) : m_random_engine.seed(t_random_engine_seed);
    m_theta_distribution.param(std::uniform_real_distribution<double>::param_type(-M_PI, M_PI));
    m_v_distribution.param(std::uniform_real_distribution<double>::param_type(-1.0, 1.0));

    //* Generate oscillators
    m_natural_frequency_list.assign(m_size, 0.0);
    m_theta_list.assign(m_size, 0.0);
    m_v_list.assign(m_size, 0.0);
    for (unsigned index = 0; index < m_size; ++index) {
        m_natural_frequency_list[index] = m_natural_frequency_distribution(m_random_engine);
        m_theta_list[index] = m_theta_distribution(m_random_engine);
        m_v_list[index] = m_v_distribution(m_random_engine);
    }

    //* Save initial values
    m_theta_history.emplace_back(m_theta_list);
}

const std::vector<double> Net_generator::dtheta_dt(const std::vector<double>& t_v_list) const {
    /*
        d theta / dt = v
        Do for every oscillators
    */
    return t_v_list;
}

const std::vector<double> Net_generator::dv_dt(const std::vector<double>& t_theta_list,
                                               const std::vector<double>& t_v_list) const {
    /*
        dv/dt = -beta v + omega + K sum Asin(x'-x)
        Do for every oscillators
    */
   using namespace linearAlgebra;
   std::vector<double> result(m_size, 0.0);
   //* sum Asin(x'-x)
   for (unsigned index=0; index<m_size; ++index){
       for (const unsigned neighbor : m_adjacency[index]){
           result[index] += sin(t_theta_list[neighbor] - t_theta_list[index]);
       }
   }

    //* K sum Asin(x'-x)
    result *= m_coupling;

    //* -beta v + omega + K sum Asin(x'-x)
    result += -m_friction * t_v_list + m_natural_frequency_list;
    return result;
}

void Net_generator::RK4_step(const double& t_dt){
    using namespace linearAlgebra;

    const std::vector<double> dtheta1 = dtheta_dt(m_v_list) * t_dt;
    const std::vector<double> dv1 = dv_dt(m_theta_list, m_v_list) * t_dt;

    const std::vector<double> dtheta2 = dtheta_dt(m_v_list + 0.5 * dv1) * t_dt;
    const std::vector<double> dv2 = dv_dt(m_theta_list + 0.5 * dtheta1, m_v_list + 0.5 * dv1) * t_dt;

    const std::vector<double> dtheta3 = dtheta_dt(m_v_list + 0.5 * dv2) * t_dt;
    const std::vector<double> dv3 = dv_dt(m_theta_list + 0.5 * dtheta2, m_v_list + 0.5 * dv2) * t_dt;

    const std::vector<double> dtheta4 = dtheta_dt(m_v_list + dv3) * t_dt;
    const std::vector<double> dv4 = dv_dt(m_theta_list + dtheta3, m_v_list + dv3) * t_dt;

    m_theta_list += (dtheta1 + 2.0 * dtheta2 + 2.0 * dtheta3 + dtheta4) / 6.0;
    m_v_list += (dv1 + 2.0 * dv2 + 2.0 * dv3 + dv4) / 6.0;
}

void Net_generator::run(const unsigned& t_max_iteration, const double& t_dt = 0.005) {
    //* Save delta t
    dt = t_dt;

    //* Vector to store every history of model
    m_theta_history.reserve(t_max_iteration + 1);

    //* Run max_iteration times
    for (unsigned iteration = 0; iteration < t_max_iteration; ++iteration) {
        RK4_step(dt);
        m_theta_history.emplace_back(m_theta_list);
    }
}


void Net_generator::save(const std::string& t_file_path = "") const {
    /*
        Save theta history to input file path
    */

    //* When file path is not specified, print to std out
    if (t_file_path == "") {
        using namespace linearAlgebra;
        print(m_theta_history);
    }
    //* Otherwise, save to the path
    else {
        CSV::write(t_file_path, m_theta_history);
    }
}

} // namespace Swing