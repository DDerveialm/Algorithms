#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

enum class Color : char {
	BLACK, RED
};

class Node {
public:
	Node(Color, int, string);
	Color color;
	int key;
	Node *left, *right, *p;
	string data;
};

Node::Node(Color c, int k = 0, string s = ""s) : color(c), key(k), data(s) {}

class RBT {
public:
	RBT();
	~RBT();
	void print();
	bool insert(int, string);
	void deletee(int);
	Node* search(int);
private:
	Node* minimum(Node*);
	void left_rotate(Node*);
	void right_rotate(Node*);
	void insert_fixup(Node*);
	void transplant(Node*, Node*);
	void delete_fixup(Node*);
	Node *root, *NIL;
};

RBT::RBT() {
	NIL = new Node(Color::BLACK);
	root = NIL;
}

RBT::~RBT() {
	queue<Node*> q;
	if (root != NIL)
		q.push(root);
	while(!q.empty()) {
		auto x = q.front();
		q.pop();
		if (x -> left != NIL) q.push(x -> left);
		if (x -> right != NIL) q.push(x -> right);
		delete x;
	}
	delete NIL;
}

void RBT::print() {
	vector<Node*> vec;
	vec.push_back(root);
	auto h = 1;
	auto n = 1;
	auto flag = true;
	while(flag) {
		flag = false;
		++h;
		auto last_n = n;
		++n; // n = pow(2, h) - 1
		n <<= 1;
		--n;
		vec.reserve(n);
		for (auto i = last_n; i < n; ++i) {
			auto p = (i - 1) / 2;
			if (vec[p] == NIL) vec.push_back(NIL);
			else {
				if ((i - 1) % 2 == 0) vec.push_back(vec[p] -> left);
				else vec.push_back(vec[p] -> right);
				if (vec[i] != NIL) flag = true;
			}
		}
		if (!flag) {
			for (int i = last_n; i < n; ++i)
				vec.pop_back();
			--h;
			n = last_n;
		}
	}
	auto w = (n + 1) << 2; // pow(2, h + k);
	n = 0;
	for (auto j = 1; j <= h; ++j) {
		auto last_n = n;
		++n;
		n <<= 1;
		--n;

		w >>= 1;
		
		for (auto i = last_n; i < n; ++i) {
			string out;
			if (vec[i] != NIL) {
				out = to_string(vec[i] -> key);
				if (out.length() % 2) out = " "s + out;
				out += " "s;
				if (vec[i] -> color == Color::BLACK) out += "BLACK"s;
				else out += "RED"s;
			}

			cout << "'";
			for (auto k = 1; k < w - out.length() / 2; ++k) cout << " ";
			cout << out;
			for (auto k = 1; k < w - out.length() / 2; ++k) cout << " ";
			cout << "'";
		}
		cout << endl;
	}
}

bool RBT::insert(int k, string s) {
	auto y = NIL;
	auto x = root;
	while(x != NIL) {
		y = x;
		if (k == x -> key) return false; // cannot insert with existing key
		else if (k < x -> key) x = x -> left;
		else x = x -> right;
	}

	Node *z = new Node(Color::RED, k, s);
	z -> p = y;
	if (y == NIL) root = z;
	else if (z -> key < y -> key) y -> left = z;
	else y -> right = z;
	z -> left = z -> right = NIL;
	
	insert_fixup(z);
	return true;
}

void RBT::deletee(int k) {
	auto z = search(k);
	if (z == NIL) return ;

	Node *x;
	auto y = z;
	auto y_original_color = y -> color;
	if (z -> left == NIL) {
		x = z -> right;
		transplant(z, x);
	} else if (z -> right == NIL) {
		x = z -> left;
		transplant(z, x);
	} else {
		y = minimum(z -> right);
		y_original_color = y -> color;
		x = y -> right;
		if (y -> p != z) {
			transplant(y, x);
			y -> right = z -> right;
			y -> right -> p = y;
		}
		transplant(z, y);
		y -> left = z -> left;
		y -> left -> p = y;
		y -> color = z -> color;
	}
	delete z;
	if (y_original_color == Color::BLACK)
		delete_fixup(x);
}

Node* RBT::search(int k) {
	auto x = root;
	while (x != NIL) {
		if (x -> key == k) break;
		else if (x -> key > k) x = x -> left;
		else x = x -> right;
	}
	return x;
}

Node* RBT::minimum(Node* x) {
	if (x == NIL) return NIL;
	while(x -> left != NIL)
		x = x -> left;
	return x;
}

