#pragma once

#include <cassert>
#include "defs.hpp"

enum class Dir : U8 { north, east, south, west };

constexpr Dir turn_left(Dir d) noexcept {
	if      ( d == Dir::east  )  return Dir::north;
	else if ( d == Dir::north )  return Dir::west;
	else if ( d == Dir::south )  return Dir::east;
	else   /* d == Dir::west */  return Dir::south;
}

constexpr Dir turn_right(Dir d) noexcept {
	if      ( d == Dir::east  )  return Dir::south;
	else if ( d == Dir::north )  return Dir::east;
	else if ( d == Dir::south )  return Dir::west;
	else   /* d == Dir::west */  return Dir::north;
}

constexpr Dir turn_around(Dir d) noexcept {
	if      ( d == Dir::east  )  return Dir::west;
	else if ( d == Dir::north )  return Dir::south;
	else if ( d == Dir::south )  return Dir::north;
	else   /* d == Dir::west */  return Dir::east;
}
