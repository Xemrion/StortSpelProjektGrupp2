#pragma once

#include "defs.hpp"
#include "utils.hpp"
#include <unordered_map>

enum class Time    : U8 { sunrise, noon, sunset, night,                                       /* end */ size };
enum class Biome   : U8 { grass, sandy, snowy, burnt,                                         /* end */ size };
enum class Weather : U8 { clear, overcast, fog, rain, thunderstorm, snow, sandstorm, ashfall, /* end */ size };

// TODO: refactor out
class Environment {
public:
  Environment( RNG &rng ):
    time    ( util::randomEnumOf< Time>( rng )                     ),
    biome   ( util::randomEnumOf<Biome>( rng )                     ),
    weather ( util::randomElementOf( weatherTable.at(biome), rng ) )
  {}

	Environment():
		time    ( Time::sunrise  ),
		biome   ( Biome::grass   ),
		weather ( Weather::clear )
	{}
  
  inline static const std::unordered_map<Biome, std::vector<Weather>> weatherTable {{
     // key: Biome   // value: list of Weathers valid for the Biome
     {Biome::grass, { Weather::clear, Weather::overcast, Weather::fog, Weather::rain    }},
     {Biome::sandy, { Weather::clear, Weather::overcast, Weather::sandstorm             }},
     {Biome::snowy, { Weather::clear, Weather::overcast, Weather::fog, Weather::snow    }},
     {Biome::burnt, { Weather::clear, Weather::overcast, Weather::fog, Weather::ashfall }}
  }};
  
  inline Time    getTime()    const noexcept { return time;    }
  inline Biome   getBiome()   const noexcept { return biome;   }
  inline Weather getWeather() const noexcept { return weather; }
  
private:
  Time     time;
  Biome    biome;
  Weather  weather;
};

inline auto stringify( Biome b ) noexcept {
	std::string result {};
	switch(b) {
		case Biome::sandy:          { result = "desert";       break; }
		case Biome::grass:          { result = "grasslands";   break; }
		case Biome::snowy:          { result = "arctic";       break; }
		case Biome::burnt:          { result = "ashlands";     break; }
		default:                    { result = "N/A";          break; }
	}
	return result;
}

inline auto stringify( Time t ) noexcept {
	std::string result {};
	switch(t) {
		case Time::sunrise:         { result = "sunrise";      break; }
		case Time::noon:            { result = "noon";         break; }
		case Time::sunset:          { result = "sunset";       break; }
		case Time::night:           { result = "night";        break; }
		default:                    { result = "N/A";          break; }
	}
	return result;
}

inline auto stringify( Weather w ) noexcept {
	std::string result {};
	switch(w) {
		case Weather::clear:        { result = "clear";        break; }
		case Weather::overcast:     { result = "overcast";     break; }
		case Weather::fog:          { result = "fog";          break; }
		case Weather::rain:         { result = "rain";         break; }
		case Weather::thunderstorm: { result = "thunderstorm"; break; }
		case Weather::snow:         { result = "snow";         break; }
		case Weather::sandstorm:    { result = "sandstorm";    break; }
		case Weather::ashfall:      { result = "ashfall";      break; }
		default:                    { result = "N/A";          break; }
	}
	return result;
}
