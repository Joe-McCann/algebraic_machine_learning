from enum import Enum
class Node:
    def __init__(self, name, kind, data={}):
        self.name = name
        self.kind = kind
        self.dual = None
        self.data = data
        self.upper = set()
        self.lower = set()

    def set_dual(self, node):
        self.dual = node

    def get_dual(self):
        return self.dual

    def make_dual(self):
        dual = Node(self.name, self.kind, self.data)
        self.dual = dual
        self.dual.dual = self

    def connect_dual(self):
        for parent in self.upper:
            self.dual.add_child(parent.dual)
        for child in self.lower:
            self.dual.add_parent(child.dual)

    def add_parent(self, node):
        self.upper.add(node) 

    def add_child(self, node):
        self.lower.add(node)
    
    def __str__(self):
        return self.name

class Kind(Enum):
    posdata = 0
    negdata = 1
    piece = 2
    atom = 3
    d_posdata = 4
    d_negdata = 5
    d_piece = 6
    d_atom = 7
    d_baseAtom = 8
