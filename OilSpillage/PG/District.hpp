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
	//                                                road    house    multi    min    max    min      max
	//                               string name      dist    cover%   tile%    tiles  tiles  floors   floors
	inline Type const residential  { "residential",   2.0f,   .85f,    .8f,     2,     14,    3,        8 },
	                  park         { "park",          2.0f,   .00f,    .0f,     0,     0,     0,        0 },
	                  metropolitan { "metropolitan",  2.0f,   .95f,    .0f,     1,     1,     8,       16 },
	                  suburban     { "suburban",      2.0f,   .30f,    .0f,     1,     4,     1,        2 },
	                  downtown     { "downtown",      2.0f,   .90f,    .3f,     6,     16,    1,        6 };
/*
	inline Type const residential  { "residential",   2.0f,   1.0f,    .8f,     2,     14,    3,        8 },
	                  park         { "park",          2.0f,   .00f,    .0f,     0,     0,     0,        0 },
	                  metropolitan { "metropolitan",  2.0f,   1.0f,    .0f,     1,     1,     8,       16 },
	                  suburban     { "suburban",      2.0f,   1.0f,    .0f,     1,     4,     1,        2 },
	                  downtown     { "downtown",      2.0f,   1.0f,    .3f,     6,     16,    1,        6 };*/

	inline Vector<Type const *> all { &residential, &park, &metropolitan, &suburban, &downtown };

	using Enum = Type const *;
}