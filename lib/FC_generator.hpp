#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "CSV.hpp"
#include "linearAlgebra.hpp"
#include "pcg_random.hpp"

/*
    Structure to generate swing equation. Osillators are fully connected
    d^2x/dt^2 + beta dx/dt = omega + K/N sum sin(x'-x)

    Change the equation to two coupled first order differential equation
    v = dx/dt
    dv/dt + beta v = omega + K/N sum sin(x'-x)

    Use 4-th order Runge-Kutta method to solve two equations

*/
namespace Swing {

struct FC_generator {
  protected:
    //* Member variables
    unsigned size;
    double dt;

    //* Parameters of swing equation
    double m_friction; // beta
    double m_coupling; // K/N

    //* Oscillator related variables
    std::vector<double> m_natural_frequency_list; // omega
    std::vector<double> m_theta_list;             // x
    std::vector<double> m_v_list;                 // v

    //* Random variables
    pcg32 m_random_engine;
    std::normal_distribution<double> m_natural_frequency_distribution;
    std::uniform_real_distribution<double> m_theta_distribution;
    std::uniform_real_distribution<double> m_v_distribution;

    //* Parameter to save data
    std::vector<std::vector<double>> m_theta_history;

    //* Member functions
  public:
    FC_generator() {}
    FC_generator(const unsigned&, const double&, const double&, const int&);
    void run(const unsigned&, const double&);
    void save(const std::string&) const;

  protected:
    const std::vector<double> dtheta_dt(const std::vector<double>&) const;
    const std::vector<double> dv_dt(const std::vector<double>&, const std::vector<double>&) const;

    void RK4_step(const double&);
};

FC_generator::FC_generator(const unsigned& t_size,
                     const double& t_friction,
                     const double& t_coupling,
                     const int& t_random_engine_seed)
    : size(t_size),
      m_friction(t_friction),
      m_coupling(t_coupling) {
    //* Initialize random variables
    t_random_engine_seed == -1 ? m_random_engine.seed((std::random_device())()) : m_random_engine.seed(t_random_engine_seed);
    m_theta_distribution.param(std::uniform_real_distribution<double>::param_type(-M_PI, M_PI));
    m_v_distribution.param(std::uniform_real_distribution<double>::param_type(-1.0, 1.0));

    //* Generate oscillators
    m_natural_frequency_list.assign(size, 0.0);
    m_theta_list.assign(size, 0.0);
    m_v_list.assign(size, 0.0);
    for (unsigned index = 0; index < t_size; ++index) {
        m_natural_frequency_list[index] = m_natural_frequency_distribution(m_random_engine);
        m_theta_list[index] = m_theta_distribution(m_random_engine);
        m_v_list[index] = m_v_distribution(m_random_engine);
    }

    //* Save initial values
    m_theta_history.emplace_back(m_theta_list);
}

const std::vector<double> FC_generator::dtheta_dt(const std::vector<double>& t_v_list) const {
    /*
        d theta / dt = v
        Do for every oscillators
    */
    return t_v_list;
}

const std::vector<double> FC_generator::dv_dt(const std::vector<double>& t_theta_list,
                                           const std::vector<double>& t_v_list) const {
    /*
        dv/dt = -beta v + omega + K/N sum sin(x'-x)
        Do for every oscillators
    */
    using namespace linearAlgebra;
    std::vector<double> result(size, 0.0);
    //* sum sin(x'-x)
    for (unsigned index = 0; index < size; ++index) {
        for (unsigned neighbor = 0; neighbor < size; ++neighbor) {
            result[index] += sin(t_theta_list[neighbor] - t_theta_list[index]);
        }
    }
    //* K/N sum sin(x'-x)
    result *= m_coupling;

    //* -beta v + omega + K/N sum sin(x'-x)
    result += -m_friction * t_v_list + m_natural_frequency_list;
    return result;
}

void FC_generator::RK4_step(const double& t_dt) {
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

void FC_generator::run(const unsigned& t_max_iteration, const double& t_dt = 0.005) {
    //* Save delta t
    dt = t_dt;

    //* Vector to store every history of model
    m_theta_history.reserve(t_max_iteration + 1);

    for (unsigned iteration = 0; iteration < t_max_iteration; ++iteration) {
        RK4_step(dt);
        m_theta_history.emplace_back(m_theta_list);
    }
}

void FC_generator::save(const std::string& t_file_path = "") const {
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
