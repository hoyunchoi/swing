#pragma once

#include <random>

#include "graph.hpp"
#include "pcg_random.hpp"

namespace ER {
/* Generate random ER graph with adding link with given probability*/
Graph generate_by_prob(
    const Count& t_num_nodes, const double& t_prob, pcg64& t_random_engine
) {
    Graph graph(t_num_nodes);
    std::uniform_real_distribution<double> prob_distribution(0.0, 1.0);

    for (Node node1 = 0; node1 < t_num_nodes; ++node1) {
        for (Node node2 = node1 + 1; node2 < t_num_nodes; ++node2) {
            if (prob_distribution(t_random_engine) < t_prob) {
                graph.add_edge(node1, node2);
            }
        }
    }
    return graph;
}

/* Generate random ER graph with given mean degree*/
Graph generate_by_degree(
    const Count& t_num_nodes, const double& t_mean_degree, pcg64& t_random_engine
) {
    Graph graph(t_num_nodes);
    const Count num_edges = (Count)(t_num_nodes * t_mean_degree / 2);
    std::uniform_int_distribution<Node> node_distribution(0, t_num_nodes - 1);

    while (graph.num_edges < num_edges) {
        graph.add_edge(
            node_distribution(t_random_engine), node_distribution(t_random_engine)
        );
    }

    return graph;
}

}  // namespace ER
