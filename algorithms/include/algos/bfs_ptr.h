#pragma once

#include <queue>
#include <algorithm>
#include <unordered_map>

#include "board_defs.h"
#include "adjacency_list2.h"

/**
 * @brief Use Breadth-First Search to get shortest path between two cities
 *
 * @param aGraphAdjList - graph representing game board
 * @param aStartCity - start city
 * @param aEndCity - end city
 * @return Path_T
 */
inline EdgePath_T BFS_Ptr(const TTRAdjacencyList2& aGraphAdjList, const CityNode* aStartCity, const CityNode* aEndCity) {
	std::unordered_map<const CityNode*, const Edge*> visitedNodes;

	std::queue<std::pair<const CityNode*, const Edge*> > nodesToVisit;
	nodesToVisit.push({aStartCity, nullptr});

	bool destReached = false;
	int numIters = 0;
	int MaxIters = 36+100*2;
	// until queue is empty too
	while (numIters < MaxIters and not nodesToVisit.empty()) {
		const auto &[CurrentNode, EdgeToGetHere] = nodesToVisit.front();
		nodesToVisit.pop();

		if (visitedNodes.contains(CurrentNode)) {
			continue;
		}

		const std::vector<const Edge *> & AdjacentNodes = aGraphAdjList.at(CurrentNode);
		visitedNodes.emplace(CurrentNode, EdgeToGetHere);

		if (CurrentNode == aEndCity) {
			destReached = true;
			spdlog::info("Found shortest path in {} iterations", numIters);
			break;
		}

		for (const Edge * Edge : AdjacentNodes) {
			if (Edge->mTo == CurrentNode) {
				spdlog::error("Edge neighbor is current node");
			}

			if (not visitedNodes.contains(Edge->mTo)) {
				nodesToVisit.push({Edge->mTo, Edge});
			}
		}

		numIters++;
	}

	EdgePath_T shortestPathByLegs;

	if (destReached) {
		const CityNode* currentNode = aEndCity;

		bool sourceReached = false;
		int numIters = 0;
		while (not sourceReached) {
			const Edge* incomingEdge = visitedNodes.at(currentNode);

			if (incomingEdge == nullptr) {
				spdlog::error("Reached end of graph while recording path");
			}

			shortestPathByLegs.emplace_back(incomingEdge);

			if (incomingEdge->mFrom == aStartCity) {
				sourceReached = true;
			}
			currentNode = incomingEdge->mFrom;
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