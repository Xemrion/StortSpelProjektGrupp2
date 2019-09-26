#pragma once

#include <cassert>
#include "defs.hpp"

enum class Dir : U8 { north, east, south, west };

constexpr Dir turn_left(Dir d) noexcept {
	switch (d) {
	   case Dir::north: return Dir::west;
	   case Dir::east:  return Dir::north;
	   case Dir::south: return Dir::east;
	   case Dir::west:  return Dir::south;
	}
}

constexpr Dir turn_right(Dir d) noexcept {
	switch (d) {
	   case Dir::north: return Dir::east;
	   case Dir::east:  return Dir::south;
	   case Dir::south: return Dir::west;
	   case Dir::west:  return Dir::north;
	}
}

constexpr Dir turn_around(Dir d) noexcept {
	switch (d) {
	   case Dir::north: return Dir::south;
	   case Dir::east:  return Dir::west;
	   case Dir::south: return Dir::north;
	   case Dir::west:  return Dir::east;
	}
}
