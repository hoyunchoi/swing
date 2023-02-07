#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "arguments.hpp"
#include "er.hpp"
#include "parameters.hpp"
#include "pcg_random.hpp"
#include "solver.hpp"
#include "solver_original.hpp"

namespace Swing {

template <typename T>
void solve(const std::string& t_solver_name, const Parameters<T>& t_params) {
    //* Run Runge-Kutta solver
    std::vector<std::vector<T>> trajectories;
    if (t_solver_name.find("rk1") != std::string::npos) {
        trajectories = Swing::solve_rk1(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    } else if (t_solver_name.find("rk2") != std::string::npos) {
        trajectories = Swing::solve_rk2(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    } else {
        trajectories = Swing::solve_rk4(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    }
}

template <typename T>
void solve_original(const std::string& t_solver_name, const Parameters<T>& t_params) {
    //* Run Runge-Kutta solver
    std::vector<std::vector<T>> trajectories;
    if (t_solver_name.find("rk1") != std::string::npos) {
        trajectories = Swing::solve_rk1_original(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    } else if (t_solver_name.find("rk2") != std::string::npos) {
        trajectories = Swing::solve_rk2_original(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    } else {
        trajectories = Swing::solve_rk4_original(
            t_params.weighted_edge_list,
            {t_params.phase, t_params.dphase},
            {t_params.power, t_params.gamma, t_params.mass},
            t_params.dts
        );
    }
}

}  // namespace Swing

int main(int argc, char* argv[]) {
    //* Get input variables
    const std::string solver_name = argv[1];
    const uint64_t num_nodes = std::stoull(argv[2]);
    const double mean_degree = std::stod(argv[3]);
    const uint64_t num_steps = std::stoull(argv[4]);
    const uint64_t precision = std::stoull(argv[5]);

    pcg64 random_engine((std::random_device())());
    const Graph graph = ER::generate_by_degree(num_nodes, mean_degree, random_engine);

    if (precision == 32) {
        const Swing::Parameters<float> params(
            graph, num_steps, (float)0.01, random_engine
        );
        if (solver_name.find("original") != std::string::npos) {
            for (int i = 0; i < 700; ++i) {
                const auto start = std::chrono::system_clock::now();
                Swing::solve_original(solver_name, params);
                std::chrono::duration<double> sec =
                    std::chrono::system_clock::now() - start;
                std::cout << sec.count() << "\n";
            }
        } else {
            for (int i = 0; i < 700; ++i) {
                const auto start = std::chrono::system_clock::now();
                Swing::solve(solver_name, params);
                std::chrono::duration<double> sec =
                    std::chrono::system_clock::now() - start;
                std::cout << sec.count() << "\n";
            }
        }

    } else {
        const Swing::Parameters<double> params(
            graph, num_steps, (double)0.01, random_engine
        );
        if (solver_name.find("original") != std::string::npos) {
            for (int i = 0; i < 700; ++i) {
                const auto start = std::chrono::system_clock::now();
                Swing::solve_original(solver_name, params);
                std::chrono::duration<double> sec =
                    std::chrono::system_clock::now() - start;
                std::cout << sec.count() << "\n";
            }
        } else {
            for (int i = 0; i < 700; ++i) {
                const auto start = std::chrono::system_clock::now();
                Swing::solve(solver_name, params);
                std::chrono::duration<double> sec =
                    std::chrono::system_clock::now() - start;
                std::cout << sec.count() << "\n";
            }
        }
    }

    return 0;
}