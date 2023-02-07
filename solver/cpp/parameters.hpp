#pragma once
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include "graph.hpp"
#include "pcg_random.hpp"
#include "weighted_edge.hpp"

using Count = uint64_t;
using Node = uint64_t;

namespace Swing {

template <typename T>
struct Parameters {
    //* Parameters: node properties
    std::vector<T> phase, dphase, power, gamma, mass;

    //* Parameters: network properties
    std::vector<WeightedEdge<T>> weighted_edge_list;

    //* Parameters: times
    std::vector<T> dts;

    Parameters() {}
    Parameters(
        const Graph& t_graph,
        const Count& t_num_steps,
        const T& t_dt,
        pcg64& t_random_engine
    ) {
        //* Randomly create phase: from 0 to 2pi
        phase.assign(t_graph.num_nodes, 0.0);
        std::uniform_real_distribution<T> phase_distribution(0.0, M_PI_2);
        std::generate(
            phase.begin(),
            phase.end(),
            [&phase_distribution, &t_random_engine]() {
                return phase_distribution(t_random_engine);
            }
        );

        //* Randomly create dphase: from 0 to 1
        dphase.assign(t_graph.num_nodes, 0.0);
        std::uniform_real_distribution<T> dphase_distribution(0.0, 1.0);
        std::generate(
            dphase.begin(),
            dphase.end(),
            [&dphase_distribution, &t_random_engine]() {
                return dphase_distribution(t_random_engine);
            }
        );

        //* Fill 1 to power, gamma, mass
        power.assign(t_graph.num_nodes, 1.0);
        gamma.assign(t_graph.num_nodes, 1.0);
        mass.assign(t_graph.num_nodes, 1.0);

        //* Weighted edge_list with all weights are 1
        weighted_edge_list.reserve(t_graph.num_edges);
        for (const auto edge_list : t_graph.get_edge_list()) {
            weighted_edge_list.emplace_back(
                WeightedEdge<T>(edge_list[0], edge_list[1], (T)1.0)
            );
        }

        //* Fill dts with value dt
        dts.assign(t_num_steps, t_dt);
    }

    Parameters(
        const std::vector<T> t_args,
        const Count& t_num_nodes,
        const Count& t_num_edges,
        const Count& t_num_steps
    ) {
        //* Parameter locations at argument file
        const Count phase_start_idx = 0;
        const Count dphase_start_idx = phase_start_idx + t_num_nodes;
        const Count power_start_idx = dphase_start_idx + t_num_nodes;
        const Count gamma_start_idx = power_start_idx + t_num_nodes;
        const Count mass_start_idx = gamma_start_idx + t_num_nodes;
        const Count wedge_list_start_idx = mass_start_idx + t_num_nodes;
        const Count dt_start_idx = wedge_list_start_idx + 3 * t_num_edges;

        //* Parameters: node properties
        phase.assign(t_num_nodes, 0.0);
        dphase.assign(t_num_nodes, 0.0);
        power.assign(t_num_nodes, 0.0);
        gamma.assign(t_num_nodes, 0.0);
        mass.assign(t_num_nodes, 0.0);
        for (Count i = 0; i < t_num_nodes; ++i) {
            phase[i] = t_args[phase_start_idx + i];
            dphase[i] = t_args[dphase_start_idx + i];
            power[i] = t_args[power_start_idx + i];
            gamma[i] = t_args[gamma_start_idx + i];
            mass[i] = t_args[mass_start_idx + i];
        }

        //* Parameters: network properties
        weighted_edge_list.reserve(t_num_edges);
        for (Count e = 0; e < t_num_edges; ++e) {
            const Node node1 = (Node)t_args[wedge_list_start_idx + 3 * e];
            const Node node2 = (Node)t_args[wedge_list_start_idx + 3 * e + 1];
            const T weight = t_args[wedge_list_start_idx + 3 * e + 2];
            weighted_edge_list.emplace_back(WeightedEdge<T>(node1, node2, weight));
        }

        //* Parameters: times
        dts.assign(t_num_steps, 0.0);
        for (Count t = 0; t < t_num_steps; ++t) {
            dts[t] = t_args[dt_start_idx + t];
        }
    }
};

}  // namespace Swing
