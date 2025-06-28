#include <iostream>
#include <fstream>

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "adjacency_list.h"
#include "route_list.h"

nlohmann::json loadJSONFromFile(const std::string & aFilePath) {
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

int main(int argc, char* argv[]) {
	// Check if file argument is provided
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <game_board_json_file> <routes_json_file>" << std::endl;
		return 1;
	}

	// Get filename from command line argument
	std::string boardFilename = argv[1];
	std::string routesFilename = argv[2];
	nlohmann::json gameBoard = loadJSONFromFile(boardFilename);
	nlohmann::json routes = loadJSONFromFile(routesFilename);

	// Initialize spdlog
	spdlog::info("Starting Ticket to Ride Graph Viewer");

	TTRAdjacencyList myGraph = GenerateAdjacencyList(gameBoard);
	RouteList myRoutes = GenerateRouteList(routes);

	int totalEdges = 0;
	for (const auto& [city, edges] : myGraph) {
		totalEdges += edges.size();
	}
	totalEdges /= 2;
	spdlog::info("Total edges in graph: {}", totalEdges);

	return 0;
}