void RBT::left_rotate(Node* x) {
	if (x -> right == NIL) return ;
	auto y = x -> right; // set y

	x -> right = y -> left; // turn y's left subtree into x's right subtree
	if (y -> left != NIL) y -> left -> p = x;

	y -> p = x -> p; // link x's parent to y
	if (x -> p == NIL) root = y;
	else if (x == x -> p -> left) x -> p -> left = y;
	else x -> p -> right = y;

	y -> left = x; // put x on y's left
	x -> p = y;
}

void RBT::right_rotate(Node* y) {
	if (y -> left == NIL) return ;
	auto x = y -> left;

	y -> left = x -> right;
	if (x -> right != NIL) x -> right -> p = y;

	x -> p = y -> p;
	if (y -> p == NIL) root = x;
	else if (y == y -> p -> left) y -> p -> left = x;
	else y -> p -> right = x;

	x -> right = y;
	y -> p = x;
}

void RBT::transplant(Node* u, Node* v) {
	if (u -> p == NIL) root = v;
	else if (u == u -> p -> left) u -> p -> left = v;
	else u -> p -> right = v;
	v -> p = u -> p;
}

void RBT::insert_fixup(Node* z) {
	while(z -> p -> color == Color::RED) {
		if (z -> p == z -> p -> p -> left) {
			auto y = z -> p -> p -> right;
			if (y -> color == Color::RED) {
				z -> p -> color = Color::BLACK; // case 1
				y -> color = Color::BLACK;
				z -> p -> p -> color = Color::RED;
				z = z -> p -> p;
			} else {
				if (z == z -> p -> right) {
					z = z -> p; // case 2
					left_rotate(z);
					// transform case 2 into case 3
				}
				z -> p -> color = Color::BLACK; // case 3
				z -> p -> p -> color = Color::RED;
				right_rotate(z -> p -> p);
			}
		} else {
			// same but with "right" and "left" exchanged
			auto y = z -> p -> p -> left;
			if (y -> color == Color::RED) {
				z -> p -> color = Color::BLACK;
				y -> color = Color::BLACK;
				z -> p -> p -> color = Color::RED;
				z = z -> p -> p;
			} else {
				if (z == z -> p -> left) {
					z = z -> p;
					right_rotate(z);
				}
				z -> p -> color = Color::BLACK;
				z -> p -> p -> color = Color::RED;
				left_rotate(z -> p -> p);
			}
		}
	}
	root -> color = Color::BLACK;
}

void RBT::delete_fixup(Node* x) {
	while(x != root && x -> color == Color::BLACK) {
		if (x == x -> p -> left) {
			auto w = x -> p -> right;
			if (w -> color == Color::RED) {
				w -> color = Color::BLACK; // case 1
				x -> p -> color == Color::RED;
				left_rotate(x -> p);
				w = x -> p -> right;
			}
			if (w -> left -> color == Color::BLACK && w -> right -> color == Color::BLACK) {
				w -> color == Color::RED; // case 2
				x = x -> p;
			} else {
				if (w -> right -> color == Color::BLACK) {
					w -> left -> color = Color::BLACK; // case 3
					w -> color == Color::RED;
					right_rotate(w);
					w = x -> p -> right;
				}
				w -> color = x -> p -> color; // case 4
				x -> p -> color = Color::BLACK;
				w -> right -> color = Color::BLACK;
				left_rotate(x -> p);
				x = root; // in order to break
			}
		} else {
			// same but with "right" and "left" excahnged
			auto w = x -> p -> left;
			if (w -> color == Color::RED) {
				w -> color = Color::BLACK;
				x -> p -> color == Color::RED;
				right_rotate(x -> p);
				w = x -> p -> left;
			}
			if (w -> right -> color == Color::BLACK && w -> left -> color == Color::BLACK) {
				w -> color == Color::RED;
				x = x -> p;
			} else {
				if (w -> left -> color == Color::BLACK) {
					w -> right -> color = Color::BLACK;
					w -> color == Color::RED;
					left_rotate(w);
					w = x -> p -> left;
				}
				w -> color = x -> p -> color;
				x -> p -> color = Color::BLACK;
				w -> left -> color = Color::BLACK;
				right_rotate(x -> p);
				x = root;
			}
		}
	}
	x -> color = Color::BLACK;
}

int main() {
	RBT t;

	string op;
	while(cin >> op) {
		if (op == "INSERT") {
			int key;
			string data;
			cin >> key >> data;
			//cout << "INSERT " << key << " - " << data << endl;
			t.insert(key, data);
		} else if (op == "DELETE") {
			int key;
			cin >> key;
			//cout << "delete " << key << endl;
			t.deletee(key);
		} else if (op == "SEARCH") {
			int key;
			cin >> key;
			cout << (t.search(key) -> data);
		}
	}
	cout << endl;
	t.print();
	return 0;
}
