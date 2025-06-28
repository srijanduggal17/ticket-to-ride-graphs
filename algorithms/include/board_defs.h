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

using Path_T = std::vector<Leg_T>;

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
