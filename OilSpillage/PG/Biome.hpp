#pragma once
#include "defs.hpp"

enum class Biome : U8 {
	grass,
	desert,
	snow
};

inline auto stringify( Biome b ) noexcept {
	std::string result = "N/A";
	switch( b ) {
		case Biome::desert: { result = "desert"; break; }
		case Biome::grass:  { result = "grass";  break; }
		case Biome::snow:   { result = "snow";   break; }
	}
	return result;
}
