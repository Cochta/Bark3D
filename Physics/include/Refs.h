#pragma once

#include "cstddef"

struct BodyRef {
	std::size_t Index;
	std::size_t GenIndex;

	constexpr bool operator==(const BodyRef& other) const {
		return (Index == other.Index) && (GenIndex == other.GenIndex);
	}
};
struct BodyRefHash {
	std::size_t operator()(const BodyRef& pair) const {
		const std::size_t hashA = std::hash<size_t>{}(pair.Index);
		const std::size_t hashB = std::hash<size_t>{}(pair.GenIndex);
		// XOR for the hash
		return hashA ^ hashB;
	}
};

struct ColliderRef {
	std::size_t Index;
	std::size_t GenIndex;

	constexpr bool operator==(const ColliderRef& other) const {
		return Index == other.Index && GenIndex == other.GenIndex;
	}
};