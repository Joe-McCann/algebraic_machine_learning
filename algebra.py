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

    def getLowerSet(self, node):
        lset = set()
        lset.add(node)
        for ns in node.lower:
            hold = self.getLowerSet(ns)
            for el in hold:
                lset.add(el)
        return lset

    def getUpperSet(self, node):
        uset = set()
        for ns in node.upper:
            uset.add(ns)
            hold = self.getUpperSet(ns)
            for el in hold:
                uset.add(el)
        return uset

    def getUpperSetConstrained(self, node, layer):
        uset = self.getUpperSet(node)
        return uset.intersection(self.graph.layers[layer])

    def getLowerSetConstrained(self, node, layer):
        uset = self.getLowerSet(node)
        return uset.intersection(self.graph.layers[layer])

    def getTrace(self, node):
        atoms = self.getLowerSetConstrained(node, Kind.atom)
        trace = self.graph.layers[Kind.d_baseAtom]
        for atom in atoms:
            gl = self.getLowerSet(atom.dual)
            trace = trace.intersection(gl)
        return trace
    
    def mini_preprocessing_step(self):
        for piece in self.graph.layers[Kind.d_negdata]:
            natom = Node("NegZetta", Kind.d_baseAtom)
            self.graph.layers[Kind.d_baseAtom].add(natom)
            self.graph.connect_nodes(natom, piece)

    def enforce_negative_trace_constraints(self): # algorithm 1
        for neg in self.graph.layers[Kind.negdata]:
            if self.getTrace(neg).issubset(self.getTrace(self.graph.pqual)):
                c = False
                while not c:
                    c = self.find_strongly_discriminant_constant(self.graph.pqual, neg)
                    if not c:
                        temp = self.getLowerSet(neg.dual).intersection(self.graph.layers[Kind.d_negdata].union(self.graph.layers[Kind.d_posdata])).difference(self.getLowerSet(self.graph.pqual.dual))
                        h = temp.pop()
                        # Create new d_baseAtom zeta and connect it to h
                        zeta = Node("Zeta", Kind.d_baseAtom)
                        self.graph.layers[Kind.d_baseAtom].add(zeta)
                        self.graph.connect_nodes(zeta, h)
                # Create new atom phi and connect phi to c
                phi = Node("phi", Kind.atom)
                phi.make_dual()
                self.graph.connect_nodes(phi, c)
                phi.connect_dual()
                self.graph.layers[Kind.atom].add(phi)
                self.graph.layers[Kind.d_atom].add(phi.dual)

    def find_strongly_discriminant_constant(self, a, b):
        Omega = set([c.dual for c in self.getLowerSetConstrained(a, Kind.piece)])
        U = self.getTrace(b)
        while U:
            zeta = U.pop()
            Psi = Omega.difference(self.getUpperSet(zeta))
            if Psi:
                c = Psi.pop()
                return c.dual
        
        return None


    def enforce_positive_trace_constraints(self): # algorithm 2
        for pos in self.graph.layers[Kind.posdata]:
            while not self.getTrace(pos).issubset(self.getTrace(self.graph.pqual)):
                zeta = self.getTrace(pos).difference(self.getTrace(self.graph.pqual)).pop()
                gammaPrime = self.getLowerSetConstrained(pos, Kind.piece)
                gamma = set()
                for node in gammaPrime:
                    if zeta not in self.getLowerSet(node.dual):
                        gamma.add(node)
                if not gamma:
                    # add edge zeta -> pqual.dual
                    self.graph.connect_nodes(zeta, self.graph.pqual.dual)
                else:
                    c = gamma.pop()
                    # create atom phi and connect phi -> c
                    phi = Node("phi", Kind.atom)
                    phi.make_dual()
                    phi.connect_dual()
                    self.graph.layers[Kind.atom].add(phi)
                    self.graph.layers[Kind.d_atom].add(phi.dual)
                    self.graph.connect_nodes(phi, c)
                    self.graph.connect_nodes(phi.dual, c.dual)

    def sparse_crossing_of_A_into_B(self, a, b): # algorithm 3
        for pos in self.graph.layers[Kind.posdata]:
            A = self.getLowerSetConstrained(self.graph.pqual, Kind.atom).difference(self.getLowerSet(pos))
            for phi in A:
                U = set()
                B = self.getLowerSetConstrained(pos, Kind.atom)
                D = self.graph.layers[Kind.d_baseAtom].difference(self.getLowerSet(phi.dual))
                flag = True
                while D or flag:
                    e = B.pop()
                    DPrime = D.intersection(self.getLowerSet(e.dual))
                    if not D or (DPrime.issubset(D) and D.issubset(DPrime)):
                        # create new atom psi and psi -> phi, psi -> e
                        D = DPrime
                        U.add(e)
                    flag = False
        for eps in U:
            # create edge eps' -> eps
        

    def atom_set_reduction(self): # algorithm 4
        pass

    def atom_set_reduction_for_dual_algebra(self): # algorithm 5
        pass

    

        

