#pragma once
#include <set>
#include <string>
#include <vector>
using namespace std;

class Node {
private:
	set<Node*> upper;
	set<Node*> lower;
	Node* dual;
	vector<vector<int>> arr;
	string name;

public:
	Node(string name);
	Node(vector<vector<int>> arr);
	void linkDual(Node *dual);
	void removeReferences(Node* node);
	Node* getDual();
	void addUpper(Node *upper);
	set<Node*> getUpper();
	void addLower(Node *lower);
	set<Node*> getLower();
	set<Node*> getUpperSet();
	set<Node*> getUpperSet(set<Node*> constraints);
	set<Node*> getLowerSet();
	set<Node*> getLowerSet(set<Node*> constraints);
	set<Node*> getTrace(set<Node*> atoms, set<Node*> dualAtoms);
	vector<vector<int>> getArray();
	void setName(string name);
	string getName();
};

void Union(set<Node*> &a, set<Node*> &b);
set<Node *> intersect(set<Node *> const &a, set<Node *> const &b);
bool isSubset(set<Node*> const &a, set<Node*> const &b);
set<Node *> difference(set<Node *> const &a, set<Node *> const &b);
void linkLU(Node* lower, Node* upper);
void linkDuals(Node* one, Node* two);
void removeNode(Node* node);
void removeUpper(Node* node);
void removeLower(Node* node);

