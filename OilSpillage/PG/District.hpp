#pragma once

#include <cassert>
#include "defs.hpp"

namespace District {
   class Type {
	public:
		explicit Type( String name,
		               F32    maxDistFromRoad,
		               F32    buildingDensity,
		               F32    multitilePercentage,
		               U8     minArea,
		               U8     maxArea,
		               U8     minFloors,
		               U8     maxFloors )
	:
		name                ( name                ),
		maxDistFromRoad     ( maxDistFromRoad     ),
		buildingDensity     ( buildingDensity     ),
		multitilePercentage ( multitilePercentage ),
		minArea             ( minArea             ),
		maxArea             ( maxArea             ),
		minFloors           ( minFloors           ),
		maxFloors           ( maxFloors           ),
      index               ( nextIndex++         )
	{}
	~Type() {};
	Type( Type const &)             = delete;
	Type( Type &&)                  = delete;
	Type& operator=( Type &&)       = delete;
	Type& operator=( Type  const &) = delete;
		F32    const maxDistFromRoad,
		             buildingDensity,
		             multitilePercentage;
		U8     const minArea, 
		             maxArea,
		             minFloors,
		             maxFloors,
		             index;
		String const name;
		static Size inline size() noexcept { return nextIndex; }
	private:
		inline static Size nextIndex { 0U };
	};
	//                                                road    house     multi    min    max    min      max
	//                               string name      dist    cover%    ti	le%    tiles  tiles  floors   floors
	inline Type const residential  { "residential",   6.0f,   .875f,    .8f,     2,     25,    3,        7 },
	                  park         { "park",          1.0f,   .000f,    .0f,     0,     0,     0,        0 },
	                  metropolitan { "metropolitan",  4.0f,   .950f,    .0f,     1,     1,     8,       16 },
	                  suburban     { "suburban",      3.5f,   .150f,    .0f,     1,     4,     1,        2 },
	                  downtown     { "downtown",      5.0f,   .925f,    .3f,     2,     8,     5,       10 };
/*
	inline Type const residential  { "residential",   2.0f,   1.0f,    .8f,     2,     14,    3,        8 },
	                  park         { "park",          2.0f,   .00f,    .0f,     0,     0,     0,        0 },
	                  metropolitan { "metropolitan",  2.0f,   1.0f,    .0f,     1,     1,     8,       16 },
	                  suburban     { "suburban",      2.0f,   1.0f,    .0f,     1,     4,     1,        2 },
	                  downtown     { "downtown",      2.0f,   1.0f,    .3f,     6,     16,    1,        6 };
*/
	inline Vector<Type const *> all { &residential, &park, &metropolitan, &suburban, &downtown };

	using Enum = Type const *;
}

inline bool isInnerCity( District::Enum d ) noexcept {
	using namespace District;
	return d==&metropolitan or d==&residential or d==&downtown;
}

inline bool isOuterCity( District::Enum d ) noexcept {
	using namespace District;
	return d==&park or d==&suburban;
}
