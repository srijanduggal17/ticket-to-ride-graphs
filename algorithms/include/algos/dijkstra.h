#pragma once

#include <queue>

#include "board_defs.h"
#include "adjacency_list2.h"

// using TTRAdjacencyList2 = std::unordered_map<const CityNode *, std::vector<const Edge *> >;

struct PQElement {
	size_t mIndex;
	int mDist;
	const Edge * mSource;

	PQElement(size_t index, int dist, const Edge * source) :
		mIndex(index),
		mDist(dist),
		mSource(source) {
	}
};

struct CompareNode {
	bool operator()(const PQElement& a, const PQElement& b) const {
		return a.mDist > b.mDist; // min-heap (smaller distance has higher priority)
	}
};

inline EdgePath_T Dijkstra(const TTRAdjacencyList2& aGraphAdjList, const CityNode* aStartCity, const CityNode* aEndCity) {
	// Data structure initialization
	size_t startIndex = 0UL;
	std::vector<const CityNode*> cityIndices;
	std::unordered_map<const CityNode *, int> cityNodeIndex;
	cityIndices.reserve(aGraphAdjList.size());
	for (const auto& pair : aGraphAdjList) {
		if (pair.first == aStartCity) {
			startIndex = cityIndices.size();
		}
		cityNodeIndex.emplace(pair.first, cityIndices.size());
		cityIndices.push_back(pair.first);
	}

	std::vector<bool> visitedIndices;
	std::vector<int> distances;
	std::vector<const Edge*> arrivalMethod;
	visitedIndices.resize(cityIndices.size(), false);
	distances.resize(cityIndices.size(), std::numeric_limits<int>::max());
	arrivalMethod.resize(cityIndices.size(), nullptr);

	std::priority_queue<PQElement, std::vector<PQElement>, CompareNode> myQueue;

	// Setup
	visitedIndices[startIndex] = true;
	distances[startIndex] = 0;
	myQueue.emplace(startIndex, 0, nullptr);

	int numIters = 0;
	while (not myQueue.empty()) {
		const PQElement cur = myQueue.top();
		myQueue.pop();
		visitedIndices[cur.mIndex] = true;

		spdlog::info("Examining: {}/{}", cur.mDist, *cityIndices[cur.mIndex]);
		// Check if we are done
		if (cityIndices[cur.mIndex] == aEndCity) {
			distances[cur.mIndex] = cur.mDist;
			arrivalMethod[cur.mIndex] = cur.mSource;
			break;
		}

		// Go over neighbors
		const std::vector<const Edge*> & AdjEdges = aGraphAdjList.at(cityIndices[cur.mIndex]);
		for (const Edge * curEdge : AdjEdges) {
			// Shorten distance to neighbor if needed
			int DistToNeighbor = distances[cur.mIndex] + curEdge->mCost;
			size_t NeighborNdx = cityNodeIndex[curEdge->mTo];
			if (visitedIndices[NeighborNdx]) {
				continue;
			}

			if (DistToNeighbor < distances[NeighborNdx]) {
				distances[NeighborNdx] = DistToNeighbor;
				arrivalMethod[NeighborNdx] = curEdge;
				myQueue.emplace(NeighborNdx, DistToNeighbor, curEdge);
			}
		}
		numIters++;
	}

	spdlog::info("Reached goal in {} iters", numIters);

	// Reconstruct path
	EdgePath_T shortestPath;

	const CityNode* currentNode = aEndCity;

	bool sourceReached = false;
	int numItersBack = 0;
	while (not sourceReached) {
		const Edge* incomingEdge = arrivalMethod[cityNodeIndex[currentNode]];

		if (incomingEdge == nullptr) {
			spdlog::error("Reached end of graph while recording path");
		}

		shortestPath.emplace_back(incomingEdge);

		if (incomingEdge->mFrom == aStartCity) {
			sourceReached = true;
		}
		currentNode = incomingEdge->mFrom;
		numItersBack++;
	}

	if (sourceReached) {
		spdlog::info("Backtracked path in {} iterations", numItersBack);
		std::reverse(shortestPath.begin(), shortestPath.end());
	}
	else {
		spdlog::error("Did not reach source while backtracking");
	}

	return shortestPath;
}

/*

   Make a data structure to store distances to each node from the start node

   Single-source shortest path -> will give the shortest distance from source node
   to all nodes

    Add start node to queue
    Mark as visited

    Check distance to all neighbors and update distance if it is shorter
    Add neighbors to queue if not visited

    If goal node is hit, we are done.


    Reconstruct the path to the goal node:
    - As we are building, store an array that has reference to the previous id
    - Walk backwards until the start
 */