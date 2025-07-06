#include <iostream>
#include <fstream>
#include <random>

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "adjacency_list2.h"
#include "route_list.h"
#include "algos/bfs_ptr.h"
#include "utils.h"

int main(int argc, char* argv[]) {
	spdlog::set_level(spdlog::level::info);
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
	spdlog::debug("Loaded game board with {} cities", gameBoard["cities"].size());
	spdlog::debug("Loaded {} routes", routes.size());

	// Make objects
	std::unordered_map<City_T, CityNode> myVertices;
	std::vector<Edge> myEdges;

	spdlog::info(gameBoard.dump());

	for (const auto &CityJSON : gameBoard["cities"]) {
		CityNode myNode = MakeCityNode(CityJSON);
		myVertices[myNode.mCity] = myNode;
	}
	spdlog::trace("Made Vertices");
	for (const auto &ConnectionJSON : gameBoard["connections"]) {
		std::string city1String = ConnectionJSON["city1"].get<std::string>();
		std::string city2String = ConnectionJSON["city2"].get<std::string>();
		City_T City1 = cityFromString(city1String);
		City_T City2 = cityFromString(city2String);
		myEdges.emplace_back(MakeEdge(ConnectionJSON, &myVertices.at(City1), &myVertices.at(City2)));
		myEdges.emplace_back(MakeEdge(ConnectionJSON, &myVertices.at(City2), &myVertices.at(City1)));
	}
	spdlog::trace("Made Edges");

	TTRAdjacencyList2 myGraph = GenerateAdjacencyList2(myVertices, myEdges);
	spdlog::trace("Made Adjacency List");

	RouteList myRoutes = GenerateRouteList(routes);
	spdlog::trace("Made Route List");

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

	EdgePath_T shortestPath = BFS_Ptr(myGraph,
	                                  &myVertices.at(targetRoute.mCity1),
	                                  &myVertices.at(targetRoute.mCity2));
	spdlog::info("Found shortest path from {} to {}:\n{}", targetRoute.mCity1, targetRoute.mCity2, shortestPath);

	nlohmann::json outputJSON = createEmptyBoardState();
	outputJSON["red"] = edgePathToList(shortestPath);
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