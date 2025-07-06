#pragma once

#include "uuid.h"

enum class City_T : uint8_t {
	Seattle = 0U,
	Vancouver,
	Calgary,
	Portland,
	San_Francisco,
	Los_Angeles,
	Las_Vegas,
	Salt_Lake_City,
	Phoenix,
	Helena,
	Winnipeg,
	Denver,
	Santa_Fe,
	El_Paso,
	Omaha,
	Duluth,
	Sault_St_Marie,
	Kansas_City,
	Oklahoma_City,
	Dallas,
	Houston,
	Chicago,
	Saint_Louis,
	Little_Rock,
	New_Orleans,
	Toronto,
	Montreal,
	Pittsburgh,
	Nashville,
	Atlanta,
	Raleigh,
	Washington,
	New_York,
	Boston,
	Charleston,
	Miami
};

enum class CardColor_T : uint8_t {
	pink,
	red,
	green,
	white,
	orange,
	yellow,
	blue,
	black,
	gray
};

struct Leg_T {
	City_T mNeighbor; // for purposes of traversal
	City_T mCity1;
	City_T mCity2;
	uint8_t mCost;
	CardColor_T mColor;
	UUID_T mEdgeID;
};

struct Route_T {
	City_T mCity1;
	City_T mCity2;
	uint8_t mPoints;
};

struct CityNode {
	City_T mCity;
	std::string mName;
	int mX;
	int mY;
};

struct Edge {
	const CityNode * mFrom;
	const CityNode * mTo;
	CardColor_T mColor;
	uint8_t mCost;
	UUID_T mEdgeID;
};

constexpr int HighestDegreeOfNode = 10;

using Path_T = std::vector<Leg_T>;
using EdgePath_T = std::vector<const Edge*>;

/**
 * @brief Replace periods and spaces with underscores in a string
 *
 * @param input - The input string to process
 * @return std::string - The string with periods and spaces replaced by underscores
 */
std::string replaceSpaces(const std::string& input) {
	std::string result = input;
	for (char& c : result) {
		if (c == ' ') {
			c = '_';
		}
	}
	return result;
}

inline City_T cityFromString(const std::string & aName) {
	auto city = magic_enum::enum_cast<City_T>(replaceSpaces(aName));
	if (not city.has_value()) {
		throw std::runtime_error(fmt::format("Invalid city: {}", aName));
	}
	return city.value();
}

inline CardColor_T cardColorFromString(const std::string &aColor) {
	auto color = magic_enum::enum_cast<CardColor_T>(aColor);
	if (not color.has_value()) {
		throw std::runtime_error(fmt::format("Invalid color: {}", aColor));
	}
	return color.value();
}

inline Leg_T MakeLeg(const nlohmann::json & aInputJSON) {
	Leg_T myLeg = {
		.mNeighbor = cityFromString(aInputJSON["city1"].get<std::string>()),
		.mCity1 = cityFromString(aInputJSON["city1"].get<std::string>()),
		.mCity2 = cityFromString(aInputJSON["city2"].get<std::string>()),
		.mCost = aInputJSON["cost"].get<uint8_t>(),
		.mColor = cardColorFromString(aInputJSON["color"].get<std::string>()),
		.mEdgeID = UUID::parseUUID(aInputJSON["id"].get<std::string>())
	};

	// Enforce that city1 < city2
	if (myLeg.mCity1 > myLeg.mCity2) {
		std::swap(myLeg.mCity1, myLeg.mCity2);
	}

	return myLeg;
}

inline CityNode MakeCityNode(const nlohmann::json &aInputJSON) {
	spdlog::info(aInputJSON.dump());
	std::string cityName = aInputJSON["name"].get<std::string>();
	return {
	        .mCity = cityFromString(cityName),
	        .mName = aInputJSON["name"].get<std::string>(),
	        .mX = aInputJSON["x"].get<int>(),
	        .mY = aInputJSON["y"].get<int>()
	};
}

