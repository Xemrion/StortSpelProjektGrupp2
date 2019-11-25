#pragma once

#include <cassert>
#include "defs.hpp"

namespace District {
   class Type {
	public:
		explicit Type( String name,
		               F32    maxDistFromRoad,
		               F32    buildingDensity,
		               U8     minArea,
		               U8     maxArea,
		               U8     minFloors,
		               U8     maxFloors )
	:
		name            ( name            ),
		maxDistFromRoad ( maxDistFromRoad ),
		buildingDensity ( buildingDensity ),
		minArea         ( minArea         ),
		maxArea         ( maxArea         ),
		minFloors       ( minFloors       ),
		maxFloors       ( maxFloors       ),
      index           ( nextIndex++     )
	{}
	~Type() {};
	Type( Type const &)             = delete;
	Type( Type &&)                  = delete;
	Type& operator=( Type &&)       = delete;
	Type& operator=( Type  const &) = delete;
		F32    const maxDistFromRoad,
		             buildingDensity;
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
	//                                                road    house    min    max    min      max
	//                               string name      dist    cover%   tiles  tiles  floors   floors
	inline Type const residential  { "residential",   3.0f,   .8f,     1,     1,     4,        6 },
	                  park         { "park",          3.0f,   .8f,     1,     1,     1,        1 },
	                  metropolitan { "metropolitan",  3.0f,   .9f,     1,     1,     8,       16 },
	                  suburban     { "suburban",      3.0f,   .8f,     1,     1,     1,        2 },
	                  downtown     { "downtown",      3.0f,   .8f,     1,     1,     3,        4 };
}