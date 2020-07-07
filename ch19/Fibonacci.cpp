#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <sstream>

using namespace std;

class Node {
public:
	Node(int, string);
	void form_dll();
	void merge_dll(Node*);
	void remove_from_dll();
	vector<Node*> get_all_nodes();
	Node* p;
	Node *left, *right;
	Node* child;
	int degree;
	bool mark;
	int key;
	string value;
};

Node::Node(int key, string value) : p(nullptr), left(this), right(this), child(nullptr), degree(0), mark(false) , key(key), value(value){}

void Node::form_dll() {
	left = right = this;
}

void Node::merge_dll(Node* x) {
	x -> left -> right = right;
	right -> left = x -> left;
	right = x;
	x -> left = this;
}

void Node::remove_from_dll() {
	left -> right = right;
	right -> left = left;
}

vector<Node*> Node::get_all_nodes() {
	vector<Node*> vec;
	auto x = this;
	do {
		vec.push_back(x);
		x = x -> right;
	} while (x != this);
	return vec;
}

class Fibonacci_heap {
public:
	Fibonacci_heap();
	void insert(Node*);
	Node* minimum();
	Node* extract_min();
	void unionn(Fibonacci_heap*);
	void decrease_key(Node*, int);
	void deletee(Node*);
	string print();
private:
	Node* min;
	int n;
	void consolidate();
	void link(Node*, Node*);
	void cut(Node*, Node*);
	void cascading_cut(Node*);
};

Fibonacci_heap::Fibonacci_heap() : min(nullptr), n(0) {}

void Fibonacci_heap::insert(Node* x) {
	if (min == nullptr) {
		min = x;
	} else {
		min -> merge_dll(x);
		if (x -> key < min -> key)
			min = x;
		++n;
	}
}

Node* Fibonacci_heap::minimum() {
	return min;
}

Node* Fibonacci_heap::extract_min() {
	auto z = min;
	if (z != nullptr) {
		auto c = z -> child;
		if (c) {
			auto all = c -> get_all_nodes();
			for (auto& i : all) i -> p = nullptr;
			z -> merge_dll(c); // add z -> child into root list
		}
		z -> remove_from_dll();
		if (z == z -> right) min = nullptr;
		else {
			min = z -> right;
			consolidate();
		}
		--n;
	}
	return z;
}

void Fibonacci_heap::unionn(Fibonacci_heap* h) {
	min -> merge_dll(h -> min);
	if (min == nullptr || (h -> min != nullptr && h -> min -> key < min -> key))
		min = h -> min;
	n += h -> n;
}

void Fibonacci_heap::decrease_key(Node* x, int k) {
	if (k >= x -> key) return ;
	x -> key = k;
	auto y = x -> p;
	if (y != nullptr && x -> key < y -> key) {
		cut(y, x);
		cascading_cut(y);
	}
	if (x -> key < min -> key)
		min = x;
}

void Fibonacci_heap::deletee(Node* x) {
	decrease_key(x, numeric_limits<int>::min());
	delete(extract_min());
}

string Fibonacci_heap::print() {
	stringstream buffer;
	if (!min) return string();
	queue<Node*> q1, q2;
	auto all = min -> get_all_nodes();
	for (const auto& i : all) {
		if (i -> child) q1.push(i -> child);
		buffer << "root : " << i -> key << endl;
		while(!q1.empty()) {
			while(!q1.empty()) {
				auto _all = q1.front() -> get_all_nodes();
				q1.pop();
				buffer << "'";
				for (const auto& j : _all) {
					buffer << j -> key << " ";
					if (j -> child) q2.push(j -> child);
				}
				buffer << "' ";
			}
			buffer << endl;
			q1.swap(q2);
		}
	}
	return buffer.str();
}

void Fibonacci_heap::consolidate() {
	auto D = floor(log(n)/log(2)); //log2(D)
	vector<Node*> A(D + 1, nullptr);
	auto all = min -> get_all_nodes();
	for (const auto& w : all) {
		auto x = w;
		auto d = x -> degree;
		while (A[d]) {
			auto y = A[d];
			if (x -> key > y -> key)
				swap(x, y);
			link(y, x);
			A[d] = nullptr;
			++d;
		}
		A[d] = x;
	}
	min = nullptr;
	for (int i = 0; i < A.size(); ++i) {
		if (A[i] != nullptr)
			if (min == nullptr) {
				A[i] -> form_dll();
				min = A[i];
			} else {
				A[i] -> form_dll();
				min -> merge_dll(A[i]);
				if (A[i] -> key < min -> key)
					min = A[i];
			}
	}
}

void Fibonacci_heap::link(Node* y, Node* x) {
	y -> remove_from_dll();
	y -> form_dll();
	y -> p = x;
	y -> mark = false;
	if (x -> child) x -> child -> merge_dll(y);
	else x -> child = y;
	++x -> degree;
}

void Fibonacci_heap::cut(Node* y, Node* x) {
	x -> remove_from_dll();
	if (x == x -> right) y -> child = nullptr;
	else y -> child = x -> right;
	--y -> degree;
	x -> p = nullptr;
	x -> mark = false;
	x -> form_dll();
	min -> merge_dll(x);
}

void Fibonacci_heap::cascading_cut(Node* y) {
	auto z = y -> p;
	if (z != nullptr)
		if (y -> mark == false)
			y -> mark = true;
		else {
			cut(z, y);
			cascading_cut(z);
		}
}

int main() {
	Fibonacci_heap fh;

	vector<Node*> record;
	string op;
	string buffer;
	while (cin >> op) {
		if (op == "INSERT"s) {
			int key;
			string value;
			cin >> key >> value;
			record.push_back(new Node(key, value));
			//cout << "INSERT " << key << " " << value << endl;
			fh.insert(record.back());
		} else if (op == "EXTRACT_MIN"s) {
			//cout << "EXTRACT_MIN" << endl;
			auto m = fh.extract_min();
			if (m) {
				cout << m -> value;
				delete(m);
			}
		} else if (op == "DECREASE_KEY"s) {
			int index, key;
			cin >> index >> key;
			//cout << "DECREASE_KEY " << index << " " << key << endl;
			fh.decrease_key(record.at(index), key);
		} else if (op == "DELETE"s) {
			int index;
			cin >> index;
			//cout << "DELETE " << index << endl;
			fh.deletee(record.at(index));
		} else if (op == "PRINT"s) {
			buffer = fh.print();
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cout << endl;
	cout << buffer;
	return 0;
}
