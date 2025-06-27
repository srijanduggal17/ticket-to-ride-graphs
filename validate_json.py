#!/usr/bin/env python3
import json
import re
from typing import List, Literal
from pydantic import BaseModel, Field, validator, root_validator

# Valid colors for connections
ValidColors = Literal["gray", "white", "orange", "pink", "yellow", "blue", "green", "black", "red"]

class City(BaseModel):
    name: str
    x: int
    y: int

class Connection(BaseModel):
    city1: str
    city2: str
    color: ValidColors
    cost: int = Field(ge=1, le=6)  # Between 1 and 6
    id: str
    
    @validator('id')
    def validate_uuid(cls, v):
        uuid_pattern = re.compile(r'^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$', re.IGNORECASE)
        if not uuid_pattern.match(v):
            raise ValueError(f'ID must be a valid UUID: {v}')
        return v
    
    @root_validator
    def validate_city_order(cls, values):
        city1 = values.get('city1')
        city2 = values.get('city2')
        if city1 and city2 and city1 >= city2:
            raise ValueError(f"city1 '{city1}' should be alphabetically before city2 '{city2}'")
        return values

class TicketToRideData(BaseModel):
    cities: List[City]
    connections: List[Connection]
    
    @validator('cities')
    def validate_city_count(cls, v):
        if len(v) != 36:
            raise ValueError(f'Expected 36 cities, found {len(v)}')
        return v
    
    @validator('cities')
    def validate_unique_city_names(cls, v):
        names = [city.name for city in v]
        duplicates = [name for name in set(names) if names.count(name) > 1]
        if duplicates:
            raise ValueError(f'Duplicate city names: {duplicates}')
        return v
    
    @validator('connections')
    def validate_connection_cities(cls, v, values):
        if 'cities' not in values:
            return v
        
        valid_cities = {city.name for city in values['cities']}
        
        for connection in v:
            if connection.city1 not in valid_cities:
                raise ValueError(f"Connection city1 '{connection.city1}' is not a valid city")
            if connection.city2 not in valid_cities:
                raise ValueError(f"Connection city2 '{connection.city2}' is not a valid city")
        
        return v
    
    @validator('connections')
    def validate_unique_connection_ids(cls, v):
        ids = [connection.id for connection in v]
        duplicates = [conn_id for conn_id in set(ids) if ids.count(conn_id) > 1]
        if duplicates:
            raise ValueError(f'Duplicate connection IDs: {duplicates}')
        return v

def validate_json_file(file_path: str) -> dict:
    """Validate the Ticket to Ride JSON file using Pydantic."""
    results = {
        "valid": True,
        "errors": [],
        "warnings": []
    }
    
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        results["valid"] = False
        results["errors"].append(f"File {file_path} not found")
        return results
    except json.JSONDecodeError as e:
        results["valid"] = False
        results["errors"].append(f"Invalid JSON: {e}")
        return results
    
    try:
        # Validate with Pydantic
        validated_data = TicketToRideData(**data)
        print(f"✅ Found {len(validated_data.cities)} cities")
        print(f"✅ Found {len(validated_data.connections)} connections")
        
    except Exception as e:
        results["valid"] = False
        # Pydantic provides detailed error messages
        if hasattr(e, 'errors'):
            for error in e.errors():
                field_path = ' -> '.join(str(x) for x in error['loc'])
                message = error['msg']
                results["errors"].append(f"{field_path}: {message}")
        else:
            results["errors"].append(str(e))
    
    return results

def main():
    """Main function to run the validation."""
    file_path = "graph_representation.json"
    
    print("Validating Ticket to Ride JSON file with Pydantic...")
    print("=" * 60)
    
    results = validate_json_file(file_path)
    
    if results["valid"]:
        print("✅ Validation PASSED!")
        print(f"Found {len(results['warnings'])} warnings")
    else:
        print("❌ Validation FAILED!")
        print(f"Found {len(results['errors'])} errors")
    
    if results["errors"]:
        print("\nErrors:")
        for error in results["errors"]:
            print(f"  ❌ {error}")
    
    if results["warnings"]:
        print("\nWarnings:")
        for warning in results["warnings"]:
            print(f"  ⚠️  {warning}")
    
    print("\n" + "=" * 60)
    return results["valid"]

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1) 