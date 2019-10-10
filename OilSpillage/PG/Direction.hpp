#pragma once

#include <cassert>
#include "defs.hpp"

enum class Direction : U8 { north, east, south, west };

constexpr Direction turnLeft(Direction d) noexcept {
	if      ( d == Direction::east  )  return Direction::north;
	else if ( d == Direction::north )  return Direction::west;
	else if ( d == Direction::south )  return Direction::east;
	else   /* d == Dir::west */  return Direction::south;
}

constexpr Direction turnRight(Direction d) noexcept {
	if      ( d == Direction::east  )  return Direction::south;
	else if ( d == Direction::north )  return Direction::east;
	else if ( d == Direction::south )  return Direction::west;
	else   /* d == Dir::west */  return Direction::north;
}

constexpr Direction turn_around(Direction d) noexcept {
	if      ( d == Direction::east  )  return Direction::west;
	else if ( d == Direction::north )  return Direction::south;
	else if ( d == Direction::south )  return Direction::north;
	else   /* d == Dir::west */  return Direction::east;
}
