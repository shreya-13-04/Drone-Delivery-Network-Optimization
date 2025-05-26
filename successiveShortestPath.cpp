#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "json.hpp"  // Include the nlohmann/json library

using json = nlohmann::json;
using namespace std;

// Structure for an edge in the graph
struct Edge {
    int from, to, capacity, cost, flow;
};

class MinCostMaxFlow {
private:
    unordered_map<string, int> nodeMap;  // Maps node names to integer indices
    unordered_map<int, vector<int>> adj;
    vector<Edge> edges;
    int numNodes = 0;

public:
    MinCostMaxFlow() {}

    void addEdge(int u, int v, int capacity, int cost) {
        edges.push_back({u, v, capacity, cost, 0});
        edges.push_back({v, u, 0, -cost, 0});
        adj[u].push_back(edges.size() - 2);
        adj[v].push_back(edges.size() - 1);
    }

    void loadGraphFromJSON(const string &filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error: Unable to open " << filename << endl;
            return;
        }

        json networkData;
        file >> networkData;
        file.close();

        int nodeCounter = 0;
        for (auto it = networkData.begin(); it != networkData.end(); ++it) {
            string nodeName = it.key();
            if (nodeMap.find(nodeName) == nodeMap.end()) {
                nodeMap[nodeName] = nodeCounter++;
            }
        }

        for (auto it = networkData.begin(); it != networkData.end(); ++it) {
            string from = it.key();
            int u = nodeMap[from];
            for (auto& edge : it.value()) {
                string dest = edge["destination"];
                int capacity = edge["capacity"];
                int cost = 1;

                if (nodeMap.find(dest) == nodeMap.end()) {
                    nodeMap[dest] = nodeCounter++;
                }
                int v = nodeMap[dest];
                addEdge(u, v, capacity, cost);
                cout << "Added edge: " << from << " -> " << dest
                     << " | Capacity: " << capacity << " | Cost: " << cost << endl;
            }
        }
        numNodes = nodeCounter;
        cout << "Total nodes after processing: " << numNodes << endl;
    }

    pair<int, int> minCostMaxFlow(int source, int sink) {
        int flow = 0, cost = 0;
        vector<int> potential(numNodes, 0);

        while (true) {
            const int INF = 1e9;
            vector<int> dist(numNodes, INF);
            vector<int> parent(numNodes, -1);
            vector<int> parentEdge(numNodes, -1);
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

            dist[source] = 0;
            pq.push(make_pair(0, source));

            while (!pq.empty()) {
                pair<int, int> top = pq.top();
                pq.pop();
                int d = top.first, u = top.second;
                if (d != dist[u]) continue;
                for (int idx : adj[u]) {
                    Edge &e = edges[idx];
                    if (e.capacity > e.flow) {
                        int nd = d + e.cost + potential[u] - potential[e.to];
                        if (nd < dist[e.to]) {
                            dist[e.to] = nd;
                            parent[e.to] = u;
                            parentEdge[e.to] = idx;
                            pq.push(make_pair(nd, e.to));
                        }
                    }
                }
            }

            if (dist[sink] == INF) break;

            for (int i = 0; i < numNodes; i++) {
                if (dist[i] < INF) potential[i] += dist[i];
            }

            int pushFlow = INF;
            for (int v = sink; v != source; v = parent[v]) {
                int idx = parentEdge[v];
                pushFlow = min(pushFlow, edges[idx].capacity - edges[idx].flow);
            }

            for (int v = sink; v != source; v = parent[v]) {
                int idx = parentEdge[v];
                edges[idx].flow += pushFlow;
                edges[idx ^ 1].flow -= pushFlow;
                cost += pushFlow * edges[idx].cost;
            }
            flow += pushFlow;
            cout << "Augmented Flow: " << pushFlow
                 << " | Total Flow: " << flow
                 << " | Total Cost: " << cost << endl;
        }
        return make_pair(flow, cost);
    }

    int getNodeIndex(const string &nodeName) {
        if (nodeMap.find(nodeName) == nodeMap.end()) {
            cerr << "Error: Node " << nodeName << " not found in graph!" << endl;
            return -1;
        }
        return nodeMap[nodeName];
    }
};

int main() {
    MinCostMaxFlow mcmf;
    mcmf.loadGraphFromJSON("network.json");

    int source = mcmf.getNodeIndex("Source");
    int sink = mcmf.getNodeIndex("Sink");

    if (source == -1 || sink == -1) {
        cerr << "Error: Source or Sink node not found!" << endl;
        return 1;
    }
    cout << "Source index: " << source << ", Sink index: " << sink << endl;

    pair<int, int> result = mcmf.minCostMaxFlow(source, sink);
    int maxFlow = result.first;
    int minCost = result.second;

    cout << "Final Maximum Flow: " << maxFlow << endl;
    cout << "Final Minimum Cost (Energy Usage): " << minCost << endl;

    return 0;
}