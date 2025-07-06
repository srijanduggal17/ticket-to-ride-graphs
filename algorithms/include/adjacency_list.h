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

using TTRAdjacencyList = std::unordered_map<City_T, std::vector<Leg_T> >;

/**
 * @brief Make adjacency list from json file
 *
 * @param aInputJSON - Already validate JSON
    - Assumed to include two keys: cities, and connections
 */
TTRAdjacencyList GenerateAdjacencyList(const nlohmann::json & aInputJSON) {
	TTRAdjacencyList myList;

	// Add cities
	for (const auto &CityJSON : aInputJSON["cities"]) {
		City_T myCity = cityFromString(CityJSON["name"].get<std::string>());
		myList[myCity] = std::vector<Leg_T>();
		myList.at(myCity).reserve(HighestDegreeOfNode);
	}

	// Add edges
	for (const auto &ConnectionJSON : aInputJSON["connections"]) {
		Leg_T myLeg = MakeLeg(ConnectionJSON);

		myLeg.mNeighbor = myLeg.mCity2;
		myList.at(myLeg.mCity1).emplace_back(myLeg);
		spdlog::trace("Added edge: {}", myLeg);

		myLeg.mNeighbor = myLeg.mCity1;
		myList.at(myLeg.mCity2).emplace_back(myLeg);
		spdlog::trace("Added edge: {}", myLeg);
	}

	return myList;
}