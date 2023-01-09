import networkx as nx

from .utils import filter_gcc


def get_rr(num_nodes: int, mean_degree: float, gcc: bool = True) -> nx.Graph:
    """Get giant component of random-regular graph
    num_nodes: number of nodes. returning graph could be smaller
    degree: degree of all nodes
    """
    graph = nx.random_regular_graph(int(mean_degree), num_nodes)
    if gcc:
        graph = filter_gcc(graph)
    return graph
