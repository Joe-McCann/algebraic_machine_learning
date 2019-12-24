from algebra import Algebra
from node import Kind

def isConnected(const, term):
    for i in range(len(term.data)):
        if term.data[i] == const.data[i]:
            return True
    
    return False

pos_data = ["1100", "0011"]
neg_data = ["1010", "1000", "0010", "0101"]
constants = ["0---", "-0--", "--0-", "---0", "1---", "-1--", "--1-", "---1"]

algebra = Algebra()
algebra.insert_data(pos_data, neg_data, constants, isConnected)
algebra.mini_preprocessing_step()

algebra.enforce_negative_trace_constraints()
algebra.enforce_positive_trace_constraints()

print("all done")

