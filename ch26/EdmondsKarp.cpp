#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <memory>
#include <tuple>

using namespace std;

class Edge {
public:
	explicit Edge(int, int);
	const int& head();
	void addReverseEdge(const shared_ptr<Edge>&);
	int cf();
	void augment(int);
private:
	int h, c, f{0};
	weak_ptr<Edge> reverse_edge{};
};

Edge::Edge(int v, int capacity) : h{v}, c{capacity} {}

inline const int& Edge::head() { return h; }

inline void Edge::addReverseEdge(const shared_ptr<Edge>& re) { reverse_edge = re; }

inline int Edge::cf() { // residual capacity
	if (c) return c - f;
	else return reverse_edge.lock() -> f;
}

inline void Edge::augment(int fp) { 
	if (c) f += fp;
	else reverse_edge.lock() -> f -= fp;
}

class FlowNetwork {
public:
	void read();
	int maxFlow();
private:
	int S{}, T{};
	vector<vector<shared_ptr<Edge>>> G{}; // assume 0 is s, G.size() - 1 is target
	int augment();
};

void FlowNetwork::read() {
	int n, e;
	cin >> n >> e;
	G.assign(n, {});
	while (e--) {
		int u, v, c;
		cin >> u >> v >> c;
		G[u].push_back(make_shared<Edge>(v, c));
		G[v].push_back(make_shared<Edge>(u, 0)); // reverse edge for residual network
		G[u].back() -> addReverseEdge(G[v].back());
		G[v].back() -> addReverseEdge(G[u].back());
	}
	S = 0;
	T = n - 1;
}

int FlowNetwork::augment() { // BFS
	int flow{};

	vector<pair<int, shared_ptr<Edge>>> rec(G.size(), make_pair(-1, shared_ptr<Edge> {})); // rec[v] = {u, e} -> e = (u, v)
	rec[S].first = S;
	queue<pair<int, int>> q;
	q.emplace(S, numeric_limits<int>::max()); // source
	while (!q.empty() && rec[T].first == -1) {
		int u, f;
		tie(u, f) = q.front();
		q.pop();
		for (auto& e : G[u]) {
			auto v = e -> head();
			if (e -> cf() && rec[v].first == -1) {
				rec[v] = {u, e};
				q.emplace(v, min(f, e -> cf()));
				if (v == T) {
					flow = min(f, e -> cf());
					break;
				}
			}
		}
	}
	if (rec[T].first != -1)
		for (auto v = T; v != S; v = rec[v].first)
			rec[v].second -> augment(flow);
	return flow;
}

int FlowNetwork::maxFlow() {
	int flow{};
	while (auto f = augment()) // found a path
		flow += f;
	return flow;
}

int main() {
	FlowNetwork fn{};
	fn.read();
	cout << fn.maxFlow() << endl;
	return 0;
}
