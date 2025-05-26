# Efficient Drone Delivery Network Optimization

## Project Overview

This project focuses on optimizing drone delivery networks using advanced flow algorithms. The main goal is to efficiently maximize delivery capacity and prioritize high-weight packages while minimizing delivery costs.

We implement and analyze the following algorithms to solve the maximum flow and cost optimization problems in drone delivery networks:

- **Ford-Fulkerson Algorithm** for max flow optimization  
- **Edmonds-Karp Algorithm** (BFS-based) for flow search  
- **Capacity Scaling Algorithm** to prioritize packages by weight  
- **Successive Shortest Path Algorithm** for cost-based delivery optimization

## Technologies and Tools

- Python 3.x  
- NetworkX (for graph representation)  
- Matplotlib (for visualization)  

## How to Run

1. **Clone the repository:**

```bash
git clone https://github.com/shreya-13-04/Drone-Delivery-Network-Optimization.git
cd Drone-Delivery-Network-Optimization


2.(Optional) Create and activate a virtual environment:

python -m venv venv
source venv/bin/activate    # Linux/Mac
venv\Scripts\activate       # Windows

3.Install dependencies:

pip install -r requirements.txt

4.Run the algorithm scripts or notebooks:

python algorithms/ford_fulkerson.py
python algorithms/edmonds_karp.py

Contributions
This project was developed as a team effort for the Data Structures and Algorithms course. Contributions include algorithm implementation, analysis, and report writing.

License
This project is open-source and available under the MIT License.

Contact
For queries or contributions, feel free to reach out or open an issue on the GitHub repository.

© 2025 Shreya and Team