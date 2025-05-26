#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdlib> // For random numbers
#include <ctime>   // For seeding randomness
#include "json.hpp"  
using namespace std;
using json = nlohmann::json;

class DroneDeliveryNetwork {
private:
    unordered_map<string, vector<pair<string, int>>> adjList; // Adjacency list

public:
    DroneDeliveryNetwork() {
        srand(time(0)); // Seed for random numbers
    }

    void addEdge(const string& u, const string& v, int capacity) {
        adjList[u].push_back({v, capacity});
    }

    void generateTestNetwork(int numWarehouses = 2, int numDrones = 2, int numDestinations = 2) {
        adjList.clear();
        string source = "Source", sink = "Sink";

        // Add warehouses as sources
        for (int i = 0; i < numWarehouses; i++) {
            string warehouse = "Warehouse_" + to_string(i);
            addEdge(source, warehouse, rand() % 11 + 5); // Capacity 5-15
        }

        // Add drones as intermediate nodes
        for (int j = 0; j < numDrones; j++) {
            string drone = "Drone_" + to_string(j);
            for (int i = 0; i < numWarehouses; i++) {
                string warehouse = "Warehouse_" + to_string(i);
                addEdge(warehouse, drone, rand() % 9 + 2); // Capacity 2-10
            }
            for (int k = 0; k < numDestinations; k++) {
                string destination = "Destination_" + to_string(k);
                addEdge(drone, destination, rand() % 5 + 1); // Capacity 1-5
            }
        }

        // Add destinations as sinks
        for (int k = 0; k < numDestinations; k++) {
            string destination = "Destination_" + to_string(k);
            addEdge(destination, sink, rand() % 6 + 5); // Capacity 5-10
        }
    }

    void saveToFile(const string& filename = "data/network.json") {
        json networkJson;

        for (auto& node : adjList) {
            json edges;
            for (auto& edge : node.second) {
                edges.push_back({{"destination", edge.first}, {"capacity", edge.second}});
            }
            networkJson[node.first] = edges;
        }

        ofstream file(filename);
        if (!file) {
            cerr << "Error: Unable to save network!" << endl;
            return;
        }

        file << networkJson.dump(4); // Pretty print with an indent of 4 spaces
        file.close();
        cout << "Network saved to " << filename << endl;
    }

    void printNetwork() {
        cout << "Drone Delivery Flow Network:\n";
        for (auto& node : adjList) {
            cout << node.first << " -> ";
            for (auto& edge : node.second) {
                cout << "(" << edge.first << ", capacity=" << edge.second << ") ";
            }
            cout << endl;
        }
    }
};

int main() {
    DroneDeliveryNetwork network;
    network.generateTestNetwork();
    network.printNetwork();
    network.saveToFile("network.json"); // Save to JSON file
    return 0;
}
