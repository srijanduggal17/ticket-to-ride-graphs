#include <iostream>
#include <fstream>
#include <random>

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "adjacency_list.h"
#include "route_list.h"
#include "algos/bfs.h"
#include "utils.h"

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

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, myRoutes.size() - 1);
	int randomIndex = dis(gen);
	Route_T targetRoute = myRoutes.at(randomIndex);

	Path_T shortestPath = BFS(myGraph, targetRoute.mCity1, targetRoute.mCity2);
	spdlog::info("Found shortest path from {} to {}:\n{}", targetRoute.mCity1, targetRoute.mCity2, shortestPath);

	nlohmann::json outputJSON = createEmptyBoardState();
	outputJSON["red"] = pathToList(shortestPath);
	std::ofstream outputFile("path_output.json");
	if (!outputFile.is_open()) {
		spdlog::error("Failed to open output file for writing");
		return 1;
	}
	outputFile << outputJSON.dump(4);
	outputFile.close();
	spdlog::info("Path output written to path_output.json");
	return 0;
}