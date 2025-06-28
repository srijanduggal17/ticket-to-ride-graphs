#pragma once

#include <uuid/uuid.h>
#include <string>
#include <stdexcept>
#include <array>

using UUID_T = std::array<unsigned char, 16>;

class UUID {
public:
// Static method to parse UUID from string
static UUID_T parseUUID(const std::string& uuid_str) {
	UUID_T uuid;
	if (uuid_parse(uuid_str.c_str(), uuid.data()) != 0) {
		throw std::invalid_argument("Invalid UUID format: " + uuid_str);
	}
	return uuid;
}

// Static method to generate a random UUID
static UUID_T generate() {
	UUID_T uuid;
	uuid_generate(uuid.data());
	return uuid;
}

// Static method to convert UUID to string
static std::string toString(const UUID_T& uuid) {
	char uuid_str[37];         // UUID string is 36 chars + null terminator
	uuid_unparse_lower(uuid.data(), uuid_str);
	return std::string(uuid_str);
}

// Static method to compare two UUIDs
static bool equals(const UUID_T& uuid1, const UUID_T& uuid2) {
	return uuid_compare(uuid1.data(), uuid2.data()) == 0;
}

// Static method to check if UUID is null/empty
static bool isNull(const UUID_T& uuid) {
	return uuid_is_null(uuid.data()) != 0;
}
};