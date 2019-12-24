#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "Node.h"

using namespace std;

Node* decomposeTerm(Node* term, set<Node* > &constants);
Node* findStronglyDiscriminantConstant(Node* a, Node* b, vector<set<Node* >> layers);
void sparseCross(Node* a, Node* b, vector<set<Node*>> &layers);
void algebraReduction(vector<set<Node*>> &layers);

int main() {
	//Atoms, DualofAtoms, Constants, DualContants, Terms, DualTerms, DualAtoms

	/*First Section sets up all necessary components to begin graph formation*/
	vector<set<Node* >> layers(7, set<Node* >());
	set<Node*> posConstraints;
	set<Node*> negConstraints;
	vector<vector<int>> init{ {-1, -1}, {-1, -1} };
	vector<vector<vector<int>>> tplus{ {{1,0},{1,0}}, {{1,1},{1,0}}, {{0,1},{0,1}}, {{1,1},{0,1}}, {{1,0},{1,1}}, {{0,1},{1,1}}, {{1,1},{1,1}} };//tplus{ {{1,0},{1,0}}, {{0,1},{0,1}} };
	vector<vector<vector<int>>> tneg{ {{0,0},{0,0}}, {{1,0},{0,0}}, {{0,1},{0,0}}, {{1,1},{0,0}}, {{0,0},{1,0}}, {{0,1},{1,0}}, {{0,0},{0,1}}, {{1,0},{0,1}}, {{0,0},{1,1}} };//tneg{ {{1,0},{0,1}}, {{0,1},{0,0}}, {{0,0},{0,1}} };
	Node* base = new Node("Base");
	Node* dualBase = new Node("*Base*");
	Node* v = new Node(init);
	linkLU(base, v);
	layers[0].insert(base);
	layers[2].insert(v);
	layers[6].insert(dualBase);

	/*Creating constants and linking them to the base*/
	for (int i = 0; i < 8; ++i) {
		int val = i / 4;
		for (int j = 0; j < 4; ++j) {
			if (j == (i % 4)) {
				init[j / 2][j % 2] = val;
			}
			else {
				init[j / 2][j % 2] = -1;
			}
		}
		Node* constant = new Node(init);
		linkLU(base, constant);
		layers[2].insert(constant);
	}

	/*Creating terms and linking them to constants*/
	for (vector<vector<int>> arr : tplus) {
		Node* pos = new Node(arr);
		pos = decomposeTerm(pos, layers[2]);
		//linkLU(v, pos);
		layers[4].insert(pos);
		posConstraints.insert(pos);
	}

	for (vector<vector<int>> arr : tneg) {
		Node* neg = new Node(arr);
		neg = decomposeTerm(neg, layers[2]);
		layers[4].insert(neg);
		negConstraints.insert(neg);
	}

	/*Create the dual graph M* and then link it together*/
	for (int i = 0; i < 3; ++i) {
		for (Node* node : layers[2 * i]) {
			Node* dual = new Node(node->getArray());
			dual->setName(node->getName() + "*");
			linkDuals(node, dual);
			layers[2 * i + 1].insert(dual);
		}
	}

	for (int i = 0; i < 3; ++i) {
		for (Node* node : layers[2 * i]) {
			Node* dual = node->getDual();
			for (Node* low : node->getLower()) {
				dual->addUpper(low->getDual());
			}
			for (Node* up : node->getUpper()) {
				dual->addLower(up->getDual());
			}
			if (i == 2) {
				linkLU(dualBase, dual);
			}
		}
	}

	for (Node* pos : posConstraints) {
		linkLU(pos->getDual(), v->getDual());
	}

	/*Enforce Trace Constraints*/
	int zcount = 1;
	int pcount = 1;
	for (Node* neg : negConstraints) {
		Node* zeta = new Node("zeta" + to_string(zcount++));
		layers[6].insert(zeta);
		linkLU(zeta, neg->getDual());
	}

	/*Algorithm 1: Enforce Negative Constraints*/
	for (Node* neg : negConstraints) {
		if (isSubset(neg->getTrace(layers[0], layers[6]), v->getTrace(layers[0], layers[6]))) {
			Node* c;
			do {
				c = findStronglyDiscriminantConstant(v, neg, layers);
				if (c == NULL) {
					Node* h = *difference(neg->getDual()->getLowerSet(layers[5]), v->getDual()->getLowerSet()).begin();
					Node* zeta = new Node("zeta" + to_string(zcount++));
					linkLU(zeta, h);
					layers[6].insert(zeta);
				}
			} while (c == NULL);
			Node* phi = new Node("phi" + to_string(pcount));
			Node* dphi = new Node("phi" + to_string(pcount++) + "*");
			linkLU(phi, c);
			linkDuals(phi, dphi);
			linkLU(c->getDual(), dphi);
			layers[0].insert(phi);
			layers[1].insert(dphi);
		}
	}

	/*Algorithm 2: Enforce positive constraints*/
	for (Node* pos : posConstraints) {
		set<Node*> Te = pos->getTrace(layers[0], layers[6]);
		set<Node*> Td = v->getTrace(layers[0], layers[6]);
		while (!isSubset(Te, Td)) {
			Node* zeta = *difference(Te, Td).begin();
			set<Node*> GammaTemp = pos->getLowerSet(layers[2]);
			set<Node*> Gamma;
			bool flag = true;
			for (Node* c : GammaTemp) {
				flag = true;
				for (Node* du : c->getDual()->getLowerSet()) {
					if (du == zeta)
						flag = false;
				}
				if (flag)
					Gamma.insert(c);
			}

			if (Gamma.size() == 0) {
				linkLU(zeta, v->getDual());
			}
			else {
				Node* c = *Gamma.begin();
				Node* phi = new Node("phi" + to_string(pcount));
				Node* dphi = new Node("phi" + to_string(pcount++) + "*");
				linkLU(phi, c);
				linkDuals(phi, dphi);
				linkLU(c->getDual(), dphi);
				layers[0].insert(phi);
				layers[1].insert(dphi);
			}

			Te = pos->getTrace(layers[0], layers[6]);
			Td = v->getTrace(layers[0], layers[6]);

		}
	}

	/*Sparse Crossing*/

	for (Node* pos : posConstraints) {
		sparseCross(v, pos, layers);
	}

	/* Model Reduction */

	algebraReduction(layers);
	algebraReduction(layers);


	/*Testing*/
	for (Node* neg : negConstraints) {
		if (isSubset(neg->getTrace(layers[0], layers[6]), v->getTrace(layers[0], layers[6]))) {
			cout << "FUCK" << endl;
		}
	}

	for (Node* pos : posConstraints) {
		set<Node*> Te = pos->getTrace(layers[0], layers[6]);
		set<Node*> Td = v->getTrace(layers[0], layers[6]);
		if (!isSubset(Te, Td)) {
			cout << "FUCK" << endl;
		}
	}
	int count = 1;
	for (Node* atom : layers[0]) {
		set<Node*> up = atom->getUpper();
		if (up.find(v) != up.end()) {
			cout << "Atom " << count << endl;
			for (Node* u : atom->getUpper()) {
				cout << u->getName() << endl;
			}
			count++;
		}
	}

	/*Cleaning up after myself*/
	for (set<Node* > layer : layers) {
		for (Node* node : layer) {
			delete(node);
		}
	}
	cout << "We done" << endl;
	cin.ignore();
	cin.ignore();
	return 0;
}

