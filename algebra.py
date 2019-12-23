from node import Node, Kind
from graph import Graph

class Algebra:
    def __init__(self):
        self.graph = Graph("Main")
        self.dual_graph = Graph("Dual")

    def insert_data(self, pos_data, neg_data, constant, rule):
        pos_nodes = []
        for data in pos_data:
            node = Node("PosTerm", 0)
            node.data = data
            pos_nodes.append(node)
        
        neg_nodes = []
        for data in neg_data:
            node = Node("NegTerm", 0)
            node.data = data
            neg_nodes.append(node)

        const_nodes = []
        for data in constant:
            node = Node("Constant", 1)
            node.data = data
            const_nodes.append(node)

        self.pqual = Node("v", 1)

        self.graph.make_graph(pos_nodes, neg_nodes, const_nodes, self.pqual, rule)
        

    def enforce_negative_trace_constraints(self): # algorithm 1
        pass

    def find_strongly_discriminant_constant(self, a, b):
        pass

    def enforce_positive_trace_constraints(self): # algorithm 2
        pass

    def sparse_crossing_of_A_into_B(self, a, b): # algorithm 3
        pass

    def atom_set_reduction(self): # algorithm 4
        pass

    def atom_set_reduction_for_dual_algebra(self): # algorithm 5
        pass

    

        