inline Edge MakeEdge(const nlohmann::json &aInputJSON,
                     const CityNode * aFrom,
                     const CityNode * aTo) {
	std::string colorString = aInputJSON["color"].get<std::string>();
	std::string idString = aInputJSON["id"].get<std::string>();
	return {
	        .mFrom = aFrom,
	        .mTo = aTo,
	        .mColor = cardColorFromString(colorString),
	        .mCost = aInputJSON["cost"].get<uint8_t>(),
	        .mEdgeID = UUID::parseUUID(idString)
	};
}

template<>
struct fmt::formatter<City_T> : formatter<string_view> {
	auto format(City_T city, format_context& ctx) const {
		return formatter<string_view>::format(magic_enum::enum_name(city), ctx);
	}
};

template<>
struct fmt::formatter<CardColor_T> : formatter<string_view> {
	auto format(CardColor_T color, format_context& ctx) const {
		return formatter<string_view>::format(magic_enum::enum_name(color), ctx);
	}
};

template<>
struct fmt::formatter<Leg_T> : formatter<string_view> {
	auto format(const Leg_T& leg, format_context& ctx) const {
		return formatter<string_view>::format(
			fmt::format("Leg({} <-> {}, cost: {}, color: {}, id: {}, neighbor: {})",
			            leg.mCity1,
			            leg.mCity2,
			            leg.mCost,
			            magic_enum::enum_name(leg.mColor),
			            UUID::toString(leg.mEdgeID),
			            leg.mNeighbor),
			ctx);
	}
};

template<>
struct fmt::formatter<CityNode> : formatter<string_view> {
	auto format(const CityNode& aCity, format_context& ctx) const {
		return formatter<string_view>::format(
			fmt::format("City {} at ({},{})",
			            aCity.mName,
			            aCity.mX,
			            aCity.mY),
			ctx);
	}
};

template<>
struct fmt::formatter<Edge> : formatter<string_view> {
	auto format(const Edge& aEdge, format_context& ctx) const {
		return formatter<string_view>::format(
			fmt::format("Edge({} <-> {}, cost: {}, color: {}, id: {})",
			            aEdge.mFrom->mName,
			            aEdge.mTo->mName,
			            aEdge.mCost,
			            magic_enum::enum_name(aEdge.mColor),
			            UUID::toString(aEdge.mEdgeID)),
			ctx);
	}
};

template<>
struct fmt::formatter<Route_T> : formatter<string_view> {
	auto format(const Route_T& route, format_context& ctx) const {
		return formatter<string_view>::format(
			fmt::format("Route({} <-> {}: {} pts)",
			            route.mCity1,
			            route.mCity2,
			            route.mPoints),
			ctx);
	}
};

template<>
struct fmt::formatter<Path_T> : formatter<string_view> {
	auto format(const Path_T& path, format_context& ctx) const {
		if (path.empty()) {
			return formatter<string_view>::format("Path(empty)", ctx);
		}

		std::string pathStr = "Path(\n";
		for (size_t i = 0; i < path.size(); ++i) {
			if (i > 0) {
				pathStr += " -> ";
			}
			pathStr += fmt::format("\t{}-{}: {}\n", path[i].mCity1, path[i].mCity2, UUID::toString(path[i].mEdgeID));
		}
		pathStr += ")";

		return formatter<string_view>::format(pathStr, ctx);
	}
};

template<>
struct fmt::formatter<EdgePath_T> : formatter<string_view> {
	auto format(const EdgePath_T& path, format_context& ctx) const {
		if (path.empty()) {
			return formatter<string_view>::format("Path(empty)", ctx);
		}

		std::string pathStr = "Path(\n";
		for (size_t i = 0; i < path.size(); ++i) {
			if (i > 0) {
				pathStr += " -> ";
			}
			pathStr += fmt::format("\t{}-{}: {}\n", path[i]->mFrom->mName, path[i]->mTo->mName, UUID::toString(path[i]->mEdgeID));
		}
		pathStr += ")";

		return formatter<string_view>::format(pathStr, ctx);
	}
};
