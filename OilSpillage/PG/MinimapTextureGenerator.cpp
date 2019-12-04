#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../STB/stb_image_write.h"
#include <cassert>
#include "MinimapTextureGenerator.hpp"
#include "MapConfig.hpp"
#include "District.hpp"
#include "Environment.hpp"

// Tweak these to alter minimap generation:
static auto constexpr tileSide         {  5U  }; // e.g. 5 => 5x5 pixel tiles
static auto constexpr scanlineInterval {  4U  }; // lines between scanlines
static auto constexpr scanlineStrength { .03f }; // brightening/darkening impact of scanlines; 0 = disabled
static auto constexpr multitileBorder  {  1U  }; // thickness of border around multitile houses (in pixels)
static auto constexpr roadBorder       {  1U  }; // thickness of sidewalk around roads          (in pixels)
static auto constexpr districtBlendFac { .1f };

#pragma warning( disable : 4715 ) 
String createMinimapTexture( Map const &map, Bool isDistrictColoured )
{
	auto numDistricts = District::Type::size();
	Vector<RGBA> districtColors( numDistricts );
					 districtColors[District::residential  .index] = 0xFF'00FFFF;
					 districtColors[District::park         .index] = 0xFF'00FF00;
					 districtColors[District::metropolitan .index] = 0xFF'0000FF;
					 districtColors[District::suburban     .index] = 0xFF'FFFF00;
					 districtColors[District::downtown     .index] = 0xFF'FF0000;

   auto const &tilemap = map.getTileMap();
   // image data:
   Size const    TEX_WIDTH   { tilemap.width  * tileSide },
                 TEX_HEIGHT  { tilemap.height * tileSide };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   // random number generation:
   RNG         rng{ RD()() };
   F32_Dist    generateSelection  { .0f, 1.0f };

	Vector<std::pair<U16,U16>> hospitalMarkerDrawingSchedule {};
	hospitalMarkerDrawingSchedule.reserve( map.getDistrictMap().noise.size() );

   // sub-tile pixel offsets:
   I64 const   C   {               0 },
               N   { -(I64)TEX_WIDTH },
               E   {              +1 },
               S   { +(I64)TEX_WIDTH },
               W   {              -1 };
   I64 const   NE  {           N + E },
               SE  {           S + E },
               SW  {           S + W },
               NW  {           N + W };
	auto const &hospitals = map.getHospitalTable();

   // converts a tile-space XY into a pixel XY:
   auto centerIndex {
		[TEX_WIDTH]( I64 const tileX, I64 const tileY ) constexpr {
			return ((tileSide*tileY)+(tileSide/2)) * TEX_WIDTH + ((tileSide*tileX)+(tileSide/2));
   }};

	auto nwIndex {
		[TEX_WIDTH]( I64 const tileX, I64 const tileY ) constexpr {
			return (tileSide*tileY) * TEX_WIDTH + (tileSide*tileX);
   }};

	auto constexpr concreteColorBase = RGBA(0xFF'888888);
	auto constexpr buildingColorBase = RGBA(0xFF'39456B);
	auto constexpr asphaltColorBase  = RGBA(0xFF'222222);

	auto biome = map.getInfo().environment.getBiome();

	auto maybeDistrictShade = [isDistrictColoured]( RGBA &targetColor, RGBA districtColor ) {
		 if ( isDistrictColoured ) targetColor = util::blendColor(districtColor, targetColor, districtBlendFac );
	};

   // produces random ground color:
   auto generateGroundColor {
		[&generateSelection,&maybeDistrictShade,&rng,isDistrictColoured,biome,concreteColorBase]( RGBA districtColor, District::Enum district ) {
			RGBA groundColor = 0xFF'FF00FF; // magenta default (for error detection)
			// set base color
			if ( district == &District::metropolitan )
				groundColor = concreteColorBase;
			else switch (biome) {
				case Biome::sandy: groundColor = 0xFF'99BBFF; break;
				case Biome::grass: groundColor = 0xFF'3EADA4; break;
				case Biome::snowy: groundColor = 0xFF'DDCCCC; break;
				case Biome::burnt: groundColor = 0xFF'282020; break;
				default: assert(false and "Unaccounted for biome type!");
			}
			// add noise
			if      ( generateSelection(rng) <= .05f ) groundColor = util::blendColor(0xFF'FFFFFF, groundColor, .05f );
			else if ( generateSelection(rng) >= .95f ) groundColor = util::blendColor(0xFF'000000, groundColor, .05f );
			// apply potential district shade
			maybeDistrictShade( groundColor, districtColor );
			// final color
			return groundColor;
   }};

   // main loop:
   for ( U16 tileY = 0;  tileY < tilemap.height;  ++tileY ) {
      for ( U16 tileX = 0;  tileX < tilemap.width;  ++tileX ) {
			auto district      { map.districtAt(tileX,tileY)     };
			RGBA districtColor { districtColors[district->index] };
			auto nw            { nwIndex(tileX, tileY)           };
			auto tileIndex   = [&nw,TEX_WIDTH]( U32 x, U32 y ) {
				return nw+x+(TEX_WIDTH*y); // return the pixel index of the tile local x,y
			};
         // TODO: add district borders?
         switch ( tilemap.tileAt(tileX, tileY) ) {
            case Tile::ground: {
					for ( auto x = 0;  x < tileSide; ++x )
						for ( auto y = 0;  y < tileSide; ++y )
							pixels[tileIndex(x,y)] = generateGroundColor(districtColor,district);
            }; break;
            
            // TODO: add building ID or floor influence
            // TODO: add 3D parallax effect?
            case Tile::building: {
					bool isHospital = false;
					for ( auto const possibleHospitalLocation : hospitals ) {
						if ( possibleHospitalLocation ) {
							auto hospitalXY = possibleHospitalLocation.value();
							if ( (hospitalXY.x == tileX) and (hospitalXY.y == tileY) )
								isHospital = true;
						}
					}
					if ( isHospital ) // if hospital, defer drawing until later
						hospitalMarkerDrawingSchedule.push_back({ tileX, tileY });
					// assume single-tile (overwrite later if multi-tile)
					RGBA innerBuildingColor { buildingColorBase };
					RGBA outerBuildingColor { util::blendColor( 0xFF'000000, buildingColorBase, .1f ) };
					maybeDistrictShade(innerBuildingColor, districtColor);
					maybeDistrictShade(outerBuildingColor, districtColor);
					for ( auto x = 0;  x < tileSide; ++x ) {
						for ( auto y = 0;  y < tileSide; ++y ) {
							if ( (((x+1) % tileSide)==0) or (((y+1) % tileSide)==0) ) // outer:
								pixels[tileIndex(x,y)] = outerBuildingColor;
							else // inner:
								pixels[tileIndex(x,y)] = innerBuildingColor;
						}
					}
            }; break;

            case Tile::road: {
               RGBA  roadOuterColor = concreteColorBase;
               RGBA  roadInnerColor = asphaltColorBase;
					maybeDistrictShade(roadOuterColor, districtColor);
					maybeDistrictShade(roadInnerColor, districtColor);

					F64 constexpr offset    = -.5f * tileSide;
					F64 constexpr radius    = (.5f * tileSide)- roadBorder;
					F64 constexpr radiusSqr = radius * radius;
					for ( F64 x = 0;  x < tileSide; ++x )
						for ( F64 y = 0;  y < tileSide; ++y )
							if ( (offset+x)*(offset+x)+(offset+y)*(offset+y) <= radiusSqr )
								pixels[tileIndex(x,y)] = roadInnerColor;
							else
								pixels[tileIndex(x,y)] = roadOuterColor;

					if ( tilemap.neighbourIsRoad(Direction::north, tileX, tileY) )
						for ( auto y = 0;  y < (tileSide+1)/2; ++y )
							for ( auto x = roadBorder;  x < tileSide-roadBorder; ++x )
								pixels[tileIndex(x,y)] = roadInnerColor; 
					if ( tilemap.neighbourIsRoad(Direction::east, tileX, tileY) )
						for ( auto y = roadBorder;  y < tileSide-roadBorder; ++y )
							for ( auto x = tileSide-(tileSide+1)/2;  x < tileSide; ++x )
								pixels[tileIndex(x,y)] = roadInnerColor; 
					if ( tilemap.neighbourIsRoad(Direction::south, tileX, tileY) )
						for ( auto y = tileSide-(tileSide+1)/2;  y < tileSide; ++y )
							for ( auto x = roadBorder;  x < tileSide-roadBorder; ++x )
								pixels[tileIndex(x,y)] = roadInnerColor; 
					if ( tilemap.neighbourIsRoad(Direction::west, tileX, tileY) )
						for ( auto y = roadBorder;  y < tileSide-roadBorder; ++y )
							for ( auto x = 0;  x < (tileSide+1)/2; ++x )
								pixels[tileIndex(x,y)] = roadInnerColor;
            }; break;

            case Tile::water: {
					for ( auto x = 0;  x < tileSide; ++x )
						for ( auto y = 0;  y < tileSide; ++y )
							pixels[tileIndex(x,y)] = 0xFF'FF0000; // TODO
            }; break;

            default: assert( false and "Unaccounted for enum value!" );
         }
      }
	}

	// Multi-tile houses:
	for ( auto &multi : map.getHouseData().multis ) {
		auto const &layout = multi.layout;
		auto  index = [&layout]( U32 x, U32 y) { return y * layout.width + x; };
		// generate masks and compute the final number of parts:
		auto  masks       = Vector<NeighbourMask>( Size(layout.width) * layout.length );
		U32   numParts    = { 0 };
		U32   numHitboxes = { 0 };
		for ( U32 x = 0;  x < layout.width;  ++x ) {
			for ( U32 y = 0;  y < layout.length;  ++y ) {
				U32   idx                { index(x,y)         };
				auto  floorCount         { layout.floors[idx] };
				auto  tileX              { x + multi.nw.x     },
				      tileY              { y + multi.nw.y     };
				auto nw           = nwIndex(tileX,tileY);
				auto tileIndex    = [&nw,TEX_WIDTH]( U32 x, U32 y ) {
					return nw+x+(TEX_WIDTH*y); // return the pixel index of the tile local x,y
				};

				RGBA  districtColor      { districtColors[map.districtAt(tileX,tileY)->index] };
				RGBA  buildingColorInner { buildingColorBase };
				RGBA  buildingColorOuter { util::blendColor( concreteColorBase, 0xFF'FFFFFF, .9f ) };
				maybeDistrictShade(buildingColorInner, districtColor);
				maybeDistrictShade(buildingColorOuter, districtColor);
				if ( layout.floors[idx] != 0 ) {
					bool connectsN = (y-1 < layout.length) and (layout.floors[index( x, y-1 )] > 0);
					if ( !connectsN )
						for ( auto x = multitileBorder;  x < (tileSide-multitileBorder);  ++x )
							for ( auto y = 0;  y < multitileBorder;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					bool connectsE = (x+1 < layout.width) and (layout.floors[index( x+1, y )] > 0 );
					if ( !connectsE )
						for ( auto x = tileSide-multitileBorder;  x < tileSide;  ++x )
							for ( auto y = multitileBorder;  y < (tileSide-multitileBorder);  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					bool connectsS = (y+1 < layout.length) and (layout.floors[index( x, y+1 )] > 0 );
					if ( !connectsS )
						for ( auto x = multitileBorder;  x < (tileSide-multitileBorder);  ++x )
							for ( auto y = tileSide-multitileBorder;  y < tileSide;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					bool connectsW = (x-1 < layout.width) and (layout.floors[index( x-1, y )] > 0 );
					if ( !connectsW )
						for ( auto x = 0;  x < multitileBorder;  ++x )
							for ( auto y = multitileBorder;  y < (tileSide-multitileBorder);  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					if ( !connectsN or !connectsE or (y-1 >= layout.length)
					     or (x+1 >= layout.width) or (layout.floors[index(x+1,y-1)] == 0) )
						for ( auto x = (tileSide-multitileBorder);  x < tileSide;  ++x )
							for ( auto y = 0;  y < multitileBorder;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					if ( !connectsS or !connectsE or (y+1 >= layout.length)
					     or (x+1 >= layout.width) or (layout.floors[index(x+1,y+1)] == 0) )
						for ( auto x = (tileSide-multitileBorder);  x < tileSide;  ++x )
							for ( auto y = (tileSide-multitileBorder);  y < tileSide;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					if ( !connectsS or !connectsW or (y+1 >= layout.length)
					     or (x-1 >= layout.width) or (layout.floors[index(x-1,y+1)] == 0) )
						for ( auto x = 0;  x < multitileBorder;  ++x )
							for ( auto y = (tileSide-multitileBorder);  y < tileSide;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;

					if ( !connectsN or !connectsW or (y-1 >= layout.length)
					     or (x-1 >= layout.width) or (layout.floors[index(x-1,y-1)] == 0) )
						for ( auto x = 0;  x < multitileBorder;  ++x )
							for ( auto y = 0;  y < multitileBorder;  ++y )
								pixels[tileIndex(x,y)] = buildingColorOuter;
				}
			}
		}
	}

	// scanlines
	for ( auto y = 0;  y < TEX_HEIGHT;  ++y ) {
		for ( auto x = 0; x < TEX_WIDTH;  ++x ) {
			if ((y % scanlineInterval) == 0)
				pixels[y*TEX_WIDTH+x] = util::blendColor( 0xFF'000000, pixels[y*TEX_WIDTH+x], scanlineStrength );
			else if ((y % (scanlineInterval+1)) == 0)
				pixels[y*TEX_WIDTH+x] = util::blendColor( 0xFF'FFFFFF, pixels[y*TEX_WIDTH+x], scanlineStrength );
		}
	}

	for ( auto &[tileX,tileY] : hospitalMarkerDrawingSchedule ) {
		auto hospitalPrimaryColor = 0xFF'FFFFFF;
		auto hospitalOutlineColor = 0xFF'0000FF;
		auto center               = centerIndex(tileX,tileY);
		auto nw                   = nwIndex(tileX,tileY);
		auto tileIndex            = [&nw,TEX_WIDTH]( U32 x, U32 y ) {
			return nw+x+(TEX_WIDTH*y); // return the pixel index of the tile local x,y
		};
		auto setPixelIfInBounds   = [&]( I64 pixelIndex, RGBA color ) {
										       if ( pixelIndex < TEX_WIDTH * TEX_HEIGHT )
										          pixels[ pixelIndex ] = color;
										  };
		// hospital border:
		float constexpr radius    = .5f * tileSide;
		float constexpr radiusSqr = radius * radius;
		for ( I32 x = -radius;  x < radius; ++x )
			for ( I32 y = -radius;  y < radius; ++y )
				if ( (x*x) + (y*y) <= radiusSqr )
					setPixelIfInBounds( center+x+(y*I32(TEX_WIDTH)), hospitalOutlineColor );
		// hospital cross:
		for ( U32 y = .2f * tileSide;  y < .8f * tileSide;  ++y )
			for ( U32 x = .4f * tileSide;  x < .6f * tileSide;  ++x )
				pixels[tileIndex(x,y)] = hospitalPrimaryColor;
		for ( U32 y = .4 * tileSide;  y < .6f * tileSide;  ++y )
			for ( U32 x = .2f * tileSide;  x < .8f * tileSide;  ++x )
				setPixelIfInBounds( tileIndex(x,y), hospitalPrimaryColor );
   }

   auto path = String("data/textures/map/map.tga");// +mapConfigToFilename(map.config, ".tga");
   stbi_write_tga( path.c_str(), static_cast<I32>(TEX_WIDTH), static_cast<I32>(TEX_HEIGHT), 4, pixels.data() );
   return String("map/map");// +mapConfigToFilename(map.config);
}


#pragma warning( disable : 4715 ) 
String createFogOfWarTexture( Map const &map )
{
   // image data:
	// auto minimum = util::minValue( map.getTileMap().width  * 3, map.getTileMap().height * 3 );
	// Size side;
	// for ( side = 1; side < minimum;  side*=2 );
		
   Size const    TEX_WIDTH   { map.getTileMap().width  * tileSide },
                 TEX_HEIGHT  { map.getTileMap().height * tileSide };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   // random number generation:
   RNG         rng{ RD()() };
   F32_Dist    generateSelection  { .0f, 1.0f };

   // produces random fog color:
   auto generateFogColor {
		[&generateSelection,&rng]() {
			RGBA color;
			if      ( generateSelection(rng) <= .05f ) color = util::blendColor( 0xFF'FFFFFF, 0xFF'000000, .00f );
			else if ( generateSelection(rng) <= .95f ) color = util::blendColor( 0xFF'FFFFFF, 0xFF'000000, .10f );
			else                                       color = util::blendColor( 0xFF'FFFFFF, 0xFF'000000, .15f );
			return color;
   }};

	for ( auto &pixel: pixels )
		pixel = generateFogColor();

	stbi_write_tga( "data/textures/map/fog.tga",
                   static_cast<I32>(TEX_WIDTH),
                   static_cast<I32>(TEX_HEIGHT),
                   4, pixels.data() );
	return String("map/fog");
}

#pragma warning( disable : 4715 ) 
String createDistanceTexture( Map const &map )
{
   Size const    TEX_WIDTH   { map.getTileMap().width  },
                 TEX_HEIGHT  { map.getTileMap().height };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

	auto const &tilemap     = map.getTileMap();
	auto const &distanceMap = map.getRoadDistanceMap();

	float furthest = .0f;
	for ( auto x=0;  x < tilemap.width; ++x )
		for ( auto y=0;  y < tilemap.height; ++y ) {
			auto dist = distanceMap[tilemap.index(x,y)];
			if ( dist > furthest )
				furthest = dist;
			}

	for ( auto x=0;  x < tilemap.width; ++x )
		for ( auto y=0;  y < tilemap.height; ++y ) {
			auto dist = distanceMap[tilemap.index(x,y)];
			pixels[y*TEX_WIDTH+x] = I32(dist/furthest*255)         // RR
                              + (I32(dist/furthest*255) <<  8)  // GG
                              + (I32(dist/furthest*255) << 16)  // BB
                              + (I32(255)               << 24); // AA
		}

	stbi_write_tga( "data/textures/map/distance.tga",
                   static_cast<I32>(TEX_WIDTH),
                   static_cast<I32>(TEX_HEIGHT),
                   4, pixels.data() );
	return String("map/distance");
}
