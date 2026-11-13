// Between the original demo and step 1: add DFS and exercise it in the driver.
#include <iostream>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace std;

const int SIZE = 7;

struct Edge {
    int src, dest, weight;
};

typedef pair<int, int> Pair;

class Graph {
public:
    // Each pair contains a neighboring vertex and its edge weight.
    vector<vector<Pair>> adjList;

    Graph(const vector<Edge>& edges) {
        adjList.resize(SIZE);
        for (const Edge& edge : edges) {
            adjList[edge.src].push_back(make_pair(edge.dest, edge.weight));
            adjList[edge.dest].push_back(make_pair(edge.src, edge.weight));
        }
    }

    bool isVertex(int vertex) const {
        return vertex >= 0 && vertex < static_cast<int>(adjList.size());
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

    void printGraph() const {
        cout << "Graph's adjacency list:\n";
        for (int vertex = 0; vertex < static_cast<int>(adjList.size()); ++vertex) {
            cout << vertex << " --> ";
            for (const Pair& neighbor : adjList[vertex])
                cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            cout << '\n';
        }
    }
};

void printTraversal(const vector<int>& order) {
    for (int vertex : order)
        cout << vertex << ' ';
    cout << '\n';
}

int main() {
    vector<Edge> edges = {
        {0,1,12},{0,2,8},{0,3,21},{2,3,6},{2,6,2},
        {5,6,6},{4,5,9},{2,4,4},{2,5,5}
    };
    Graph graph(edges);
    graph.printGraph();
    cout << "DFS starting from vertex 0:\n";
    printTraversal(graph.dfs(0));
    return 0;
}
