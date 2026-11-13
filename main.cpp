// Between steps 2 and 3: name campus stops and display walkway times.
#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int SIZE = 7;

struct Edge {
    int src, dest, weight;
};

typedef pair<int, int> Pair;

class Graph {
    vector<bool> active;

public:
    // Each pair contains a neighboring vertex and its edge weight.
    vector<vector<Pair>> adjList;

    Graph(const vector<Edge>& edges) {
        adjList.resize(SIZE);
        active.assign(SIZE, true);
        for (const Edge& edge : edges)
            addEdge(edge.src, edge.dest, edge.weight);
    }

    bool isVertex(int vertex) const {
        return vertex >= 0 && vertex < static_cast<int>(adjList.size())
               && active[vertex];
    }

    // IDs stay stable: removing a vertex never renumbers the other vertices.
    void removeVertex(int vertex) {
        if (!isVertex(vertex))
            throw out_of_range("Invalid vertex to remove.");
        adjList[vertex].clear();
        active[vertex] = false;
        for (vector<Pair>& neighbors : adjList) {
            neighbors.erase(remove_if(neighbors.begin(), neighbors.end(),
                [vertex](const Pair& edge) { return edge.first == vertex; }),
                neighbors.end());
        }
    }

    int addVertex() {
        adjList.emplace_back();
        active.push_back(true);
        return static_cast<int>(adjList.size()) - 1;
    }

    void addEdge(int src, int dest, int weight) {
        if (!isVertex(src) || !isVertex(dest))
            throw out_of_range("Invalid edge endpoint.");
        if (weight <= 0)
            throw invalid_argument("Weights must be positive.");
        adjList[src].push_back(make_pair(dest, weight));
        adjList[dest].push_back(make_pair(src, weight));
    }

    void setWeight(int src, int dest, int weight) {
        if (!isVertex(src) || !isVertex(dest))
            throw out_of_range("Invalid edge endpoint.");
        if (weight <= 0)
            throw invalid_argument("Weights must be positive.");
        bool found = false;
        for (Pair& neighbor : adjList[src]) {
            if (neighbor.first == dest) {
                neighbor.second = weight;
                found = true;
            }
        }
        if (!found)
            throw invalid_argument("Edge does not exist.");
        for (Pair& neighbor : adjList[dest]) {
            if (neighbor.first == src)
                neighbor.second = weight;
        }
    }

    // Mark on removal from the stack so the last neighbor is explored first.
    vector<int> dfs(int start) const {
        if (!isVertex(start))
            throw out_of_range("Invalid starting vertex.");
        vector<bool> visited(adjList.size(), false);
        vector<int> order;
        stack<int> pending;
        pending.push(start);

        while (!pending.empty()) {
            int current = pending.top();
            pending.pop();
            if (visited[current])
                continue;
            visited[current] = true;
            order.push_back(current);
            for (const Pair& neighbor : adjList[current]) {
                if (!visited[neighbor.first])
                    pending.push(neighbor.first);
            }
        }
        return order;
    }

    // Mark on insertion so each vertex enters the queue only once.
    vector<int> bfs(int start) const {
        if (!isVertex(start))
            throw out_of_range("Invalid starting vertex.");
        vector<bool> visited(adjList.size(), false);
        vector<int> order;
        queue<int> pending;
        visited[start] = true;
        pending.push(start);

        while (!pending.empty()) {
            int current = pending.front();
            pending.pop();
            order.push_back(current);
            for (const Pair& neighbor : adjList[current]) {
                if (!visited[neighbor.first]) {
                    visited[neighbor.first] = true;
                    pending.push(neighbor.first);
                }
            }
        }
        return order;
    }

    void printGraph() const {
        cout << "Graph's adjacency list:\n";
        for (int vertex = 0; vertex < static_cast<int>(adjList.size()); ++vertex) {
            if (!isVertex(vertex))
                continue;
            cout << vertex << " --> ";
            for (const Pair& neighbor : adjList[vertex])
                cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            cout << '\n';
        }
    }
};

// Vertices 1 and 6 remain retired; their IDs are never reused.
const vector<string> CAMPUS_NAMES = {
    "Main Gate", "Retired Parking", "Library", "Science Hall",
    "Student Center", "Gym", "Retired Annex", "Cafeteria", "Bookstore",
    "Arts Center", "Engineering Hall", "Residence Hall", "Transit Stop"
};

string stopLabel(int vertex) {
    return to_string(vertex) + " (" + CAMPUS_NAMES.at(vertex) + ")";
}

void displayCampus(const Graph& graph) {
    cout << "Campus Walking Network (walkway times in minutes):\n";
    for (int vertex = 0; vertex < static_cast<int>(graph.adjList.size()); ++vertex) {
        if (!graph.isVertex(vertex))
            continue;
        cout << stopLabel(vertex) << " connects to:\n";
        for (const Pair& neighbor : graph.adjList[vertex])
            cout << "  -> " << stopLabel(neighbor.first) << ": "
                 << neighbor.second << " min\n";
    }
}

void printTraversal(const vector<int>& order) {
    for (int vertex : order)
        cout << vertex << ' ';
    cout << '\n';
}

Graph createGraph() {
    vector<Edge> edges = {
        {0,1,12},{0,2,8},{0,3,21},{2,3,6},{2,6,2},
        {5,6,6},{4,5,9},{2,4,4},{2,5,5}
    };
    Graph graph(edges);
    // Delete two original vertices and all edges incident to them.
    graph.removeVertex(1);
    graph.removeVertex(6);

    // Change every surviving original edge weight.
    graph.setWeight(0, 2, 6);
    graph.setWeight(0, 3, 15);
    graph.setWeight(2, 3, 5);
    graph.setWeight(4, 5, 7);
    graph.setWeight(2, 4, 4);
    graph.setWeight(2, 5, 9);

    // Add six new vertices, with IDs 7 through 12 (11 active vertices total).
    for (int count = 0; count < 6; ++count)
        graph.addVertex();
    vector<Edge> newEdges = {
        {0,7,3},{7,8,4},{2,8,6},{8,9,2},{4,9,5},{9,10,3},
        {5,10,4},{10,11,2},{11,12,5},{3,12,8},{7,12,11}
    };
    for (const Edge& edge : newEdges)
        graph.addEdge(edge.src, edge.dest, edge.weight);
    return graph;
}

int main() {
    Graph graph = createGraph();
    displayCampus(graph);
    cout << "DFS starting from vertex 0:\n";
    printTraversal(graph.dfs(0));
    cout << "BFS starting from vertex 0:\n";
    printTraversal(graph.bfs(0));
    return 0;
}
