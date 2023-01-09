import networkx as nx
from .utils import filter_gcc


def get_er(num_nodes: int, mean_degree: float, gcc: bool = True) -> nx.Graph:
    """ Get giant component of ER random graph
    num_nodes: number of nodes. returning graph could be smaller
    mean_degree: mean degree of resulting graph
    """
    p = mean_degree / (num_nodes-1)
    graph = nx.fast_gnp_random_graph(num_nodes, p)
    if gcc:
        graph = filter_gcc(graph)
    return graph

if __name__ == "__main__":
    from utils import get_mean_degree
    g = get_er(10, 2)
    print(g.number_of_nodes())
    print(get_mean_degree(g))
    a  = nx.laplacian_matrix(g).todense()
    print(type(a))
    import numpy as np
    print(np.asarray(a))