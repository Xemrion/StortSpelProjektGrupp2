#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../STB/stb_image_write.h"
#include <cassert>
#include "MinimapTextureGenerator.hpp"
#include "MapConfig.hpp"
#include "District.hpp"

// TODO (low prio) animated scanlines & gauss effects on map? coloured mode?

#pragma warning( disable : 4715 ) 
String createMinimapTexture( Map const &map, Bool isDistrictColoured )
{
	static auto constexpr districtBlendFac { .05f };
	auto numDistricts = District::Type::size();
	Vector<RGBA> districtColors( numDistricts );
					 districtColors[District::residential  .index] = 0xFF'00FFFF;
					 districtColors[District::park         .index] = 0xFF'00FF00;
					 districtColors[District::metropolitan .index] = 0xFF'0000FF;
					 districtColors[District::suburban     .index] = 0xFF'FFFF00;
					 districtColors[District::downtown     .index] = 0xFF'FF0000;

   auto const &tilemap = map.getTileMap();
   // image data:
   Size const    TEX_WIDTH   { tilemap.width  * 3 },
                 TEX_HEIGHT  { tilemap.height * 3 };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   // random number generation:
   RNG         rng{ RD()() };
   F32_Dist    generateSelection  { .0f, 1.0f };

   // converts a tile-space XY into a pixel XY:
   auto centerIndex {
		[TEX_WIDTH]( I64 const tileX, I64 const tileY ) constexpr {
			return (3*tileY+1) * TEX_WIDTH + (3*tileX+1);
   }};

	auto constexpr concreteColorBase = RGBA(0xFF'888888);
	auto constexpr buildingColorBase = RGBA(0xFF'39456B);
	auto constexpr asphaltColorBase  = RGBA(0xFF'222222);

	auto biome = map.getBiome();

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
				case Biome::desert: groundColor = 0xFF'99BBFF; break;
				case Biome::grass:  groundColor = 0xFF'3EADA4; break;
				case Biome::snow:   groundColor = 0xFF'DDCCCC; break;
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

   // main loop:
   for ( U16 tileY = 0;  tileY < tilemap.height;  ++tileY ) {
      for ( U16 tileX = 0;  tileX < tilemap.width;  ++tileX ) {
			auto district      = map.districtAt(tileX,tileY);
			RGBA districtColor = districtColors[ district->index ];

         // TODO: add district borders?
         switch ( tilemap.tileAt(tileX, tileY) ) {
            case Tile::ground: {
               pixels[centerIndex(tileX,tileY)+NW] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+N ] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+NE] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+ W] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+ C] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+ E] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+SW] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+S ] = generateGroundColor(districtColor,district);
               pixels[centerIndex(tileX,tileY)+SE] = generateGroundColor(districtColor,district);
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
					else {
						RGBA buildingColor = buildingColorBase;
						maybeDistrictShade(buildingColor, districtColor);
						pixels[centerIndex(tileX,tileY)+NW] = buildingColor;
						pixels[centerIndex(tileX,tileY)+N ] = buildingColor;
						pixels[centerIndex(tileX,tileY)+NE] = buildingColor;
						pixels[centerIndex(tileX,tileY)+ W] = buildingColor;
						pixels[centerIndex(tileX,tileY)+ C] = buildingColor;
						pixels[centerIndex(tileX,tileY)+ E] = buildingColor;
						pixels[centerIndex(tileX,tileY)+SW] = buildingColor;
						pixels[centerIndex(tileX,tileY)+S ] = buildingColor;
						pixels[centerIndex(tileX,tileY)+SE] = buildingColor;
					}
            }; break;

            case Tile::road: {
               RGBA  roadOuterColor = concreteColorBase;
               RGBA  roadInnerColor = asphaltColorBase;
					maybeDistrictShade(roadOuterColor, districtColor);
					maybeDistrictShade(roadInnerColor, districtColor);
               pixels[centerIndex(tileX,tileY)+NW] = roadOuterColor;
               pixels[centerIndex(tileX,tileY)+N ] = tilemap.neighbourIsRoad(Direction::north,tileX,tileY)? roadInnerColor:roadOuterColor;
               pixels[centerIndex(tileX,tileY)+NE] = roadOuterColor;
               pixels[centerIndex(tileX,tileY)+ W] = tilemap.neighbourIsRoad(Direction::west, tileX,tileY)? roadInnerColor:roadOuterColor;
               pixels[centerIndex(tileX,tileY)+ C] = roadInnerColor;
               pixels[centerIndex(tileX,tileY)+ E] = tilemap.neighbourIsRoad(Direction::east, tileX,tileY)? roadInnerColor:roadOuterColor;
               pixels[centerIndex(tileX,tileY)+SW] = roadOuterColor;
               pixels[centerIndex(tileX,tileY)+S ] = tilemap.neighbourIsRoad(Direction::south,tileX,tileY)? roadInnerColor:roadOuterColor;
               pixels[centerIndex(tileX,tileY)+SE] = roadOuterColor;
            }; break;

            case Tile::water: {
               pixels[centerIndex(tileX,tileY)+NW] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+N ] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+NE] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+ W] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+ C] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+ E] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+SW] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+S ] = 0xFF'FF0000; // TODO
               pixels[centerIndex(tileX,tileY)+SE] = 0xFF'FF0000; // TODO
            }; break;

            default: assert( false and "Unaccounted for enum value!" );
         }

			// scanlines
			pixels[centerIndex(tileX,tileY)+N+W] = util::blendColor( pixels[centerIndex(tileX,tileY)+N+W], 0xFF000000, .98f );
			pixels[centerIndex(tileX,tileY)+N  ] = util::blendColor( pixels[centerIndex(tileX,tileY)+N  ], 0xFF000000, .98f );
			pixels[centerIndex(tileX,tileY)+N+E] = util::blendColor( pixels[centerIndex(tileX,tileY)+N+E], 0xFF000000, .98f );
			pixels[centerIndex(tileX,tileY)+S+W] = util::blendColor( pixels[centerIndex(tileX,tileY)+S+W], 0xFFFFFFFF, .98f );
			pixels[centerIndex(tileX,tileY)+S  ] = util::blendColor( pixels[centerIndex(tileX,tileY)+S  ], 0xFFFFFFFF, .98f );
			pixels[centerIndex(tileX,tileY)+S+E] = util::blendColor( pixels[centerIndex(tileX,tileY)+S+E], 0xFFFFFFFF, .98f );
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
				U32  idx        = index(x,y);
				auto floorCount = layout.floors[idx];
				auto tileX      = x + multi.nw.x,
				     tileY      = y + multi.nw.y;
				RGBA districtColor = districtColors[ map.districtAt(tileX,tileY)->index ];
				RGBA  buildingColorInner  { buildingColorBase };
				RGBA  buildingColorOuter  { util::blendColor( concreteColorBase, 0xFF'FFFFFF, .9f ) };
				maybeDistrictShade(buildingColorInner, districtColor);
				maybeDistrictShade(buildingColorOuter, districtColor);
				if ( layout.floors[idx] != 0 ) {
					pixels[centerIndex(tileX,tileY)] = buildingColorInner;

					bool connectsN = (y-1 < layout.length) and (layout.floors[index( x, y-1 )] > 0);
					if ( connectsN )
					     pixels[centerIndex(tileX,tileY)+N ] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+N ] = buildingColorOuter;

					bool connectsE = (x+1 < layout.width) and (layout.floors[index( x+1, y )] > 0 );
					if ( connectsE )
					     pixels[centerIndex(tileX,tileY)+E ] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+E ] = buildingColorOuter;

					bool connectsS = (y+1 < layout.length) and (layout.floors[index( x, y+1 )] > 0 );
					if ( connectsS )
					     pixels[centerIndex(tileX,tileY)+S ] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+S ] = buildingColorOuter;

					bool connectsW = (x-1 < layout.width) and (layout.floors[index( x-1, y )] > 0 );
					if ( connectsW )
					     pixels[centerIndex(tileX,tileY)+W ] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+W ] = buildingColorOuter;

					if ( connectsN and connectsE and ((y-1 < layout.length) and (x+1 < layout.width) and (layout.floors[index( x+1, y-1 )] > 0 )) )
					     pixels[centerIndex(tileX,tileY)+NE] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+NE] = buildingColorOuter;

					if ( connectsS and connectsE and ((y+1 < layout.length) and (x+1 < layout.width) and (layout.floors[index( x+1, y+1 )] > 0 )) )
					     pixels[centerIndex(tileX,tileY)+SE] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+SE] = buildingColorOuter;

					if ( connectsS and connectsW and ((y+1 < layout.length) and (x-1 < layout.width) and (layout.floors[index( x-1, y+1 )] > 0 )) )
					     pixels[centerIndex(tileX,tileY)+SW] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+SW] = buildingColorOuter;

					if ( connectsN and connectsW and ((y-1 < layout.length) and (x-1 < layout.width) and (layout.floors[index( x-1, y-1 )] > 0 )) )
					     pixels[centerIndex(tileX,tileY)+NW] = buildingColorInner;
					else pixels[centerIndex(tileX,tileY)+NW] = buildingColorOuter;
				}
			}
		}
	}

	for ( auto &[tileX,tileY] : hospitalMarkerDrawingSchedule ) {
		auto hospitalPrimaryColor = 0xFF'FFFFFF;
		auto hospitalOutlineColor = 0xFF'0000FF;
		pixels[centerIndex(tileX,tileY)+NW] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+N ] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+NE] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+ W] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+ C] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+ E] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+SW] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+S ] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+SE] = hospitalOutlineColor;

		auto setPixelIfInBounds =	[&]( I64 pixelIndex, RGBA color ) {
												if ( pixelIndex < TEX_WIDTH * TEX_HEIGHT )
													pixels[ pixelIndex ] = color;
											};

		// hospital border:
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NW,   hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NW+W, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NW+N, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NE,   hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NE+E, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ NE+N, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SE,   hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SE+E, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SE+S, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SW,   hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SW+W, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ SW+S, hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ N+N,  hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ S+S,  hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ W+W,  hospitalOutlineColor );
		setPixelIfInBounds( I64(centerIndex(tileX, tileY))+ E+E,  hospitalOutlineColor );
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
		
   Size const    TEX_WIDTH   { map.getTileMap().width  * 3 },
                 TEX_HEIGHT  { map.getTileMap().height * 3 };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   // random number generation:
   RNG         rng{ RD()() };
   F32_Dist    generateSelection  { .0f, 1.0f };

   // produces random fog color:
   auto generateFogColor {
		[&generateSelection,&rng]() {
			RGBA color;
			if      ( generateSelection(rng) <= .05f ) color = 0xFF'333333;
			else if ( generateSelection(rng) <= .95f ) color = 0xFF'000000;
			else                                       color = 0xFF'212121;
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
