#include <iostream>
#include <vector>
#include <iterator>
#include <queue>

using namespace std;

class Node {
public:
	vector<int> key;
	vector<string> data;
	vector<Node*> child;
	bool is_leaf;
};

class B_tree {
public:
	B_tree(int);
	void insert(int, string);
	void deletee(int);
	pair<Node*, int> search(int);
	void print();
private:
	Node* root;
	int t;
	void disk_read(Node*);
	void disk_write(Node*);
	void split_child(Node*, int);
	void merge(Node*, int);
	pair<Node*, int> maximum(Node*);
	pair<Node*, int> minimum(Node*);
};

B_tree::B_tree(int t = 2) : t(t) {
	root = new Node();
	root -> is_leaf = true;
	disk_write(root);
}

void B_tree::insert(int k, string d) {
	if (root -> key.size() == 2 * t - 1) {
		auto s = new Node();
		s -> is_leaf = false;
		s -> child.push_back(root);
		root = s;
		split_child(s, 0);
	}
	auto x = root;
	while(!x -> is_leaf) {
		int i = 0;
		while(i < x -> key.size() && k >= x -> key[i])
			++i;
		disk_read(x -> child[i]);
		if (x -> child[i] -> key.size() == 2 * t - 1) {
			split_child(x, i);
			if (k >= x -> key[i]) ++i;
		}
		x = x -> child[i];
	}
	int i = x -> key.size() - 1;
	x -> key.push_back(int{});
	x -> data.push_back(string{});
	while(i >= 0 && k < x -> key[i]) {
		x -> key[i + 1] = x -> key[i];
		x -> data[i + 1] = x -> data[i];
		--i;
	}
	++i;
	x -> key[i] = k;
	x -> data[i] = d;
	disk_write(x);
}

void B_tree::deletee(int k) {
	auto x = root;
	while (!x -> is_leaf) {
		int i = 0;
		while(i < x -> key.size() && k > x -> key[i]) ++i;
		if (i == x -> key.size() || k < x -> key[i]) {
			auto y = x -> child[i];
			if (y -> key.size() == t - 1) {
				Node *s1 = nullptr, *s2 = nullptr;
				if (i - 1 >= 0) s1 = x -> child[i - 1];
				if (i + 1 < x -> child.size()) s2 = x -> child[i + 1];
				if (s1 && s1 -> key.size() >= t) { // 3a
					//cout << "case 3a - 1" << endl;
					y -> key.insert(y -> key.begin(), x -> key[i - 1]);
					y -> data.insert(y -> data.begin(), x -> data[i - 1]);
					if (!s1 -> is_leaf) y -> child.insert(y -> child.begin(), s1 -> child.back());
					x -> key[i - 1] = s1 -> key.back();
					x -> data[i - 1] = s1 -> data.back();
					s1 -> key.pop_back();
					s1 -> data.pop_back();
					if (!s1 -> is_leaf) s1 -> child.pop_back();
				} else if (s2 && s2 -> key.size() >= t) {
					//cout << "case 3a - 2" << endl;
					y -> key.push_back(x -> key[i]);
					y -> data.push_back(x -> data[i]);
					if (!s2 -> is_leaf) y -> child.push_back(s2 -> child.front());
					x -> key[i] = s2 -> key.front();
					x -> data[i] = s2 -> data.front();
					s2 -> key.erase(s2 -> key.begin());
					s2 -> data.erase(s2 -> data.begin());
					if (!s2 -> is_leaf) s2 -> child.erase(s2 -> child.begin());
				} else if (s1) { // 3b
					//cout << "case 3b - 1" << endl;
					--i;
					merge(x, i);
					y = x -> child[i];
				} else if (s2) {
					//cout << "case 3b - 2" << endl;
					merge(x, i);
				}
			}
			if (x == root && x -> key.size() == 0) { // happen only in 3b
				//cout << "change root in 3b" << endl;
				root = y;
				delete x;
			}
			x = y;
		} else {
			auto y = x -> child[i];
			auto z = x -> child[i + 1];
			if (y -> key.size() >= t) { // 2a // move k' and replace k by k'
				//cout << "case 2a" << endl;
				auto p = maximum(y); // predecessor
				x -> key[i] = p.first -> key[p.second];
				x -> data[i] = p.first -> data[p.second];
				x = y;
				k = p.first -> key[p.second];
			} else if (z -> key.size() >= t) { // 2b
				//cout << "case 2b" << endl;
				auto s = minimum(z); // successor
				x -> key[i] = s.first -> key[s.second];
				x -> data[i] = s.first -> data[s.second];
				x = z;
				k = s.first -> key[s.second];
			} else { // 2c // merge k and z into y
				//cout << "case 2c" << endl;
				auto median = x -> key[i];
				merge(x, i);
				if (x == root && x -> key.size() == 0) {
					//cout << "change root in 2c" << endl;
					root = y;
					delete x;
				}
				x = y;
				k = median;
			}	
		}
	}
	int i = 0;
	while(i < x -> key.size() && k > x -> key[i]) ++i;
	if (k != x -> key[i]) return; // nothing to delete
	//cout << "case 1" << endl;
	for (int j = i; j < x -> key.size() - 1; ++j) {// 1 // delete from a leaf node
		x -> key[j] = x -> key[j + 1];
		x -> data[j] = x -> data[j + 1];
	}
	x -> key.pop_back();
	x -> data.pop_back();
}

