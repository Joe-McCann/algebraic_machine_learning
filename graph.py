from node import Node, Kind
class Graph:
    def __init__(self, name):
        self.name = name
        self.layers = {}
        self.order = []
        self.pqual = None
    
    def insert_layer(self, name, data=[]):
        self.layers[name] = set(data)
        self.order.append(name)

    def insert_node(self, layer, node):
        self.layers[layer].add(node)

    def connect_nodes(self, parent, child):
        parent.add_child(child)
        child.add_parent(parent)

    def get_layer(self, name):
        return self.layers[name]

    def make_graph(self, posData, negData, pieces, pqual, rule):
        #Make layers from input
        self.pqual = pqual
        self.insert_layer(Kind.posdata, posData)
        self.insert_layer(Kind.negdata, negData)
        self.insert_layer(Kind.piece, pieces)
        self.layers[Kind.piece].add(self.pqual)
        #Create atom
        self.insert_layer(Kind.atom, [Node("0", 2)])
        #Create connections from atom layer to piece layer
        for atom in self.layers[Kind.atom]:
            for piece in self.layers[Kind.piece]:
                self.connect_nodes(atom, piece)
        #Create connections from piece layer to data layer
        for piece in self.layers[Kind.piece]:
            for pdata in self.layers[Kind.posdata]:
                if rule(pdata, piece):
                    self.connect_nodes(piece, pdata)
            for ndata in self.layers[Kind.negdata]:
                if rule(ndata, piece):
                    self.connect_nodes(piece, ndata)
        #Make the dual graph
        self.make_dualGraph(pqual)
        return self

    def make_dualGraph(self, pqual):
        #Make duals for positive data and put it in the dual positive data layer
        self.layers[Kind.d_posdata] = set()
        for member in self.layers[Kind.posdata]:
            member.make_dual()
            self.layers[Kind.d_posdata].add(member.dual)

        #Make duals for negative data and put it in the dual negative data layer
        self.layers[Kind.d_negdata] = set()
        for member in self.layers[Kind.negdata]:
            member.make_dual()
            self.layers[Kind.d_negdata].add(member.dual)

        #Make duals for pieces and put it in the dual pieces layer
        self.layers[Kind.d_piece] = set()
        for member in self.layers[Kind.piece]:
            member.make_dual()
            self.layers[Kind.d_piece].add(member.dual)

        #Make duals for atoms and put it in the dual atom layer
        self.layers[Kind.d_atom] = set()
        for member in self.layers[Kind.atom]:
            member.make_dual()
            self.layers[Kind.d_atom].add(member.dual)

        #Connect all the nodes in the dual graph
        for level in self.layers:
            for member in self.layers[level]:
                member.connect_dual()

        #Connect the positive qualities constant in the dual graph to all the positive data in the dual graph
        for data in self.layers[Kind.posdata]:
            self.connect_nodes(data.dual, pqual.dual)
        