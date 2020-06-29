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
	Node(Color, int);
	Color color;
	int key;
	Node *left, *right, *p;
	int size;
};

Node::Node(Color c, int k = 0) : color(c), key(k) {}

class OS { // a red-black tree
public:
	OS();
	~OS();
	void print();
	bool insert(int);
	void deletee(int);
	Node* search(int);
	Node* select(int);
	int rank(Node*);
private:
	Node* minimum(Node*);
	void left_rotate(Node*);
	void right_rotate(Node*);
	void insert_fixup(Node*);
	void transplant(Node*, Node*);
	void delete_fixup(Node*);
	Node* _select(Node*, int);
	Node *root, *NIL;
};

OS::OS() {
	NIL = new Node(Color::BLACK);
	NIL -> size = 0;
	root = NIL;
}

OS::~OS() {
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

void OS::print() {
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
				out += "/" + to_string(vec[i] -> size);
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

bool OS::insert(int k) {
	auto y = NIL;
	auto x = root;
	while(x != NIL) {
		y = x;
		++y -> size;
		if (k < x -> key) x = x -> left;
		else x = x -> right;
	}

	Node *z = new Node(Color::RED, k);
	z -> size = 1;
	z -> p = y;
	if (y == NIL) root = z;
	else if (z -> key < y -> key) y -> left = z;
	else y -> right = z;
	z -> left = z -> right = NIL;
	
	insert_fixup(z);
	return true;
}

void OS::deletee(int k) {
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
	while(x != root) {
		x = x -> p;
		--x -> size;
	}
	delete z;
	if (y_original_color == Color::BLACK)
		delete_fixup(x);
}

Node* OS::search(int k) {
	auto x = root;
	while (x != NIL) {
		if (x -> key == k) break;
		else if (x -> key > k) x = x -> left;
		else x = x -> right;
	}
	return x;
}

Node* OS::select(int i) {
	return _select(root, i);
}

Node* OS::_select(Node* x, int i) {
	if (x == NIL) return NIL;

	auto r = x -> left -> size + 1;
	if (i == r) return x;
	else if (i < r) return _select(x -> left, i);
	else return _select(x -> right, i - r);
}

int OS::rank(Node* x) {
	if (x == NIL) return 0;

	auto r = x -> left -> size + 1;
	auto y = x;
	while(y != root) {
		if (y == y -> p -> right)
			r = r + y -> p -> left -> size + 1;
		y = y -> p;
	}
	return r;
}

Node* OS::minimum(Node* x) {
	if (x == NIL) return NIL;
	while(x -> left != NIL)
		x = x -> left;
	return x;
}

void OS::left_rotate(Node* x) {
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

	y -> size = x -> size;
	x -> size = x -> left -> size + x -> right -> size + 1;
}

void OS::right_rotate(Node* y) {
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

	x -> size = y -> size;
	y -> size = y -> left -> size + y -> right -> size + 1;
}

void OS::transplant(Node* u, Node* v) {
	if (u -> p == NIL) root = v;
	else if (u == u -> p -> left) u -> p -> left = v;
	else u -> p -> right = v;
	v -> p = u -> p;
}

void OS::insert_fixup(Node* z) {
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

void OS::delete_fixup(Node* x) {
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
	OS os;

	string op;
	while(cin >> op) {
		if (op == "INSERT") {
			int key;
			cin >> key;
			os.insert(key);
		} else if (op == "DELETE") {
			int key;
			cin >> key;
			os.deletee(key);
		} else if (op == "RANK") {
			int key;
			cin >> key;
			cout << key << " - " << os.rank(os.search(key)) << endl;
		} else if (op == "SELECT") {
			int i;
			cin >> i;
			cout << i << " - " << os.select(i) -> key << endl;
		}
	}
	cout << endl;
	os.print();
	return 0;
}
