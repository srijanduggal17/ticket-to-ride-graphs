#!/usr/bin/env python3
"""
Validate routes.json against base_board_representation.json
Checks:
1. All cities exist in the base board
2. All points are nonnegative
3. No routes go from a city to itself
"""

import json
import sys
from typing import Set, Dict, Any, List


def load_json_file(filename: str) -> Any:
    """Load and parse a JSON file."""
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in '{filename}': {e}")
        sys.exit(1)


def get_valid_cities(base_board: Dict[str, Any]) -> Set[str]:
    """Extract valid city names from the base board representation."""
    cities = set()
    for city in base_board.get("cities", []):
        cities.add(city["name"])
    return cities


def validate_routes(routes: List[Dict[str, Any]], valid_cities: Set[str]) -> bool:
    """Validate the routes and return True if all valid, False otherwise."""
    errors = []
    
    for i, route in enumerate(routes):
        # Check required fields exist
        if "city1" not in route or "city2" not in route or "points" not in route:
            errors.append(f"Route {i+1}: Missing required fields (city1, city2, or points)")
            continue
        
        city1 = route["city1"]
        city2 = route["city2"]
        points = route["points"]
        
        # Check city1 exists in base board
        if city1 not in valid_cities:
            errors.append(f"Route {i+1}: City '{city1}' not found in base board")
        
        # Check city2 exists in base board
        if city2 not in valid_cities:
            errors.append(f"Route {i+1}: City '{city2}' not found in base board")
        
        # Check points is nonnegative
        if not isinstance(points, int) or points < 0:
            errors.append(f"Route {i+1}: Points must be a nonnegative integer, got {points}")
        
        # Check route doesn't go from city to itself
        if city1 == city2:
            errors.append(f"Route {i+1}: Cannot route from '{city1}' to itself")
    
    # Print all errors
    if errors:
        print("Validation errors found:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    return True


def main():
    """Main validation function."""
    print("Validating routes.json...")
    
    # Load the files
    base_board = load_json_file("base_board_representation.json")
    routes = load_json_file("routes.json")
    
    # Get valid cities
    valid_cities = get_valid_cities(base_board)
    print(f"Found {len(valid_cities)} valid cities in base board")
    
    # Validate routes
    if validate_routes(routes, valid_cities):
        print(f"✅ All {len(routes)} routes are valid!")
        return 0
    else:
        print("❌ Validation failed!")
        return 1


if __name__ == "__main__":
    sys.exit(main()) 