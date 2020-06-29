#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

enum class Color : char {
	BLACK, RED
};

class Node {
public:
	Node(Color, int, int);
	Color color;
	int key;
	Node *left, *right, *p;
	int high;
	int max;
};

Node::Node(Color c, int l, int h) : color(c), key(l), high(h), max(h) {}

class Interval { // a red-black tree
public:
	Interval();
	~Interval();
	void print();
	bool insert(int, int);
	void deletee(int, int);
	Node* search(int, int);
	vector<Node*> interval_search(int, int);
private:
	vector<Node*> _interval_search(Node*, int, int);
	Node* minimum(Node*);
	void left_rotate(Node*);
	void right_rotate(Node*);
	void insert_fixup(Node*);
	void transplant(Node*, Node*);
	void delete_fixup(Node*);
	void update_max(Node*);
	Node *root, *NIL;
};

Interval::Interval() {
	NIL = new Node(Color::BLACK, numeric_limits<int>::min(), numeric_limits<int>::min());
	root = NIL;
}

Interval::~Interval() {
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

void Interval::print() {
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
	auto w = (n + 1) << 3; // pow(2, h + k);
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
				out = "["s + to_string(vec[i] -> key) + ", "s + to_string(vec[i] -> high) + "]"s;
				out += "/"s + to_string(vec[i] -> max);
				if (out.length() % 2) out = " "s + out;
				out += " "s;
				if (vec[i] -> color == Color::BLACK) out += "B"s;
				else out += "R"s;
			}

			for (auto k = 0; k < w - out.length() / 2; ++k) cout << " ";
			cout << out;
			for (auto k = 0; k < w - out.length() / 2; ++k) cout << " ";
		}
		cout << endl;
	}
}

bool Interval::insert(int low, int high) {
	auto y = NIL;
	auto x = root;
	while(x != NIL) {
		y = x;
		y -> max = max(y -> max, high); // update max
		if (low < x -> key || (low == x -> key && high < x -> high)) x = x -> left;
		else x = x -> right;
	}

	Node *z = new Node(Color::RED, low, high);
	z -> p = y;
	if (y == NIL) root = z;
	else if (z -> key < y -> key) y -> left = z;
	else y -> right = z;
	z -> left = z -> right = NIL;

	insert_fixup(z);
	return true;
}

void Interval::deletee(int low, int high) {
	auto z = search(low, high);
	if (z == NIL) return;

	Node *x;
	auto y = z;
	auto y_original_color = y -> color;
	if (z -> left == NIL) {
		x = z -> right;
		transplant(z, x);
		update_max(x -> p);
	} else if (z -> right == NIL) {
		x = z -> left;
		transplant(z, x);
		update_max(x -> p);
	} else {
		y = minimum(z -> right);
		y_original_color = y -> color;
		x = y -> right;
		if (y -> p != z) {
			transplant(y, x);
			update_max(x -> p);
			y -> right = z -> right;
			y -> right -> p = y;
		}
		transplant(z, y);
		y -> left = z -> left;
		y -> left -> p = y;
		y -> color = z -> color;
		update_max(y);
	}
	delete z;
	if (y_original_color == Color::BLACK)
		delete_fixup(x);
}

Node* Interval::search(int low, int high) {
	auto x = root;
	while (x != NIL) {
		if (x -> key == low && x -> high == high) break;
		else if (low < x -> key || (low == x -> key && high < x -> high)) x = x -> left;
		else x = x -> right;
	}
	return x;
}

vector<Node*> Interval::interval_search(int low, int high) {
	return _interval_search(root, low, high);
}

vector<Node*> Interval::_interval_search(Node* x, int low, int high) {
	vector<Node*> ret;
	if (x == NIL) return ret;

	if (x -> left -> max >= low) {
		auto vec = _interval_search(x -> left, low, high);
		for (const auto& p : vec)
			ret.push_back(p);
	}
	if (x -> key <= high && low <= x -> high) ret.push_back(x);
	if (x -> right -> max >= low) {
		auto vec = _interval_search(x -> right, low, high);
		for (const auto& p : vec)
			ret.push_back(p);
	}

	return ret;
}

Node* Interval::minimum(Node* x) {
	if (x == NIL) return NIL;
	while(x -> left != NIL)
		x = x -> left;
	return x;
}

void Interval::left_rotate(Node* x) {
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

	// update max
	x -> max = max(x -> high, max(x -> left -> max, x -> right -> max));
	y -> max = max(y -> max, x -> max);
}

void Interval::right_rotate(Node* y) {
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
	
	y -> max = max(y -> high, max(y -> left -> max, y -> right -> max));
	x -> max = max(x -> max, y -> max);
}

void Interval::transplant(Node* u, Node* v) {
	if (u -> p == NIL) root = v;
	else if (u == u -> p -> left) u -> p -> left = v;
	else u -> p -> right = v;
	v -> p = u -> p;
}

void Interval::insert_fixup(Node* z) {
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

void Interval::delete_fixup(Node* x) {
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

void Interval::update_max(Node* x) {
	while (x != NIL) {
		auto x_old_max = x -> max;
		x -> max = max(x -> high, max(x -> left -> max, x -> right -> max));
		if (x -> max == x_old_max) break;
		x = x -> p;
	}
}

int main() {
	Interval interval;

	string op;
	while(cin >> op) {
		if (op == "INSERT") {
			int low, high;
			cin >> low >> high;
			interval.insert(low, high);
		} else if (op == "DELETE") {
			int low, high;
			cin >> low >> high;
			interval.deletee(low, high);
		} else if (op == "INTERVAL_SEARCH") {
			int low, high;
			cin >> low >> high;
			auto rst = interval.interval_search(low, high);

			cout << "Overlapped with [" << low << ", " << high << "] : " << endl;
			for (const auto& p : rst) {
				cout << "[" << p -> key << ", " << p -> high << "]" << endl;
			}
			cout << endl;
		}
	}
	cout << endl;
	interval.print();
	return 0;
}
