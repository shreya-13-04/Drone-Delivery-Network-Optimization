#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <chrono> // For execution time measurement
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

struct Edge {
    string to;
    int capacity;
    int flow;
};

// Graph representation
unordered_map<string, vector<Edge>> graph;
unordered_map<string, unordered_map<string, int>> capacity;
unordered_map<string, unordered_map<string, int>> flow;

// Load graph from JSON file
void loadGraph(const string &filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    json data;
    file >> data;

    graph.clear();
    capacity.clear();
    flow.clear();

    for (const auto &node : data.items()) {
        string from = node.key();
        for (const auto &edge : node.value()) {
            string to = edge["destination"].get<string>();
            int cap = edge["capacity"].get<int>();

            graph[from].push_back({to, cap, 0});
            graph[to].push_back({from, 0, 0}); // Reverse edge
            capacity[from][to] = cap;
            capacity[to][from] = 0;
            flow[from][to] = 0;
            flow[to][from] = 0;
        }
    }

    cout << "\nGraph Loaded Successfully!\n";
    cout << "--------------------------\n";
}

// BFS for finding an augmenting path
bool bfs(const string &source, const string &sink, unordered_map<string, string> &parent) {
    queue<string> q;
    q.push(source);
    parent.clear();
    parent[source] = "";

    while (!q.empty()) {
        string node = q.front();
        q.pop();

        for (auto &edge : graph[node]) {
            int residual = capacity[node][edge.to] - flow[node][edge.to];
            if (parent.find(edge.to) == parent.end() && residual > 0) {
                parent[edge.to] = node;
                if (edge.to == sink) return true;
                q.push(edge.to);
            }
        }
    }
    return false;
}

// Augment flow along the path
int augmentFlow(const string &source, const string &sink, unordered_map<string, string> &parent) {
    int pathFlow = numeric_limits<int>::max();
    string node = sink;

    while (node != source) {
        string prev = parent[node];
        pathFlow = min(pathFlow, capacity[prev][node] - flow[prev][node]);
        node = prev;
    }

    node = sink;
    while (node != source) {
        string prev = parent[node];
        flow[prev][node] += pathFlow;
        flow[node][prev] -= pathFlow;
        node = prev;
    }

    return pathFlow;
}

// Edmonds-Karp Algorithm (Ford-Fulkerson with BFS)
int edmondsKarp(const string &source, const string &sink) {
    int maxFlow = 0;
    unordered_map<string, string> parent;

    auto start = steady_clock::now();
    while (bfs(source, sink, parent)) {
        int addedFlow = augmentFlow(source, sink, parent);
        maxFlow += addedFlow;
        cout << "Augmenting Path Found! Flow added: " << addedFlow << endl;
    }

    auto end = steady_clock::now();
    double elapsed = duration_cast<microseconds>(end - start).count() / 1e6;

    cout << "Edmonds-Karp Execution Time: " << elapsed << " seconds\n";
    return maxFlow;
}

// Capacity Scaling Max Flow Algorithm
int capacityScalingMaxFlow(const string &source, const string &sink) {
    int maxFlow = 0;
    int maxCapacity = 0;

    for (auto &node : capacity) {
        for (auto &edge : node.second) {
            maxCapacity = max(maxCapacity, edge.second);
        }
    }

    if (maxCapacity == 0) {
        cout << "No available capacity in the network." << endl;
        return 0;
    }

    cout << "Maximum capacity in the graph: " << maxCapacity << endl;

    int delta = 1;
    while (delta * 2 <= maxCapacity) delta *= 2;

    auto start = steady_clock::now();

    while (delta > 0) {
        unordered_map<string, string> parent;
        while (bfs(source, sink, parent)) {
            int addedFlow = augmentFlow(source, sink, parent);
            maxFlow += addedFlow;
            cout << "Flow added (delta = " << delta << "): " << addedFlow << endl;
        }
        delta /= 2;
    }

    auto end = steady_clock::now();
    double elapsed = duration_cast<microseconds>(end - start).count() / 1e6;

    cout << "Capacity Scaling Execution Time: " << elapsed << " seconds\n";
    return maxFlow;
}

int main() {
    string filename = "network.json";
    string source = "Source";
    string sink = "Sink";

    // Run Capacity Scaling First
    loadGraph(filename);
    cout << "Running Capacity Scaling Max Flow Algorithm...\n";
    int maxFlowScaling = capacityScalingMaxFlow(source, sink);
    cout << "Capacity Scaling Max Flow: " << maxFlowScaling << endl;
    cout << "------------------------------------------\n";

    // Run Edmonds-Karp (Ensure fresh graph)
    loadGraph(filename);
    cout << "Running Edmonds-Karp Max Flow Algorithm...\n";
    int maxFlowEK = edmondsKarp(source, sink);
    cout << "Edmonds-Karp Max Flow: " << maxFlowEK << endl;
}