#pragma once

#include <vector>

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "board_defs.h"

inline Route_T MakeRoute(const nlohmann::json & aInputJSON) {
	return {
	        .mCity1 = cityFromString(aInputJSON["city1"].get<std::string>()),
	        .mCity2 = cityFromString(aInputJSON["city2"].get<std::string>()),
	        .mPoints = aInputJSON["points"].get<uint8_t>()
	};
}

using RouteList = std::vector<Route_T>;

RouteList GenerateRouteList(const nlohmann::json &aInputJSON) {
	RouteList myRoutes;
	myRoutes.reserve(aInputJSON.size());

	for (const auto & RouteObj : aInputJSON) {
		myRoutes.emplace_back(MakeRoute(RouteObj));
	}

	return myRoutes;
}