pair<Node*, int> B_tree::search(int k) {
	auto x = root;
	while(true) {
		int i = 0;
		while( i < x -> key.size() && k >= x -> key[i]) {
			if (k == x -> key[i]) return {x, i};
			++i;
		}
		if (!x -> is_leaf) {
			x = x -> child[i];
			disk_read(x);
		}
		else break;
	}
	return {nullptr, -1};
}

void B_tree::print() {
	queue<Node*> q, q2;
	q.push(root);
	int w = 1 << 6;
	while(!q.empty()) {
		w >>= 3;
		while(!q.empty()) {
			auto x = q.front();
			q.pop();
			for (int i = 0; i < w; ++i) cout << " ";
			cout << "'";
			for (const auto& k : x -> key) {
				cout << k;
				if (k != x -> key.back()) cout << " ";
			}
			cout << "' ";
			for (int i = 0; i < w; ++i) cout << " ";
			for (const auto& c : x -> child)
				q2.push(c);
		}
		cout << endl;
		q.swap(q2);
	}
}

void B_tree::disk_read(Node* x) {}

void B_tree::disk_write(Node* x) {}

void B_tree::split_child(Node* x, int i) { // assume x is not full and y is full
	auto y = x -> child[i];
	auto z = new Node();

	x -> key.push_back(int{});
	x -> data.push_back(string{});
	for (int j = x -> key.size() - 1; j > i; --j) { // move key from [i, end) to [i+1, end+1)
		x -> key[j] = x -> key[j - 1];
		x -> data[j] = x -> data[j - 1];
	}
	x -> key[i] = y -> key[t - 1]; // put y's median key in key[i]
	x -> data[i] = y -> data[t - 1];
	x -> child.push_back((Node*){});
	for (int j = x -> child.size() - 1; j > i + 1; --j) // move child from [i+1, end) to [i+2, end)
		x -> child[j] = x -> child[j - 1];
	x -> child[i + 1] = z; // put z in child[i+1]
	
	z -> is_leaf = y -> is_leaf;
	auto it = y -> key.begin();
	advance(it, t - 1);
	z -> key.assign(next(it), y -> key.end());
	y -> key.assign(y -> key.begin(), it);
	auto it2 = y -> data.begin();
	advance(it2, t - 1);
	z -> data.assign(next(it2), y -> data.end());
	y -> data.assign(y -> data.begin(), it2);
	if (!y -> is_leaf) {
		auto it = y -> child.begin();
		advance(it, t);
		z -> child.assign(it, y -> child.end());
		y -> child.assign(y -> child.begin(), it);
	}
	disk_write(x);
	disk_write(y);
	disk_write(z);
}

void B_tree::merge(Node* x, int i) {
// assume y and z exists and the number of keys are both t - 1
// assume x has more than (t - 1) keys
	auto y = x -> child[i];
	auto z = x -> child[i + 1];
	y -> key.push_back(x -> key[i]);
	y -> data.push_back(x -> data[i]);
	for (const auto& zk : z -> key)
		y -> key.push_back(zk);
	for (const auto& zd : z -> data)
		y -> data.push_back(zd);
	for (const auto& zc : z -> child)
		y -> child.push_back(zc);
	delete(z);
	auto it = x -> key.begin(); // remove from x
	advance(it, i);
	x -> key.erase(it);
	auto it2 = x -> data.begin();
	advance(it2, i);
	x -> data.erase(it2);
	auto it3 = x -> child.begin();
	advance(it3, i + 1);
	x -> child.erase(it3);
}

pair<Node*, int> B_tree::minimum(Node* x) {
	while(!x -> is_leaf)
		x = x -> child.front();
	return {x, 0};
}

pair<Node*, int> B_tree::maximum(Node* x) {
	while(!x -> is_leaf)
		x = x -> child.back();
	return {x, x -> key.size() - 1};
}

int main() {
	B_tree bt;
	
	string op;
	while (cin >> op) {
		int key;
		if (op == "INSERT"s) {
			cin >> key;
			string data;
			cin >> data;
			bt.insert(key, data);
		} else if (op == "DELETE"s) {
			cin >> key;
			bt.deletee(key);
		} else if (op == "SEARCH"s) {
			cin >> key;
			auto s = bt.search(key);
			if (s.first) cout << s.first -> data[s.second];
		}
	}
	cout << endl;
	bt.print();
	
	return 0;
}
