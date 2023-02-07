#pragma once
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>

using Count = uint64_t;
using Node = uint64_t;
using EdgeList = std::vector<std::vector<Node>>;  // Size should be (E, 2)
using AdjMat = std::vector<std::vector<bool>>;    // Size should be (N, N)

/* Graph storing topology using adjacency list
The graph is undirected, unweighted.
Self loop is not permitted */
struct Graph {
    Count num_nodes;
    Count num_edges;
    std::vector<std::set<Node>> adjacency_list;  // set::find, set::erase is log(N)

    Graph() {}
    Graph(const Count& t_num_nodes) : num_nodes(t_num_nodes) {
        adjacency_list.assign(num_nodes, std::set<Node>{});
    }

    // Node operators
    void add_node(const Count&);
    void remove_node(const Node&);

    // Edge operators
    const bool has_edge(const Node&, const Node&) const;
    void add_edge(const Node&, const Node&);
    void remove_edge(const Node&, const Node&);

    // Getters
    const std::set<Node> get_neighbor(const Node&) const;
    const std::vector<Count> get_degrees() const;
    const double get_mean_degree() const;
    const std::map<Count, Count> get_degree_distribution() const;
    const EdgeList get_edge_list() const;
    const AdjMat get_adjacency_matrix() const;
};

/* Add input number of nodes into graph. Default: 1 */
void Graph::add_node(const Count& t_num_nodes = 1) {
    num_nodes += t_num_nodes;
    for (Count n = 0; n < t_num_nodes; ++n) {
        adjacency_list.emplace_back(std::set<Node>{});
    }
}

/* Remove node. If the node does not exists, raise error*/
void Graph::remove_node(const Node& t_node) {
    // If there is not node, print error and stop program
    if (num_nodes <= t_node) {
        std::cout << "Trying to remove node " << t_node << " which does not exist!\n";
        exit(1);
    }

    // Remove node
    --num_nodes;
    for (const Node& neighbor : adjacency_list[t_node]) {
        adjacency_list[neighbor].erase(t_node);
    }
    adjacency_list[t_node].clear();
}

/* Check if there exists an edge between two nodes.
If given two nodes are same, return true */
const bool Graph::has_edge(const Node& t_node1, const Node& t_node2) const {
    // If two nodes are same, return true
    if (t_node1 == t_node2) {
        return true;
    }

    // Find node with lower degree and search neighbor
    if (adjacency_list[t_node1].size() <= adjacency_list[t_node2].size()) {
        return (adjacency_list[t_node1].find(t_node2) != adjacency_list[t_node1].end());
    } else {
        return (adjacency_list[t_node2].find(t_node1) != adjacency_list[t_node2].end());
    }
}

/* Add node between two nodes if they are not connected
If given two nodes are same or edges are already exists, do nothing */
void Graph::add_edge(const Node& t_node1, const Node& t_node2) {
    // If there exists an edge between two nodes, do nothing
    if (has_edge(t_node1, t_node2)) {
        return;
    }

    // Add edge
    ++num_edges;
    adjacency_list[t_node1].emplace(t_node2);
    adjacency_list[t_node2].emplace(t_node1);
}

/* Remove node between two nodes. If they are not connected, raise error */
void Graph::remove_edge(const Node& t_node1, const Node& t_node2) {
    // If there is no edge between two nodes print error and stop program
    if (not has_edge(t_node1, t_node2)) {
        std::cout << "Trying to remove edge (" << t_node1 << ", " << t_node1
                  << ") which does not exist!\n";
        exit(1);
    }

    // Remove edge
    --num_edges;
    adjacency_list[t_node1].erase(t_node2);
    adjacency_list[t_node2].erase(t_node1);
}

/* Return set of neighbor nodes */
const std::set<Node> Graph::get_neighbor(const Node& t_node) const {
    return adjacency_list[t_node];
}

/* Return degree of each node */
const std::vector<Count> Graph::get_degrees() const {
    std::vector<Count> degrees(num_nodes, 0);
    for (Node node = 0; node < num_nodes; ++node) {
        degrees[node] = adjacency_list[node].size();
    }
    return degrees;
}

const double Graph::get_mean_degree() const {
    const auto degrees = get_degrees();
    return std::accumulate(degrees.begin(), degrees.end(), 0.0) / degrees.size();
}

/* Return degree distribution of entire graph */
const std::map<Count, Count> Graph::get_degree_distribution() const {
    const auto degrees = get_degrees();
    std::map<Count, Count> degree_distribution;
    for (const auto degree : degrees) {
        ++degree_distribution[degree];
    }
    return degree_distribution;
}

/* Return edge list. If (0, 1) is included in edge list, (1, 0) is not included */
const EdgeList Graph::get_edge_list() const {
    // Create empty edge list of size num_edges
    EdgeList edge_list;
    edge_list.reserve(num_nodes);

    // Scan adjacency list and store edge with node1 < node2
    for (Node node = 0; node < num_nodes; ++node) {
        for (const Node& neighbor : adjacency_list[node]) {
            if (node >= neighbor) {
                continue;
            }
            edge_list.emplace_back(std::vector<Node>{node, neighbor});
        }
    }
    return edge_list;
}

/* Return adjacency matrix */
const AdjMat Graph::get_adjacency_matrix() const {
    // Create zero adjacency matrix
    AdjMat adjacency_matrix(num_nodes, std::vector<bool>(num_nodes, false));

    // Scan adjacency list and store true for each edges
    for (Node node = 0; node < num_nodes; ++node) {
        for (const Node& neighbor : adjacency_list[node]) {
            adjacency_matrix[node][neighbor] = true;
        }
    }
    return adjacency_matrix;
}