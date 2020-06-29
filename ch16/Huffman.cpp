#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <iomanip>

using namespace std;

class Node {
public:
	Node(Node*, Node*);
	Node *left, *right;
	char c;
	double freq;
};

Node::Node(Node* l = nullptr, Node* r = nullptr) : left(l), right(r), c(0), freq(0) {}

class Huffman {
public:
	Huffman(vector<pair<double, Node*>> C);
	void print();
private:
	Node* root;

};

Huffman::Huffman(vector<pair<double, Node*>> C) {
	priority_queue<pair<double, Node*>, vector<pair<double, Node*>>, greater<pair<double, Node*>>> Q(C.begin(), C.end());
	while(Q.size() > 1) {
		auto x = Q.top().second;
		Q.pop();
		auto y = Q.top().second;
		Q.pop();
		auto z = new Node(x, y);
		z -> freq = x -> freq + y -> freq;
		Q.push(make_pair(z -> freq, z));
	}
	root = Q.top().second;
}

void Huffman::print() {
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
			if (vec[p] == nullptr) vec.push_back(nullptr);
			else {
				if ((i - 1) % 2 == 0) vec.push_back(vec[p] -> left);
				else vec.push_back(vec[p] -> right);
				if (vec[i] != nullptr) flag = true;
			}
		}
		if (!flag) {
			for (int i = last_n; i < n; ++i) vec.pop_back();
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
			if (vec[i] != nullptr) {
				if (vec[i] -> left == nullptr && vec[i] -> right == nullptr)
					out = vec[i] -> c + ":"s;
				ostringstream oss;
				oss << setprecision(2) << vec[i] -> freq;
				out += oss.str();
				if (out.length() % 2) out = " "s + out;
			}

			for (auto k = 0; k < w - (int)out.length() / 2; ++k) cout << " ";
			cout << out;
			for (auto k = 0; k < w - (int)out.length() / 2; ++k) cout << " ";
		}
		cout << endl;
	}
}

int main() {
	vector<pair<double, Node*>> C;
	while(true) {
		char c;
		if(!(cin >> c)) break;
		double f;
		if (!(cin >> f)) break;
		auto n = new Node();
		n -> c = c;
		n -> freq = f;
		C.push_back(make_pair(f, n));
	}
	Huffman ht(C);
	ht.print();
	return 0;
}
