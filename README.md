# Ticket to Ride Graph Viewer

A web-based visualization tool for viewing Ticket to Ride game boards and current game states. This tool allows you to load the board representation and current game states to visualize the board and which legs have been built by players already.

## View Modes: 
- **Base Colors**: Shows original route colors for planning
- **Player Ownership**: Shows claimed routes by player colors

## File Formats

### Base Graph Data
JSON file containing the base board representation:
```json
{
  "cities": [
    {
      "name": "Seattle",
      "x": 50,
      "y": 80
    }
  ],
  "connections": [
    {
      "city1": "Seattle",
      "city2": "Vancouver", 
      "color": "gray",
      "cost": 1,
      "id": "unique-uuid-here"
    }
  ]
}
```

### Board State Data
JSON file containing current game state:
```json
{
  "red": ["edge-uuid-1", "edge-uuid-2"],
  "blue": ["edge-uuid-3"],
  "green": [],
  "yellow": ["edge-uuid-4"],
  "black": []
}
```

## File Validation

The tool validates:
- JSON format correctness
- Required fields in base graph (cities, connections)
- Required players in board state (red, blue, green, yellow, black)
- UUID format for connection IDs
- Array format for player edge lists
