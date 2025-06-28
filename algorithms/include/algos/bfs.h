#pragma once

#include <queue>
#include <algorithm>
#include <unordered_map>

#include "board_defs.h"
#include "adjacency_list.h"

/**
 * @brief Use Breadth-First Search to get shortest path between two cities
 *
 * @param aGraphAdjList - graph representing game board
 * @param aStartCity - start city
 * @param aEndCity - end city
 * @return Path_T
 */
inline Path_T BFS(const TTRAdjacencyList& aGraphAdjList, City_T aStartCity, City_T aEndCity) {
	Path_T shortestPathByLegs;

	std::unordered_map<City_T, Leg_T> visitedNodes;

	std::queue<std::pair<City_T, Leg_T> > nodesToVisit;
	nodesToVisit.push({aStartCity,
			   {
				   .mNeighbor = aStartCity,
				   .mCity1 = aStartCity,
				   .mCity2 = aStartCity,
				   .mCost = 0,
				   .mColor = CardColor_T::red,
				   .mEdgeID = UUID::generate()
			   }});

	bool destReached = false;
	int numIters = 0;
	int MaxIters = 36+100*2;
	while (numIters < MaxIters) {
		const auto &[CurrentNode, EdgeToGetHere] = nodesToVisit.front();
		nodesToVisit.pop();

		if (visitedNodes.contains(CurrentNode)) {
			continue;
		}

		const std::vector<Leg_T> & AdjacentNodes = aGraphAdjList.at(CurrentNode);
		visitedNodes.emplace(CurrentNode, EdgeToGetHere);

		if (CurrentNode == aEndCity) {
			destReached = true;
			spdlog::info("Found shortest path in {} iterations", numIters);
			break;
		}

		for (const Leg_T& Edge : AdjacentNodes) {
			if (Edge.mNeighbor == CurrentNode) {
				spdlog::error("Edge neighbor is current node");
			}

			if (not visitedNodes.contains(Edge.mNeighbor)) {
				nodesToVisit.push({Edge.mNeighbor, Edge});
			}
		}

		numIters++;
	}

	if (destReached) {
		City_T currentNode = aEndCity;

		bool sourceReached = false;
		int numIters = 0;
		while (not sourceReached) {
			Leg_T incomingEdge = visitedNodes.at(currentNode);
			City_T prevNode = incomingEdge.mCity1 == currentNode ? incomingEdge.mCity2 : incomingEdge.mCity1;

			shortestPathByLegs.emplace_back(incomingEdge);

			if (prevNode == aStartCity) {
				sourceReached = true;
			}
			currentNode = prevNode;
			numIters++;
		}

		if (sourceReached) {
			spdlog::info("Backtracked path in {} iterations", numIters);
			std::reverse(shortestPathByLegs.begin(), shortestPathByLegs.end());
		}
		else {
			spdlog::error("Did not reach source while backtracking");
		}
	} else {
		spdlog::error("Could not reach destination within {} iterations", MaxIters);
	}

	return shortestPathByLegs;
}