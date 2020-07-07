#include <iostream>
#include <vector>
#include <iterator>
#include <queue>

using namespace std;

class Data {
public:
	bool operator> (const Data&) const;
	bool operator< (const Data&) const;
	bool operator== (const Data&) const;
	bool operator!= (const Data&) const;
	int key;
	string value;
};

bool Data::operator> (const Data& d) const {return key > d.key;}
bool Data::operator< (const Data& d) const {return key < d.key;}
bool Data::operator== (const Data& d) const {return key == d.key;}
bool Data::operator!= (const Data& d) const {return key != d.key;}

bool operator> (const int& key, const Data& d) {return key > d.key;}
bool operator< (const int& key, const Data& d) {return key < d.key;}
bool operator== (const int& key, const Data& d) {return key == d.key;}
bool operator!= (const int& key, const Data& d) {return key != d.key;}

class Node {
public:
	vector<Data> data;
	vector<Node*> child;
	bool is_leaf;
};

class B_tree {
public:
	B_tree(int);
	bool insert(int, string);
	bool deletee(int);
	pair<bool, const Data&> search(int);
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

bool B_tree::insert(int k, string d) {
	Data inserted = {k, d};
	if (root -> data.size() == 2 * t - 1) {
		auto s = new Node();
		s -> is_leaf = false;
		s -> child.push_back(root);
		root = s;
		split_child(s, 0);
	}
	auto x = root;
	while(!x -> is_leaf) {
		int i = 0;
		while(i < x -> data.size() && inserted > x -> data[i])
			++i;
		if (i < x -> data.size() && inserted == x -> data[i]) return false; // insert with existing key
		disk_read(x -> child[i]);
		if (x -> child[i] -> data.size() == 2 * t - 1) {
			split_child(x, i);
			if (inserted > x -> data[i]) ++i;
		}
		x = x -> child[i];
	}
	for (int i = 0; i < x -> data.size(); ++i)
		if (inserted == x -> data[i]) return false; // insert with existing key
	int i = x -> data.size() - 1;
	x -> data.push_back(Data{});
	while(i >= 0 && inserted < x -> data[i]) {
		x -> data[i + 1] = x -> data[i];
		--i;
	}
	++i;
	x -> data[i] = inserted;
	disk_write(x);
	return true;
}

bool B_tree::deletee(int key) {
	auto x = root;
	while (!x -> is_leaf) {
		int i = 0;
		while(i < x -> data.size() && key > x -> data[i]) ++i;
		if (i == x -> data.size() || key < x -> data[i]) {
			auto y = x -> child[i];
			if (y -> data.size() == t - 1) {
				Node *s1 = nullptr, *s2 = nullptr;
				if (i - 1 >= 0) s1 = x -> child[i - 1];
				if (i + 1 < x -> child.size()) s2 = x -> child[i + 1];
				if (s1 && s1 -> data.size() >= t) { // 3a
					//cout << "case 3a - 1" << endl;
					y -> data.insert(y -> data.begin(), x -> data[i - 1]);
					if (!s1 -> is_leaf) y -> child.insert(y -> child.begin(), s1 -> child.back());
					x -> data[i - 1] = s1 -> data.back();
					s1 -> data.pop_back();
					if (!s1 -> is_leaf) s1 -> child.pop_back();
				} else if (s2 && s2 -> data.size() >= t) {
					//cout << "case 3a - 2" << endl;
					y -> data.push_back(x -> data[i]);
					if (!s2 -> is_leaf) y -> child.push_back(s2 -> child.front());
					x -> data[i] = s2 -> data.front();
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
			if (x == root && x -> data.size() == 0) { // happen only in 3b
				//cout << "change root in 3b" << endl;
				root = y;
				delete x;
			}
			x = y;
		} else {
			auto y = x -> child[i];
			auto z = x -> child[i + 1];
			if (y -> data.size() >= t) { // 2a // move k' and replace k by k'
				//cout << "case 2a" << endl;
				auto p = maximum(y); // predecessor
				x -> data[i] = p.first -> data[p.second];
				x = y;
				key = p.first -> data[p.second].key;
			} else if (z -> data.size() >= t) { // 2b
				//cout << "case 2b" << endl;
				auto s = minimum(z); // successor
				x -> data[i] = s.first -> data[s.second];
				x = z;
				key = s.first -> data[s.second].key;
			} else { // 2c // merge k and z into y
				//cout << "case 2c" << endl;
				auto median = x -> data[i];
				merge(x, i);
				if (x == root && x -> data.size() == 0) {
					//cout << "change root in 2c" << endl;
					root = y;
					delete x;
				}
				x = y;
				key = median.key;
			}	
		}
	}
	int i = 0;
	while(i < x -> data.size() && key > x -> data[i]) ++i;
	if (i >= x -> data.size() || key != x -> data[i]) return false; // nothing to delete
	//cout << "case 1" << endl;
	for (int j = i; j < x -> data.size() - 1; ++j) // 1 // delete from a leaf node
		x -> data[j] = x -> data[j + 1];
	x -> data.pop_back();
	return true;
}

pair<bool, const Data&> B_tree::search(int key) {
	auto x = root;
	while(true) {
		int i = 0;
		while( i < x -> data.size() && key > x -> data[i])
			++i;
		if (i < x -> data.size() && key == x -> data[i]) return {true, x -> data[i]};
		if (!x -> is_leaf) {
			x = x -> child[i];
			disk_read(x);
		} else break;
	}
	return {false, Data{}};
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
			for (const auto& d : x -> data) {
				cout << d.key;
				if (d != x -> data.back()) cout << " ";
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

	x -> data.push_back(Data{});
	for (int j = x -> data.size() - 1; j > i; --j) // move key from [i, end) to [i+1, end+1)
		x -> data[j] = x -> data[j - 1];
	x -> data[i] = y -> data[t - 1]; // put y's median key in key[i]
	x -> child.push_back((Node*){});
	for (int j = x -> child.size() - 1; j > i + 1; --j) // move child from [i+1, end) to [i+2, end)
		x -> child[j] = x -> child[j - 1];
	x -> child[i + 1] = z; // put z in child[i+1]
	
	z -> is_leaf = y -> is_leaf;
	z -> data.assign(y -> data.begin() + t, y -> data.end());
	y -> data.assign(y -> data.begin(), y -> data.begin() + t - 1);
	if (!y -> is_leaf) {
		z -> child.assign(y -> child.begin() + t, y -> child.end());
		y -> child.assign(y -> child.begin(), y -> child.begin() + t);
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
	y -> data.push_back(x -> data[i]);
	for (const auto& zd : z -> data)
		y -> data.push_back(zd);
	for (const auto& zc : z -> child)
		y -> child.push_back(zc);
	delete(z);
	x -> data.erase(x -> data.begin() + i);
	x -> child.erase(x -> child.begin() + i + 1);
}

pair<Node*, int> B_tree::minimum(Node* x) {
	while(!x -> is_leaf)
		x = x -> child.front();
	return {x, 0};
}

pair<Node*, int> B_tree::maximum(Node* x) {
	while(!x -> is_leaf)
		x = x -> child.back();
	return {x, x -> data.size() - 1};
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
			if (s.first) cout << s.second.value;
		}
	}
	cout << endl;
	bt.print();
	
	return 0;
}
