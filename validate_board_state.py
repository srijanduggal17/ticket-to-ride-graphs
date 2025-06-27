#!/usr/bin/env python3
import json
import re
from typing import Dict, List, Set

def validate_board_state(board_state_file: str, base_board_file: str) -> dict:
    """
    Validate the current board state JSON file.
    
    Args:
        board_state_file: Path to the current board state JSON file
        base_board_file: Path to the base board representation JSON file
    
    Returns:
        Dictionary with validation results
    """
    results = {
        "valid": True,
        "errors": [],
        "warnings": []
    }
    
    # Expected player colors
    expected_players = {"red", "blue", "green", "yellow", "black"}
    
    try:
        # Load board state
        with open(board_state_file, 'r') as f:
            board_state = json.load(f)
        
        # Load base board representation
        with open(base_board_file, 'r') as f:
            base_board = json.load(f)
        
        print(f"✅ Loaded board state from: {board_state_file}")
        print(f"✅ Loaded base board from: {base_board_file}")
        
    except FileNotFoundError as e:
        results["valid"] = False
        results["errors"].append(f"File not found: {e}")
        return results
    except json.JSONDecodeError as e:
        results["valid"] = False
        results["errors"].append(f"Invalid JSON: {e}")
        return results
    
    # Extract valid connection IDs from base board
    valid_connection_ids = {conn["id"] for conn in base_board["connections"]}
    print(f"✅ Found {len(valid_connection_ids)} valid connection IDs in base board")
    
    # 1. Validate that all expected players are present
    board_players = set(board_state.keys())
    missing_players = expected_players - board_players
    extra_players = board_players - expected_players
    
    if missing_players:
        results["valid"] = False
        results["errors"].append(f"Missing required players: {missing_players}")
    
    if extra_players:
        results["warnings"].append(f"Unexpected players found: {extra_players}")
    
    print(f"✅ Player validation: Found {len(board_players)} players")
    
    # 2. Validate UUID format and collect all claimed edges
    all_claimed_edges = set()
    uuid_pattern = re.compile(r'^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$', re.IGNORECASE)
    
    for player, edges in board_state.items():
        if not isinstance(edges, list):
            results["valid"] = False
            results["errors"].append(f"Player '{player}' edges must be a list, found {type(edges)}")
            continue
        
        print(f"  Player {player}: {len(edges)} edges")
        
        for i, edge_id in enumerate(edges):
            # Validate UUID format
            if not isinstance(edge_id, str):
                results["valid"] = False
                results["errors"].append(f"Player '{player}' edge {i+1} must be a string, found {type(edge_id)}")
                continue
            
            if not uuid_pattern.match(edge_id):
                results["valid"] = False
                results["errors"].append(f"Player '{player}' edge {i+1} is not a valid UUID: {edge_id}")
                continue
            
            # Check for duplicate edges within the same player
            if edge_id in all_claimed_edges:
                results["valid"] = False
                results["errors"].append(f"Edge {edge_id} is claimed by multiple players")
            else:
                all_claimed_edges.add(edge_id)
            
            # Validate that edge exists in base board
            if edge_id not in valid_connection_ids:
                results["valid"] = False
                results["errors"].append(f"Player '{player}' edge {i+1} ({edge_id}) does not exist in base board")
    
    # 3. Check for any duplicate edges across players
    all_edges = []
    for player, edges in board_state.items():
        if isinstance(edges, list):
            all_edges.extend(edges)
    
    duplicates = [edge for edge in set(all_edges) if all_edges.count(edge) > 1]
    if duplicates:
        results["valid"] = False
        results["errors"].append(f"Duplicate edges found across players: {duplicates}")
    
    # 4. Summary statistics
    total_claimed_edges = len(all_claimed_edges)
    print(f"✅ Total claimed edges: {total_claimed_edges}")
    
    # Check if any players have no edges (this is valid, just informational)
    empty_players = [player for player, edges in board_state.items() if isinstance(edges, list) and len(edges) == 0]
    if empty_players:
        print(f"ℹ️  Players with no edges: {empty_players}")
    
    return results

def main():
    """Main function to run the board state validation."""
    board_state_file = "current_board_state.json"
    base_board_file = "base_board_representation.json"
    
    print("Validating current board state...")
    print("=" * 60)
    
    results = validate_board_state(board_state_file, base_board_file)
    
    if results["valid"]:
        print("✅ Board state validation PASSED!")
    else:
        print("❌ Board state validation FAILED!")
    
    if results["errors"]:
        print(f"\n❌ Errors ({len(results['errors'])}):")
        for error in results["errors"]:
            print(f"  • {error}")
    
    if results["warnings"]:
        print(f"\n⚠️  Warnings ({len(results['warnings'])}):")
        for warning in results["warnings"]:
            print(f"  • {warning}")
    
    print("\n" + "=" * 60)
    return results["valid"]

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1) 