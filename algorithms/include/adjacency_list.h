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

using TTRAdjacencyList = std::unordered_map<City_T, std::vector<Leg_T> >;

constexpr int HighestDegreeOfNode = 10;

inline Leg_T MakeLeg(const nlohmann::json & aInputJSON) {
	Leg_T myLeg = {
		.mNeighbor = cityFromString(aInputJSON["city1"].get<std::string>()),
		.mCity1 = cityFromString(aInputJSON["city1"].get<std::string>()),
		.mCity2 = cityFromString(aInputJSON["city2"].get<std::string>()),
		.mCost = aInputJSON["cost"].get<uint8_t>(),
		.mColor = cardColorFromString(aInputJSON["color"].get<std::string>()),
		.mEdgeID = UUID::parseUUID(aInputJSON["id"].get<std::string>())
	};

	// Enforce that city1 < city2
	if (myLeg.mCity1 > myLeg.mCity2) {
		std::swap(myLeg.mCity1, myLeg.mCity2);
	}

	return myLeg;
}

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
		std::string cityName = replaceSpaces(CityJSON["name"].get<std::string>());

		auto myCity = magic_enum::enum_cast<City_T>(cityName);
		if (myCity.has_value()) {
			// Add city with empty vector for edges
			myList[myCity.value()] = std::vector<Leg_T>();
			myList.at(myCity.value()).reserve(HighestDegreeOfNode);
		} else {
			spdlog::error("Couldn't find city: {}", cityName);
		}
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