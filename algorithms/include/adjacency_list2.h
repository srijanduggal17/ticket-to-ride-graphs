#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "board_defs.h"
#include "utils.h"

using TTRAdjacencyList2 = std::unordered_map<const CityNode *, std::vector<const Edge *> >;

/*
   City * -> [Edge *]

   City: {
    "name": "Seattle",
    "x": 50,
    "y": 80
   }

   Edge:
    {
        "city1": "El Paso",
        "city2": "Los Angeles",
        "color": "black",
        "cost": 6,
        "id": "d62ff9a4-bf82-4068-955b-9ce3204dde61"
    }

   Then do the same problem using pointers but do adjacency list and edge list
 */

/**
 * @brief Make adjacency list from json file
 *
 * @param aInputJSON - Already validate JSON
    - Assumed to include two keys: cities, and connections
 */
TTRAdjacencyList2 GenerateAdjacencyList2(const std::unordered_map<City_T, CityNode> & aCityNodes, const std::vector<Edge> &aEdges) {
	TTRAdjacencyList2 myList;

	// Add cities
	for (const auto & [_, City] : aCityNodes) {
		myList[&City] = std::vector<const Edge *>();
		myList.at(&City).reserve(HighestDegreeOfNode);
	}

	// Add edges
	for (const Edge &Edge : aEdges) {
		myList.at(Edge.mFrom).emplace_back(&Edge);
		spdlog::trace("Added edge: {}", Edge);
	}

	return myList;
}