#include "Node.h"
#include <set>
#include <string>
using namespace std;


/*Link two nodes together*/
void linkLU(Node* lower, Node* upper) {
	lower->addUpper(upper);
	upper->addLower(lower);
}

/*Link duals*/
void linkDuals(Node* one, Node* two) {
	one->linkDual(two);
	two->linkDual(one);
}

void Union(set<Node*> &a, set<Node*> &b)
{
	a.insert(b.begin(), b.end());
}

void removeNode(Node* node) {
	for (Node* u : node->getUpper()) {

		for (Node* l : node->getLower()) {
			u->removeReferences(node);
			l->removeReferences(node);
			linkLU(l, u);
		}
	}
}

void removeUpper(Node* node) {
	for (Node* u : node->getUpper()) {
		u->removeReferences(node);
	}
}

void removeLower(Node* node) {
	for (Node* l : node->getLower()) {
		l->removeReferences(node);
	}
}

set<Node *> intersect(set<Node *> const &a, set<Node *> const &b) {
	set<Node *> result;
	for (Node* node : a) {
		if (b.find(node) != b.end()) {
			result.insert(node);
		}
	}

	return result;
}

bool isSubset(set<Node*> const &a, set<Node*> const &b) {
	bool flag;
	for (Node* search : a) {
		flag = true;
		for (Node* val : b) {
			if (val == search) {
				flag = false;
			}
		}
		if (flag) {
			return false;
		}
	}

	return true;
}

set<Node *> difference(set<Node *> const &a, set<Node *> const &b) {
	set<Node *> result;
	bool flag;
	for (Node *node : a) {
		flag = true;
		for (Node *check : b) {
			if (check == node) {
				flag = false;
			}
		}
		if (flag)
			result.insert(node);
	}
	return result;
}

Node::Node(string name) {
	vector<vector<int>> x{ {NULL, NULL}, {NULL, NULL} };;
	this->arr = x;
	this->name = name;
}

Node::Node(vector<vector<int>> arr) {
	this->arr = arr;
	string x = "";
	for (int j = 0; j < 4; ++j) {
		int val = arr[j / 2][j % 2];
		if (val != -1)
			x += to_string(val);
		else
			x += "-";
	}
	this->name = x;
}

void Node::linkDual(Node *dual) {
	this->dual = dual;
}

Node* Node::getDual() {
	return this->dual;
}

vector<vector<int>> Node::getArray() {
	return this->arr;
}

void Node::addLower(Node *lower) {
	this->lower.insert(lower);
}

set<Node*> Node::getLower() {
	return this->lower;
}

void Node::addUpper(Node *upper) {
	this->upper.insert(upper);
}

set<Node*> Node::getUpper() {
	return this->upper;
}

set<Node*> Node::getUpperSet() {
	set<Node*> upperSet;
	if (this->upper.size() == 0) {
		return upperSet;
	}
	set<Node*> recurse;
	for (Node* node : this->upper) {
		upperSet.insert(node);
		recurse = node->getUpperSet();
		Union(upperSet, recurse);
	}
	return upperSet;
}

set<Node*> Node::getUpperSet(set<Node*> constraints) {
	set<Node*> up = this->getUpperSet();
	return intersect(up, constraints);
}

set<Node*> Node::getLowerSet() {
	set<Node*> lowerSet;
	lowerSet.insert(this);
	if (this->lower.size() == 0) {
		return lowerSet;
	}
	set<Node*> recurse;
	for (Node* node : this->lower) {
		recurse = node->getLowerSet();
		Union(lowerSet, recurse);
	}
	return lowerSet;
}

set<Node*> Node::getLowerSet(set<Node*> constraints) {
	set<Node*> low = this->getLowerSet();
	return intersect(low, constraints);
}

set<Node*> Node::getTrace(set<Node*> atoms, set<Node*> dualAtoms) {
	set<Node*> aSet = this->getLowerSet(atoms);
	set<Node*> trace = dualAtoms;
	for (Node* node : aSet) {
		trace = node->getDual()->getLowerSet(trace);
	}

	return trace;
}

void Node::setName(string name) {
	this->name = name;
}

string Node::getName() {
	return this->name;
}

void Node::removeReferences(Node* node) {
	if (this->lower.find(node) != this->lower.end()) {
		this->lower.erase(node);
	}
	if (this->upper.find(node) != this->upper.end()) {
		this->upper.erase(node);
	}
}