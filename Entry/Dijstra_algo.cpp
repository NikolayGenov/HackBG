/*
 Copyright (C) 2014 Nikolay Genov

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 **C++11 required**
 Task:
 https://github.com/HackBulgaria/System-C-1/blob/master/TasksForCourseApply/1-dijkstra-with-structures.md
*/

#include <vector>
#include <queue>
#include <limits>
#include <stdio.h>
#include <stdexcept>
using std::vector;

typedef int Vertex;
typedef double Weight;

const Weight maxWeight = std::numeric_limits<double>::infinity();

struct Neighbour
{
    Vertex target;
    Weight weight;

    Neighbour(Vertex v, Weight w) : target(v), weight(w) {}

    bool operator<(const Neighbour& other) const { return weight > other.weight; }
};

bool isValid(int N, Vertex v) { return 1 <= v && v <= N; }

typedef vector<Neighbour> Neighbours;
typedef vector<Neighbours> AdjList; // Adjacency list
typedef std::priority_queue<Neighbour> NeighbourQueue;

// Calcucates the distances between the source vertex and the rest of the graph given in Adjacency
// list.
// Returns if there is a path between the source and the destination vertices

bool DijkstraCalculateDistances(Vertex sourceVertex, Vertex destVertex, const AdjList& adjList, vector<Weight>& minDistances)
{
    minDistances.resize(adjList.size(), maxWeight);
    minDistances[sourceVertex] = 0;

    vector<bool> visited;
    visited.resize(adjList.size(), false);

    NeighbourQueue neighbourQueue;
    neighbourQueue.emplace(Neighbour(sourceVertex, minDistances[sourceVertex]));

    while (!neighbourQueue.empty()) {
        Vertex vertexFrom = neighbourQueue.top().target;
        Weight distance = neighbourQueue.top().weight;
        neighbourQueue.pop();

        if (visited[vertexFrom]) continue;
        visited[vertexFrom] = true;

        const Neighbours& neighbours = adjList[vertexFrom];

        for (auto neighbour : neighbours) {
            Vertex vertexTo = neighbour.target;
            Weight distanceTo = neighbour.weight + distance;

            if (visited[vertexTo]) continue;

            if (distanceTo < minDistances[vertexTo]) {
                minDistances[vertexTo] = distanceTo;
                neighbourQueue.emplace(Neighbour(vertexTo, minDistances[vertexTo]));
            }
        }
    }
    return visited[destVertex]; // Has been reached from source -> there is a path
}

void readInput(Vertex& vertexFrom, Vertex& vertexTo, AdjList& adjList)
{
    int N, M; // Number of vertices and  number of edges - I prefer N and M for those.
    scanf("%d %d", &N, &M);

    adjList.resize(N);

    Weight weight;

    // Using at instead of operator [] because of the range check and the out_of_range exception
    for (int i = 0; i < M; ++i) {
        scanf("%d %d %lf", &vertexFrom, &vertexTo, &weight);
        adjList.at(vertexFrom - 1).emplace_back(Neighbour(vertexTo - 1, weight));
        adjList.at(vertexTo - 1).emplace_back(Neighbour(vertexFrom - 1, weight));
    }

    scanf("%d %d", &vertexFrom, &vertexTo);

    if (!isValid(N, vertexFrom) || !isValid(N, vertexTo))
        throw std::out_of_range("Vertex is out of range");
}

void printResult(bool hasPathBetweenFromAndTo, Vertex vertexTo, const vector<Weight>& minDistances)
{
    if (hasPathBetweenFromAndTo)
        printf("%lf\n", minDistances[vertexTo]);
    else
        printf("-1\n");
}

int main()
{
    Vertex vertexFrom = 0, vertexTo = 0; // vertex v -> where 1 <= v <= N
    AdjList adjList(0);
    vector<Weight> minDistances;

    try { readInput(vertexFrom, vertexTo, adjList); }
    catch (std::out_of_range e)
    {
        printf(
        "Please enter valid input as described in the task\n(Note: vertex v: --> 1 <= v <= N)");
        std::abort();
    }

    bool hasPathBetweenFromAndTo =
    DijkstraCalculateDistances(vertexFrom - 1, vertexTo - 1, adjList, minDistances);

    printResult(hasPathBetweenFromAndTo, vertexTo - 1, minDistances);

    return 0;
}

/* Test input:
5 8
1 4 10.5
1 3 30.52
3 1 5.5
1 2 15.4
2 3 100
2 5 5
5 3 14.8
4 5 10
1 5

Result: 20.30
*/
