#include <iostream>
#include <vector>
#include <limits>

#define NIL numeric_limits<int>::min()

using namespace std;

class vEB {
public:
	vEB(int);
	int u, upper_sqrt_u, lower_sqrt_u;
	int min, max;
	vEB* summary;
	vector<vEB*> cluster;
	int high(int);
	int low(int);
	int index(int, int);
};

vEB::vEB(int _u) : u(_u), min(NIL), max(NIL) { // _u should be exact power of 2
	int i = -1; //calculate lg(n)
	while (_u) {
		_u >>= 1;
		++i;
	}
	upper_sqrt_u = 1 << ((i + 1) / 2);
	lower_sqrt_u = 1 << (i / 2);
	if (u == 2) {
		summary = nullptr;
		return;
	} else {
		summary = new vEB(upper_sqrt_u);
		cluster.assign(upper_sqrt_u, nullptr);
		for (auto& a : cluster)
			a = new vEB(lower_sqrt_u);
	}
}
int vEB::high(int x) {
	return x / lower_sqrt_u;
}

int vEB::low(int x) {
	return x % lower_sqrt_u;
}

int vEB::index(int x, int y) {
	return x * lower_sqrt_u + y;
}


class vEB_tree {
public:
	vEB_tree(int);
	void insert(int);
	void deletee(int);
	int minimum();
	int maximum();
	bool member(int);
	int predecessor(int);
	int successor(int);
private:
	vEB* root;
	int u;
	void _insert(vEB*, int);
	void _deletee(vEB*, int);
	int _predecessor(vEB*, int);
	int _successor(vEB*, int);
};

vEB_tree::vEB_tree(int u) : u(u) {
	int _u = 1;
	while (u > _u) {
		_u <<= 1;
	}
	root = new vEB(_u);

}

void vEB_tree::insert(int x) {
	_insert(root, x);
}

void vEB_tree::deletee(int x) {
	_deletee(root, x);
}

int vEB_tree::minimum() {
	return root -> min;
}

int vEB_tree::maximum() {
	return root -> max;
}

bool vEB_tree::member(int x) {
	auto V = root;
	while (true) {
		if (x == V -> min || x == V -> max) return true;
		else if (V -> u == 2) return false;
		else {
			x = V -> low(x);
			V = V -> cluster[V -> high(x)];
		}
	}
}

int vEB_tree::predecessor(int x) {
	return _predecessor(root, x);
}

int vEB_tree::successor(int x) {
	return _successor(root, x);
}

void vEB_tree::_insert(vEB* V, int x) {
	if (V -> min == NIL) {
		V -> min = V -> max = x;
	} else {
		if (x < V -> min)
			swap(x, V -> min);
		if (V -> u > 2) {
			if (V -> cluster[V -> high(x)] -> min == NIL) {
				_insert(V -> summary, V -> high(x));
				V -> cluster[V -> high(x)] -> min = V -> cluster[V -> high(x)] -> max = V -> low(x);
			} else {
				_insert(V -> cluster[V -> high(x)], V -> low(x));
			}
		}
		if (x > V -> max)
			V -> max = x;
	}
}

void vEB_tree::_deletee(vEB* V, int x) {
	
}

int vEB_tree::_predecessor(vEB* V, int x) {

}

int vEB_tree::_successor(vEB* V, int x) {
	if (V -> u == 2) {
		if (x == 0 && V -> max == 1) return 1;
		else return NIL;
	} else if (V -> min != NIL && x < V -> min) {
		return V -> min;
	} else {
		auto max_low = V -> cluster[V -> high(x)] -> max;
		if (max_low != NIL && V -> low(x) < max_low) {
			auto offset = _successor(V -> cluster[V -> high(x)], V -> low(x));
			return V -> index(V -> high(x), offset);
		} else {
			auto succ_cluster = _successor(V -> summary, V -> high(x));
			if (succ_cluster == NIL) return NIL;
			else {
				auto offset = V -> cluster[succ_cluster] -> min;
				return V -> index(succ_cluster, offset);
			}
		}
	}
}

int main() {
	int u;
	cin >> u;
	vEB_tree V(u);
	
	string op;
	while(cin >> op) {
		int x;
		if (op == "INSERT"s) {
			cin >> x;
			V.insert(x);
		} else if (op == "DELETE"s) {
			cin >> x;
			//V.deletee(x);
		} else if (op == "MEMBER"s) {
			cin >> x;
			cout << V.member(x) << endl;
		} else if (op == "PREDECESSOR"s) {
			cin >> x;
			auto p = V.predecessor(x);
			if (p == NIL) cout << "NONE" << endl;
			else cout << p << endl;
		} else if (op == "SUCCESSOR"s) {
			cin >> x;
			auto s = V.successor(x);
			if (s == NIL) cout << "NONE" << endl;
			else cout << s << endl;
		}
	}
	return 0;
}
