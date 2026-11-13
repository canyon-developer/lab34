// Step 5: display the campus minimum spanning tree and its total weight.
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
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
const long long INF = numeric_limits<long long>::max();

struct ShortestPaths {
    vector<long long> distance;
    vector<int> parent;
};

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

    // Dijkstra: positive edge weights represent walking times in minutes.
    ShortestPaths shortestPaths(int start) const {
        if (!isVertex(start))
            throw out_of_range("Invalid starting vertex.");
        ShortestPaths result;
        result.distance.assign(adjList.size(), INF);
        result.parent.assign(adjList.size(), -1);
        typedef pair<long long, int> DistanceVertex;
        priority_queue<DistanceVertex, vector<DistanceVertex>,
                       greater<DistanceVertex>> pending;
        result.distance[start] = 0;
        pending.push(make_pair(0, start));

        while (!pending.empty()) {
            const DistanceVertex next = pending.top();
            pending.pop();
            int current = next.second;
            if (next.first != result.distance[current])
                continue; // Ignore entries superseded by a shorter route.
            for (const Pair& neighbor : adjList[current]) {
                long long candidate = next.first + neighbor.second;
                if (candidate < result.distance[neighbor.first]) {
                    result.distance[neighbor.first] = candidate;
                    result.parent[neighbor.first] = current;
                    pending.push(make_pair(candidate, neighbor.first));
                }
            }
        }
        return result;
    }

    // Prim: choose the cheapest edge joining a new vertex to the tree.
    // A disconnected graph produces a minimum spanning forest instead.
    vector<Edge> minimumSpanningTree() const {
        vector<bool> visited(adjList.size(), false);
        vector<Edge> tree;
        // Queue entries are (weight, (source, destination)).
        typedef pair<int, Pair> Candidate;
        priority_queue<Candidate, vector<Candidate>, greater<Candidate>> pending;
        for (int root = 0; root < static_cast<int>(adjList.size()); ++root) {
            if (!isVertex(root) || visited[root])
                continue;
            pending.push(make_pair(0, make_pair(-1, root)));
            while (!pending.empty()) {
                const Candidate next = pending.top();
                pending.pop();
                int src = next.second.first;
                int dest = next.second.second;
                if (visited[dest])
                    continue;
                visited[dest] = true;
                if (src != -1)
                    tree.push_back({src, dest, next.first});
                for (const Pair& neighbor : adjList[dest]) {
                    if (!visited[neighbor.first])
                        pending.push(make_pair(neighbor.second,
                                               make_pair(dest, neighbor.first)));
                }
            }
        }
        return tree;
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
        cout << "  " << stopLabel(vertex) << '\n';
}

void exploreCampus(const Graph& graph, int start) {
    cout << "\nCampus exploration order (DFS) from " << stopLabel(start) << ":\n"
         << "Explore one branch before returning to other branches.\n"
         << "This is a visit order; consecutive stops may not share a walkway.\n";
    printTraversal(graph.dfs(start));
}

void nearbyStops(const Graph& graph, int start) {
    cout << "\nStops by number of walkways (BFS) from " << stopLabel(start) << ":\n"
         << "Visit stops in increasing number of walkway connections.\n"
         << "BFS ignores walking times; use shortest paths for fastest routes.\n";
    printTraversal(graph.bfs(start));
}

void printShortestPaths(const Graph& graph, int start) {
    const ShortestPaths result = graph.shortestPaths(start);
    cout << "\nShortest walking times from " << stopLabel(start) << ":\n";
    for (int vertex = 0; vertex < static_cast<int>(graph.adjList.size()); ++vertex) {
        if (!graph.isVertex(vertex))
            continue;
        cout << "  " << stopLabel(vertex) << ": ";
        if (result.distance[vertex] == INF)
            cout << "unreachable\n";
        else {
            cout << result.distance[vertex] << " min; route: ";
            vector<int> path;
            for (int current = vertex; current != -1; current = result.parent[current])
                path.push_back(current);
            reverse(path.begin(), path.end());
            for (size_t index = 0; index < path.size(); ++index) {
                if (index != 0)
                    cout << " -> ";
                cout << stopLabel(path[index]);
            }
            cout << '\n';
        }
    }
}

void printMinimumSpanningTree(const Graph& graph) {
    const vector<Edge> tree = graph.minimumSpanningTree();
    int vertexCount = 0;
    for (int vertex = 0; vertex < static_cast<int>(graph.adjList.size()); ++vertex) {
        if (graph.isVertex(vertex))
            ++vertexCount;
    }
    const int components = vertexCount - static_cast<int>(tree.size());
    cout << "\nCampus walkway maintenance plan:\n"
         << "Connect all stops with minimum total walkway time.\n";
    if (components > 1)
        cout << "Network disconnected: minimum spanning forest with "
             << components << " components.\n";
    else if (vertexCount == 0)
        cout << "No active campus stops.\n";
    else
        cout << "Minimum spanning tree edges:\n";
    long long total = 0;
    for (const Edge& edge : tree) {
        cout << "  " << stopLabel(edge.src) << " <-> " << stopLabel(edge.dest)
             << ": " << edge.weight << " min\n";
        total += edge.weight;
    }
    cout << "Selected walkways: " << tree.size()
         << "\nTotal walkway time: " << total << " min\n";
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
    exploreCampus(graph, 0);
    nearbyStops(graph, 0);
    printShortestPaths(graph, 0);
    printMinimumSpanningTree(graph);
    return 0;
}
