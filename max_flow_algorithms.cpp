#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <chrono>
#include <limits>
#include <iomanip>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class MaxFlowAlgorithms {
private:
    unordered_map<string, unordered_map<string, int>> capacity, residualCapacity;
    unordered_map<string, vector<string>> adjList;
    string network_file;

public:
    MaxFlowAlgorithms(string filename = "network.json") : network_file(filename) {
        loadNetwork();
    }

    void loadNetwork() {
        ifstream file(network_file);
        if (!file) {
            cerr << "Error: " << network_file << " not found!" << endl;
            return;
        }

        json networkData;
        file >> networkData;
        file.close();

        capacity.clear();
        residualCapacity.clear();
        adjList.clear();

        for (auto& node : networkData.items()) {
            string from = node.key();
            for (auto& edge : node.value()) {
                string to = edge["destination"];
                int cap = edge["capacity"];
                capacity[from][to] = cap;
                residualCapacity[from][to] = cap;
                residualCapacity[to][from] = 0; // Initialize reverse edge with 0
                adjList[from].push_back(to);
                adjList[to].push_back(from);
            }
        }

        cout << "Network loaded from " << network_file << endl;
    }

    int bfs(unordered_map<string, string>& parent, string source, string sink) {
        parent.clear();
        queue<pair<string, int>> q;
        q.push({source, numeric_limits<int>::max()});

        while (!q.empty()) {
            string node = q.front().first;
            int flow = q.front().second;
            q.pop();

            for (auto& next : adjList[node]) {
                if (parent.find(next) == parent.end() && residualCapacity[node][next] > 0) {
                    parent[next] = node;
                    int new_flow = min(flow, residualCapacity[node][next]);

                    //cout << "Exploring edge " << node << " to " << next << " with flow " << new_flow << endl;

                    if (next == sink) return new_flow;
                    q.push({next, new_flow});
                }
            }
        }
        return 0;
    }

    int edmondsKarp(string source, string sink) {
        unordered_map<string, string> parent;
        int maxFlow = 0;
        residualCapacity = capacity; // Reset residual graph

        auto start = chrono::high_resolution_clock::now();
        int flow;
        while ((flow = bfs(parent, source, sink))) {
            maxFlow += flow;
            string cur = sink;

            while (cur != source) {
                string prev = parent[cur];
                residualCapacity[prev][cur] -= flow;
                residualCapacity[cur][prev] += flow; // Reverse flow
                cur = prev;
            }
        }
        auto end = chrono::high_resolution_clock::now();
        double timeTaken = chrono::duration<double>(end - start).count();

        cout << "Max Flow (Edmonds-Karp): " << maxFlow << endl;
        cout << "Flow Distribution (Edmonds-Karp):" << endl;
        for (const auto& from : capacity) {
            for (const auto& edge : from.second) {
                int usedFlow = capacity[from.first][edge.first] - residualCapacity[from.first][edge.first];
                if (usedFlow > 0) {
                    cout << "  " << from.first << " to " << edge.first << ": " << usedFlow << endl;
                }
            }
        }
        return maxFlow;
    }

    int dfs(string node, string sink, int flow, unordered_map<string, bool>& visited) {
        if (node == sink) return flow;
        visited[node] = true;

        for (auto& next : adjList[node]) {
            if (!visited[next] && residualCapacity[node][next] > 0) {
                int new_flow = min(flow, residualCapacity[node][next]);
                int result = dfs(next, sink, new_flow, visited);
                if (result > 0) {
                    residualCapacity[node][next] -= result;
                    residualCapacity[next][node] += result;
                    return result;
                }
            }
        }
        return 0;
    }

    int fordFulkerson(string source, string sink) {
        int maxFlow = 0;
        unordered_map<string, bool> visited;
        residualCapacity = capacity;

        auto start = chrono::high_resolution_clock::now();
        while (int flow = dfs(source, sink, numeric_limits<int>::max(), visited)) {
            maxFlow += flow;
            visited.clear();
        }
        auto end = chrono::high_resolution_clock::now();
        double timeTaken = chrono::duration<double>(end - start).count();

        cout << "Max Flow (Ford-Fulkerson): " << maxFlow << endl;
        cout << "Flow Distribution (Ford-Fulkerson):" << endl;
        for (const auto& from : capacity) {
            for (const auto& edge : from.second) {
                int usedFlow = capacity[from.first][edge.first] - residualCapacity[from.first][edge.first];
                if (usedFlow > 0) {
                    cout << "  " << from.first << " to " << edge.first << ": " << usedFlow << endl;
                }
            }
        }
        return maxFlow;
    }

    void compareAlgorithms() {
        string source = "Source", sink = "Sink";

        cout << "\nRunning Ford-Fulkerson Algorithm (DFS-based)..." << endl;
        auto startFF = chrono::high_resolution_clock::now();
        int maxFlowFF = fordFulkerson(source, sink);
        auto endFF = chrono::high_resolution_clock::now();
        double timeTakenFF = chrono::duration<double>(endFF - startFF).count();

        cout << "\nRunning Edmonds-Karp Algorithm (BFS-based)..." << endl;
        auto startEK = chrono::high_resolution_clock::now();
        int maxFlowEK = edmondsKarp(source, sink);
        auto endEK = chrono::high_resolution_clock::now();
        double timeTakenEK = chrono::duration<double>(endEK - startEK).count();

        cout << "\n===== Algorithm Performance Comparison =====" << endl;
        cout << "Ford-Fulkerson (DFS) Execution Time: " << fixed << setprecision(8) << timeTakenFF << " seconds" << endl;
        cout << "Edmonds-Karp (BFS) Execution Time: " << fixed << setprecision(8) << timeTakenEK << " seconds" << endl;

        if (timeTakenEK < timeTakenFF)
            cout << "Edmonds-Karp is faster on this network." << endl;
        else if (timeTakenFF < timeTakenEK)
            cout << "Ford-Fulkerson is faster on this network." << endl;
        else
            cout << "Both algorithms took the same time." << endl;
    }
};

int main() {
    MaxFlowAlgorithms maxFlowSolver("network.json");
    maxFlowSolver.compareAlgorithms();
    return 0;
}
