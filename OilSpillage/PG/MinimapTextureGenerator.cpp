#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../STB/stb_image_write.h"
#include <cassert>
#include "MinimapTextureGenerator.hpp"
#include "MapConfig.hpp"

// TODO (low prio) scanlines & gauss effects on map?

#pragma warning( disable : 4715 ) 
String createMinimapTexture( Map const &map ) {
   auto const &tilemap = map.getTileMap();
   // image data:
   Size const    TEX_WIDTH          { tilemap.width  * 3 },
                 TEX_HEIGHT         { tilemap.height * 3 };
   Vector<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   // random number generation:
   RNG         rng{ RD()() };
   F32_Dist    generateSelection  { .0f, 1.0f };

   // converts a tile-space XY into a pixel XY:
   auto        centerIndex        {[TEX_WIDTH]( U16 const tileX, U16 const tileY ) constexpr {
                                     return (3*tileY+1) * TEX_WIDTH + (3*tileX+1);
                                  }};

   // produces random ground color:
   auto        groundColor        {[&generateSelection,&rng]() {
                                          if ( generateSelection(rng) <= .05f ) return 0xFF'2B2B2B;
                                     else if ( generateSelection(rng) <= .95f ) return 0xFF'333333;
                                     else                                       return 0xFF'3B3B3B;
                                  }};

	// 
	Vector<std::pair<U16,U16>> hospitalMarkerDrawingSchedule {};
	hospitalMarkerDrawingSchedule.reserve( map.getDistrictMap().noise.size() );

   // sub-tile pixel offsets:
   I32 const   C                  {               0 },
               N                  { -(I32)TEX_WIDTH },
               E                  {              +1 },
               S                  { +(I32)TEX_WIDTH },
               W                  {              -1 };
   I32 const   NE                 {           N + E },
               SE                 {           S + E },
               SW                 {           S + W },
               NW                 {           N + W };
	auto const &hospitals = map.getHospitalTable();
   // main loop:
   for ( U16 tileY = 0;  tileY < tilemap.height;  ++tileY ) {
      for ( U16 tileX = 0;  tileX < tilemap.width;  ++tileX ) {
         // TODO: add district borders?
         auto district            { map.getDistrictMap() };
         Size tileDistrictID      { district.diagram[ district.diagramIndex(tileX,tileY)] };
         RGBA districtColorOffset ( 0x00'040404 * ((tileDistrictID+499)%7) );
         switch ( tilemap.tileAt(tileX, tileY) ) {
            case Tile::ground: {
               pixels[centerIndex(tileX,tileY)+NW] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+N ] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+NE] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+ W] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+ C] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+ E] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+SW] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+S ] = groundColor() + districtColorOffset;
               pixels[centerIndex(tileX,tileY)+SE] = groundColor() + districtColorOffset;
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
						auto buildingColor = 0xFF'777777 + districtColorOffset;
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
               RGBA  roadInnerColor { 0xFF'BBBBBB                       };
               RGBA  roadOuterColor { 0xFF'222222 + districtColorOffset };
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

            default: assert( false && "Unaccounted for enum value!" );
         }
      }
	}
	for ( auto &[tileX,tileY] : hospitalMarkerDrawingSchedule ) {
		auto hospitalPrimaryColor = 0xFF'0000FF;
		auto hospitalOutlineColor = 0xFF'FFFFFF;
		pixels[centerIndex(tileX,tileY)+NW] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+N ] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+NE] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+ W] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+ C] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+ E] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+SW] = hospitalOutlineColor;
		pixels[centerIndex(tileX,tileY)+S ] = hospitalPrimaryColor;
		pixels[centerIndex(tileX,tileY)+SE] = hospitalOutlineColor;

		auto setPixelIfInBounds =	[&]( U16 pixelIndex, RGBA color ) {
												if ( pixelIndex < TEX_WIDTH * TEX_HEIGHT )
													pixels[ pixelIndex ] = color;
											};

		// border:
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NW,   hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NW+W, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NW+N, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NE,   hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NE+E, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ NE+N, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SE,   hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SE+E, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SE+S, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SW,   hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SW+W, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ SW+S, hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ N+N,  hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ S+S,  hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ W+W,  hospitalOutlineColor );
		setPixelIfInBounds( centerIndex(tileX, tileY)+ E+E,  hospitalOutlineColor );
   }

   auto path = String("data/textures/map/map.tga");// +mapConfigToFilename(map.config, ".tga");
   stbi_write_tga( path.c_str(), static_cast<I32>(TEX_WIDTH), static_cast<I32>(TEX_HEIGHT), 4, pixels.data() );
   return String("map/map");// +mapConfigToFilename(map.config);
}
