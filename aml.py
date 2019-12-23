class Node:
    def __init__(self, name, data={}):
        self.name = name
        self.dual = None
        self.data = data
        self.upper = set()
        self.lower = set()

    def set_dual(self, node):
        self.dual = node

    def get_dual(self):
        return self.dual

    def add_parent(self, node):
        self.upper.add(node) 

    def add_child(self, node):
        self.lower.add(node)

    def get_parent(self):
        return self.upper

    def get_children(self):
        return self.lower
    
    def __str__(self):
        return self.name

class Graph:
    def __init__(self, name):
        self.name = name
        self.layers = {}
        self.order = []
    
    def insert_layer(self, name, data=[]):
        self.layers[name] = set(data)
        self.order.append(name)

    def insert_node(self, layer, node):
        self.layers[layer].add(node)

    def connect_nodes(self, child, parent):
        child.add_parent(parent)
        parent.add_child(child)

    def get_layer(self, name):
        return self.layers[name]

class Algebra:
    def __init__(self):
        self.graph = Graph("Main")
        self.dual_graph = Graph("Dual")

    def insert_data(self, data, constant, rules):
        self.graph.insert_layer("terms", data = data)
        self.graph.insert_layer("constants", data = constant)

        for term in self.graph.get_layer("terms"):
            for constant in self.graph.get_layer("constants"):
                self.graph.connect_nodes(constant, term)


