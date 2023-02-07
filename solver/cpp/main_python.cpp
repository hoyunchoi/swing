#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "arguments.hpp"
#include "parameters.hpp"
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

    //* Report result with maximum precision
    std::cout << std::setprecision(std::numeric_limits<T>::digits10 + 1);
    for (const auto& trajectory : trajectories) {
        for (const auto& traj : trajectory) {
            std::cout << traj << " ";
        }
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

    //* Report result with maximum precision
    std::cout << std::setprecision(std::numeric_limits<T>::digits10 + 1);
    for (const auto& trajectory : trajectories) {
        for (const auto& traj : trajectory) {
            std::cout << traj << " ";
        }
    }
}

}  // namespace Swing

int main(int argc, char* argv[]) {
    //* Get input variables
    const std::string solver_name = argv[1];
    const uint64_t num_nodes = std::stoull(argv[2]);
    const uint64_t num_edges = std::stoull(argv[3]);
    const uint64_t num_steps = std::stoull(argv[4]);
    const uint64_t precision = std::stoull(argv[5]);
    const std::string arg_file_name = argv[6];

    if (precision == 32) {
        // Read argument file and store to a single vector
        const std::vector<float> args = Swing::read_arg_file(
            arg_file_name, 5 * num_nodes + 3 * num_edges + num_steps, (float)0.0
        );

        // Arguments to swing parameters
        const Swing::Parameters<float> params(args, num_nodes, num_edges, num_steps);

        // Solve swing equation
        if (solver_name.find("orginal") != std::string::npos) {
            Swing::solve_original(solver_name, params);
        } else {
            Swing::solve(solver_name, params);
        }

    } else {
        // Read argument file and store to a single vector
        const std::vector<double> args = Swing::read_arg_file(
            arg_file_name, 5 * num_nodes + 3 * num_edges + num_steps, (double)0.0
        );

        // Arguments to swing parameters
        const Swing::Parameters<double> params(args, num_nodes, num_edges, num_steps);

        // Solve swing equation
        if (solver_name.find("orginal") != std::string::npos) {
            Swing::solve_original(solver_name, params);
        } else {
            Swing::solve(solver_name, params);
        }
    }

    return 0;
}