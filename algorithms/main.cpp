#include <iostream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // Initialize spdlog
    spdlog::info("Starting Ticket to Ride Graph Viewer");
    
    // Print hello world
    std::cout << "Hello, World!" << std::endl;
    
    // Demonstrate nlohmann json usage
    json game_data = {
        {"game", "Ticket to Ride"},
        {"version", "1.0"},
        {"players", {"red", "blue", "green", "yellow", "black"}}
    };
    
    spdlog::info("Game data: {}", game_data.dump());
    
    // Demonstrate spdlog levels
    spdlog::debug("Debug message");
    spdlog::info("Info message");
    spdlog::warn("Warning message");
    spdlog::error("Error message");
    
    return 0;
} 