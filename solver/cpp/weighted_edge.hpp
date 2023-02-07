#pragma once

using Node = uint64_t;
namespace Swing {

template <typename T>
struct WeightedEdge {
    Node node1;
    Node node2;
    T weight;

    WeightedEdge() {}
    WeightedEdge(const Node& t_node1, const Node& t_node2, const T& t_weight)
        : node1(t_node1), node2(t_node2), weight(t_weight) {}
};

}  // namespace Swing