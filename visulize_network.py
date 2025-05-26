import json
import networkx as nx
import matplotlib.pyplot as plt

# Load network data from JSON
with open("network.json", "r") as file:
    data = json.load(file)

# Create a directed graph
G = nx.DiGraph()

# Add edges to the graph
for node, edges in data.items():
    for edge in edges:
        G.add_edge(node, edge["destination"], weight=edge["capacity"])

# Positioning nodes using spring layout
pos = nx.spring_layout(G, seed=42)

# Draw the network
plt.figure(figsize=(10, 6))
nx.draw(G, pos, with_labels=True, node_size=3000, node_color="skyblue", edge_color="black", font_size=10, font_weight="bold")

# Draw edge labels (capacities)
edge_labels = {(u, v): d["weight"] for u, v, d in G.edges(data=True)}
nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=10)

# Show the graph
plt.title("Drone Delivery Network")
plt.show()
