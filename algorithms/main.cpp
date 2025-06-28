#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "adjacency_list.h"

int main(int argc, char* argv[]) {
	// Check if file argument is provided
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <json_file>" << std::endl;
		return 1;
	}

	// Get filename from command line argument
	std::string filename = argv[1];

	// Load JSON file
	nlohmann::json game_board;
	try {
		std::ifstream file(filename.c_str());
		if (!file.is_open()) {
			std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
			return 1;
		}
		file >> game_board;
	} catch (const nlohmann::json::parse_error& e) {
		std::cerr << "Error: Invalid JSON in file '" << filename << "': " << e.what() << std::endl;
		return 1;
	}
	// Initialize spdlog
	spdlog::info("Starting Ticket to Ride Graph Viewer");

	TTRAdjacencyList myGraph = GenerateAdjacencyList(game_board);

	int totalEdges = 0;
	for (const auto& [city, edges] : myGraph) {
		totalEdges += edges.size();
	}
	totalEdges /= 2;
	spdlog::info("Total edges in graph: {}", totalEdges);

	return 0;
}