/*Decompose the term into the constants that make it up*/
Node* decomposeTerm(Node* term, set<Node* > &constants) {
	vector<vector<int>> arr = term->getArray();
	for (Node* node : constants) {
		vector<vector<int>> nodeGrid = node->getArray();
		for (int j = 0; j < 4; ++j) {
			if (nodeGrid[j / 2][j % 2] == arr[j / 2][j % 2]) {
				linkLU(node, term);
			}
		}
	}

	return term;
}



Node* findStronglyDiscriminantConstant(Node* a, Node* b, vector<set<Node* >> layers) {
	set<Node*> omega;
	for (Node* node : a->getLowerSet(layers[2])) {
		omega.insert(node->getDual());
	}

	set<Node*> U = b->getTrace(layers[0], layers[6]);
	set<Node*> temp;
	while (U.size() != 0) {
		Node* zeta = *U.begin();
		U.erase(U.begin());
		temp = difference(omega, zeta->getUpperSet());
		if (temp.size() != 0) {
			return (*temp.begin())->getDual();
		}
	}
	return NULL;
}

void sparseCross(Node* a, Node* b, vector<set<Node*>> &layers) {
	set<Node*> A = difference(a->getLowerSet(layers[0]), b->getLowerSet());
	for (Node* phi : A) {
		set<Node*> U;
		set<Node*> B = b->getLowerSet(layers[0]);
		set<Node*> Delta = difference(layers[6], phi->getDual()->getLowerSet());
		do {
			Node* epsilon = *B.begin();
			set<Node*> DeltaP = intersect(Delta, epsilon->getDual()->getLowerSet());
			if (Delta.size() == 0 || (!isSubset(Delta, DeltaP) || !isSubset(DeltaP, Delta))) {
				Node* psi = new Node("psi");
				Node* dualPsi = new Node("psi*");
				linkDuals(dualPsi, psi);
				linkLU(psi, phi);
				linkLU(psi, epsilon);
				linkLU(epsilon->getDual(), psi->getDual());
				linkLU(phi->getDual(), psi->getDual());
				layers[0].insert(psi);
				layers[1].insert(psi->getDual());
				Delta = DeltaP;
				U.insert(epsilon);
			}
			B.erase(epsilon);
		} while (Delta.size() != 0);

		for (Node* epsilon : U) {
			Node* epPr = new Node("epsilonPrime");
			Node* dualepPr = new Node("epsilonPrime*");
			linkDuals(epPr, dualepPr);
			linkLU(epsilon->getDual(), epPr->getDual());
			linkLU(epPr, epsilon);
			layers[0].insert(epPr);
			layers[0].erase(epsilon);
			layers[1].insert(epPr->getDual());
			layers[1].erase(epsilon->getDual());
			removeNode(epsilon->getDual());
			removeNode(epsilon);
			delete(epsilon->getDual());
			delete(epsilon);
		}
	}

	for (Node* phi : A) {
		layers[0].erase(phi);
		layers[1].erase(phi->getDual());
		removeNode(phi->getDual());
		removeNode(phi);
		delete(phi->getDual());
		delete(phi);
	}
}

