#pragma once

#include "defs.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <cassert>


enum class Time    : U8 { sunrise, noon, sunset, night,                                       /* end */ size };
enum class Biome   : U8 { grass, sandy, snowy, burnt,                                         /* end */ size };
enum class Weather : U8 { clear, overcast, fog, rain, thunderstorm, snow, sandstorm, ashfall, /* end */ size };

// TODO: refactor out
class Environment {
public:
  Environment( RNG &rng ):
    time    ( util::randomEnumOf< Time>( rng )                     ),
    biome   ( util::randomEnumOf<Biome>( rng )                     ),
    weather ( util::randomElementOf( weatherTable.at(biome), rng ) ),
    name    ( generateCityName(rng)                                )
  {}

	Environment():
		time    ( Time::sunrise  ),
		biome   ( Biome::grass   ),
		weather ( Weather::clear )
	{}
  
  inline String   getName()    const noexcept { return name;    }
  inline Time     getTime()    const noexcept { return time;    }
  inline Biome    getBiome()   const noexcept { return biome;   }
  inline Weather  getWeather() const noexcept { return weather; }
  
private:

	inline static const std::unordered_map<Biome, std::vector<Weather>> weatherTable {{
		// key: Biome      // value: list of Weathers valid for the Biome
		{ Biome::grass,     { Weather::clear, Weather::overcast, Weather::fog, Weather::rain    }},
		{ Biome::sandy,     { Weather::clear, Weather::overcast, Weather::sandstorm             }},
		{ Biome::snowy,     { Weather::clear, Weather::overcast, Weather::fog, Weather::snow    }},
		{ Biome::burnt,     { Weather::clear, Weather::overcast, Weather::fog, Weather::ashfall }}
	}};

	inline static const std::unordered_map<Biome, std::vector<StringView>> biomeNamePrefix {{
		// key: Biome      // value: list of Weathers valid for the Biome
		{ Biome::grass,     { "Happy", "Pleasant", "Green", "Oak", "Plain", "Grass", "Bush", "Wood", "Forest", "Farm"  }},
		{ Biome::sandy,     { "Sandy", "Dry", "Oasis", "Sand", "Desert", "Sun", "Dry", "Beach", "Pyramid" }},
		{ Biome::snowy,     { "Snowy", "Arctic", "Icy", "Cold", "Frozen", "Santa", "Christmas", "White" }},
		{ Biome::burnt,     { "Hell", "Ash", "Pyro", "Ruin", "Black", "Grey", "Apocalypse" }}
	}};

	static std::array constexpr genericCityPrefix { "Murder", "Mega", "Necro", "Mayhem", "Death", "Techno", "Metal", "Rot", "Doom", "Happy", "Joy", "Oil", "Bone", "Car", "Auto", "Capitol", "Liberty", "Massacre", "Carnage", "Gas", "Robo", "Robot", "Car", "Tesla", "Giga", "Splatter", "Bloodbath", "Factory", "Electro", "Skull", "Kill", "Hobo", "Junk", "Gear", "Bunker", "Silo", "Gearbox", "Petrol", "Torture", "Sunset", "Chrome", "Graveyard", "Omega", "Tera", "Terror", "Scream", "Corpse", "Rock" };
	static std::array constexpr citySuffix { "town", " Town", " City", " Village", "ville", "burg", "stadt", "opolis", "heim", " Meadows", " Creek", " Base", " Metropolis", "land", "lands", " Space", " Zone", " Point" };

	std::string generateCityName( RNG &rng ) noexcept {
		F32_Dist generateSelection { .0, 1.0f };
		if ( generateSelection(rng) < 75.0f )
			return std::string(util::randomElementOf(biomeNamePrefix.at(biome), rng)) + util::randomElementOf(citySuffix, rng);
		else return std::string(util::randomElementOf(genericCityPrefix, rng)) + util::randomElementOf(citySuffix, rng);
	}

	Time     time;
	Biome    biome;
	Weather  weather;
	String   name;
};

inline auto stringify( Biome b ) noexcept {
	std::string result {};
	switch(b) {
		case Biome::sandy:          { result = "desert";          break; }
		case Biome::grass:          { result = "grasslands";      break; }
		case Biome::snowy:          { result = "arctic";          break; }
		case Biome::burnt:          { result = "ashlands";        break; }
		default:                    { assert(false && "Invalid value!"); }
	}
	return result;
}

inline auto stringify( Time t ) noexcept {
	std::string result {};
	switch(t) {
		case Time::sunrise:         { result = "sunrise";         break; }
		case Time::noon:            { result = "noon";            break; }
		case Time::sunset:          { result = "sunset";          break; }
		case Time::night:           { result = "night";           break; }
		default:                    { assert(false && "Invalid value!"); }
	}
	return result;
}

inline auto stringify( Weather w ) noexcept {
	std::string result {};
	switch(w) {
		case Weather::clear:        { result = "clear";           break; }
		case Weather::overcast:     { result = "overcast";        break; }
		case Weather::fog:          { result = "fog";             break; }
		case Weather::rain:         { result = "rain";            break; }
		case Weather::thunderstorm: { result = "thunderstorm";    break; }
		case Weather::snow:         { result = "snow";            break; }
		case Weather::sandstorm:    { result = "sandstorm";       break; }
		case Weather::ashfall:      { result = "ashfall";         break; }
		default:                    { assert(false && "Invalid value!"); }
	}
	return result;
}
