#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "board_defs.h"

inline nlohmann::json loadJSONFromFile(const std::string & aFilePath) {
	// Load JSON file
	nlohmann::json outputJSON;
	try {
		std::ifstream file(aFilePath.c_str());
		if (!file.is_open()) {
			throw std::runtime_error(fmt::format("Error: Could not open file {}", aFilePath));
		}
		file >> outputJSON;
	} catch (const nlohmann::json::parse_error& e) {
		throw std::runtime_error(fmt::format("Error: Invalid JSON in file {}: {}", aFilePath, e.what()));
	}

	return outputJSON;
}

inline nlohmann::json pathToList(const Path_T& aPath) {
	nlohmann::json pathList = nlohmann::json::array();

	for (const auto& leg : aPath) {
		pathList.push_back(UUID::toString(leg.mEdgeID));
	}

	return pathList;
}

inline nlohmann::json edgePathToList(const EdgePath_T& aPath) {
	nlohmann::json pathList = nlohmann::json::array();

	for (const auto& leg : aPath) {
		pathList.push_back(UUID::toString(leg->mEdgeID));
	}

	return pathList;
}

inline nlohmann::json createEmptyBoardState() {
	nlohmann::json boardState;

	// Initialize all color arrays as empty
	boardState["red"] = nlohmann::json::array();
	boardState["blue"] = nlohmann::json::array();
	boardState["green"] = nlohmann::json::array();
	boardState["yellow"] = nlohmann::json::array();
	boardState["black"] = nlohmann::json::array();

	return boardState;
}