void algebraReduction(vector<set<Node*>> &layers) {
	set<Node*> Q;
	set<Node*> Lambda;

	for (Node* constant : layers[2]) {
		Lambda.insert(constant);
	}

	do {
		Node* c = *Lambda.begin();
		Lambda.erase(c);

		set<Node*> Sc = intersect(Q, c->getLowerSet());
		set<Node*> Wc;
		set<Node*> Phic;

		if (Sc.size() == 0) {
			Wc = layers[6];
		}
		else {
			Node* start = *(Sc).begin();
			Wc = start->getDual()->getLowerSet(layers[6]);
			for (Node* phi : Sc) {
				if (phi == start)
					continue;
				Wc = intersect(Wc, phi->getDual()->getLowerSet(layers[6]));
			}
		}

		for (Node* phi : c->getLowerSet(layers[0])) {
			Phic.insert(phi->getDual());
		}
		set<Node*> tr = c->getTrace(layers[0], layers[6]);
		while (!isSubset(Wc, tr) || !isSubset(tr, Wc)) {
			set<Node*> temp = difference(Wc, tr);

			Node* xi = *(temp).begin();
			set<Node*> temp2 = difference(Phic, xi->getUpperSet());
			Node* dualphi = *(temp2).begin();
			Q.insert(dualphi->getDual());
			Wc = intersect(Wc, dualphi->getLowerSet(layers[6]));
		}
	} while (Lambda.size() != 0);

	for (Node* phi : difference(layers[0], Q)) {
		if (phi->getName() == "Base") {
			continue;
		}
		removeUpper(phi);
		removeLower(phi->getDual());
		layers[0].erase(phi);
		layers[1].erase(phi->getDual());
		delete(phi->getDual());
		delete(phi);
	}

	return;
}