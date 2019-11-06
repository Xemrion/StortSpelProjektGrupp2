#include <string_view>
#include "utils.hpp"
#include "Modelizer.hpp"
#include "../GameObject.h"

// Tile neighbours bit layout:
//
//  7     0     1
//    NW  N  NE
//
//  6 W   c   E 2
//
//    SW  S  SE 
//  5     4     3
//
// c is not included (it's intrinsic based off of the currently processed tiled)

// Wraps a tile's type and encodes the possible existence of neighbouring road tiles in the 8 eight directions into bits.
struct TileInfo {
   union {
		U8 bitmap  { 0x00 }; // <- encoding byte; default unset
		struct {             // <- individual bit access interface
			U8 n  : 1,
			   ne : 1,
			   e  : 1,
			   se : 1,
			   s  : 1,
			   sw : 1,
			   w  : 1,
			   nw : 1;
		};
	} roadmap;
	Vector3  origin { .0f, .0f, .0f };
	Tile     type   { Tile::ground  };
};

inline Vector<TileInfo> extractTileInfo( TileMap const &map ) noexcept {
	Vector<TileInfo>  results  { map.data.size() };
TileInfo t;
	for ( auto y=0;  y < map.height;  ++y ) {
		for ( auto x=0;  x < map.width;  ++x ) {
			auto &entry   =  results[ map.index(x,y) ];
			entry.origin  =  map.convertTilePositionToWorldPosition(x,y);
			entry.type    =  map.tileAt(x,y);
			if ( (y > 0) and (map.tileAt(x,y-1) == Tile::road) )
				entry.roadmap.n  = true;
			if ( (x < map.width-1) and (y > 0) and (map.tileAt(x+1,y-1) == Tile::road) )
				entry.roadmap.ne = true;
			if ( (x < map.width-1) and map.tileAt(x+1,y) == Tile::road )
				entry.roadmap.e  = true;
			if ( (x < map.width-1) and (y < map.height-1) and (map.tileAt(x+1,y+1) == Tile::road) )
				entry.roadmap.se = true;
			if ( (y < map.height-1) and (map.tileAt(x,y+1) == Tile::road) )
				entry.roadmap.s  = true;
			if ( (x > 0) and (y < map.height-1) and (map.tileAt(x-1,y+1) == Tile::road) )
				entry.roadmap.sw = true;
			if ( (x > 0) and (map.tileAt(x-1,y) == Tile::road) )
				entry.roadmap.w  = true;
			if ( (x > 0) and (y > 0) and (map.tileAt(x-1,y-1) == Tile::road) )
				entry.roadmap.nw = true;
			std::cout << ' ';
		}
	}
	return results;
}

// NE "outer corner":
//     map     mask    predicate
//     x01     011     001
//     x 0     0 1     0 0
//     xxx     000     000
static U8  constexpr maskOuterC { 0b0000'0111 };
static U8  constexpr predOuterC { 0b0000'0010 };

// N "side":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     xxx     000     000
static U8  constexpr maskSide   { 0b0100'0101 };
static U8  constexpr predSide   { 0b0000'0001 };

// NE "inner corner":
//     map     mask    predicate
// x1x	010	010
// 0 1	1 1	0 1
// x0x	010	000
static U8  constexpr maskInnerC { 0b0101'0101 };
static U8  constexpr predInnerC { 0b0000'0101 };

// N "U":
//     map     mask    predicate
//     x0x     010     000
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr maskU { 0b0101'0101 };
static U8  constexpr predU { 0b0101'0100 };

// "hole":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr maskHole { 0b0101'0101 };
static U8  constexpr predHole { 0b0101'0101 };

// road bitmaps:

static U8  constexpr maskRoad { 0b0101'0101 };

// "deadend":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     x0x     010     000
static U8  constexpr predDeadend { 0b0000'0001 };


// "bend":
//     map     mask    predicate
//     x1x     010     010
//     0 1     1 1     0 1
//     x0x     010     000
static U8  constexpr predBend { 0b0000'0101 };

// "straight":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     x1x     010     010
static U8  constexpr predStraight { 0b0001'0001 };

// "3-way":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x0x     010     000
static U8  constexpr pred3Way { 0b0100'0101 };

// "4-way":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr pred4Way { 0b0101'0101 };

static F32 constexpr baseOffsetY     { -1.50f };
static F32 constexpr sidewalkOffsetY { -1.49f };
static F32 constexpr markerOffsetY   { -1.49f };

Vector<UPtr<GameObject>> instantiateTilesAsModels( Graphics &graphics, TileMap const &map ) noexcept
{
	Vector<UPtr<GameObject>> models;
	models.reserve( map.data.size() * 6 );// worst case scenario: 6 quads per tile
	auto instantiatePart = [&]( std::string_view name, Vector3 const &pos, F32 deg=.0f, F32 yOffset=baseOffsetY, Bool hasNormal=true, Bool noShadowcasting=false ) {
		models.push_back( std::make_unique<GameObject>() );
      auto &model     =  *models.back();
		model.mesh      =   graphics.getMeshPointer( "Tiles/Quad_SS" );
		model.setTexture(   graphics.getTexturePointer(name.data()) );
		if ( hasNormal )
			model.setNormalMap( graphics.getTexturePointer( (std::string(name)+"_nor").c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });
		model.setPosition( pos + Vector3{.0f, yOffset, .0f} );
		model.setColor({ .0f, .0f, .0f, .0f });
		model.setScale({ map.config.tileScaleFactor.x, 1.0f, map.config.tileScaleFactor.z });
		model.setSpotShadow( noShadowcasting );
		model.setSunShadow(  noShadowcasting );
	};
	for ( auto const &e : extractTileInfo(map) ) {
		if ( e.type != Tile::road ) {
			// intrinsic: center c == 0
			instantiatePart( "Tiles/grass", e.origin );

			// place eventual outer sidewalk borders:
			if ( (e.roadmap.bitmap & maskHole) == predHole )
				 instantiatePart( "Tiles/sidewalk_hole", e.origin, sidewalkOffsetY );
			else for ( auto d=0;  d<8;  d+=2 ) {
				if ( (e.roadmap.bitmap & util::cycleLeft( maskOuterC, d )) == util::cycleLeft(predOuterC,d) )
					instantiatePart( "Tiles/sidewalk_corner_outer_ne", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (e.roadmap.bitmap & util::cycleLeft( maskSide,   d )) == util::cycleLeft(predSide,d) )
					instantiatePart( "Tiles/sidewalk_side_n", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (e.roadmap.bitmap & util::cycleLeft( maskInnerC, d )) == util::cycleLeft(predInnerC,d) )
					instantiatePart( "Tiles/sidewalk_corner_inner_ne", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (e.roadmap.bitmap & util::cycleLeft( maskU,      d )) == util::cycleLeft(predU,d) )
					instantiatePart( "Tiles/sidewalk_u_n", e.origin, 45.0f*d, sidewalkOffsetY );
			}
		}
		else if ( e.type == Tile::road ) {
			// intrinsic: center c == 1
			
			// place eventual inner sidewalk borders:
			U8 inversedMap = 0xFFu - e.roadmap.bitmap; // gotta invert since we're inside the road graph now
			if ( (inversedMap & maskHole) == predHole )
				 instantiatePart( "Tiles/sidewalk_hole", e.origin, sidewalkOffsetY );
			else for ( auto d=0; d<8; d+=2 ) {
				if ( (inversedMap & util::cycleLeft( maskOuterC, d )) == util::cycleLeft(predOuterC,d) )
					instantiatePart( "Tiles/sidewalk_corner_outer_ne", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (inversedMap & util::cycleLeft( maskSide,   d )) == util::cycleLeft(predSide,d) )
					instantiatePart( "Tiles/sidewalk_side_n", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (inversedMap & util::cycleLeft( maskInnerC, d )) == util::cycleLeft(predInnerC,d) )
					instantiatePart( "Tiles/sidewalk_corner_inner_ne", e.origin, 45.0f*d, sidewalkOffsetY );
				if ( (inversedMap & util::cycleLeft( maskU,      d )) == util::cycleLeft(predU,d) )
					instantiatePart( "Tiles/sidewalk_u_n", e.origin, 45.0f*d, sidewalkOffsetY );
			}

			// place eventual road lines:
			auto maskedMap = e.roadmap.bitmap & maskRoad;
			if ( maskedMap == pred4Way )
				instantiatePart( "Tiles/road_marker_4way", e.origin, .0f, markerOffsetY, false );
			if ( maskedMap == predStraight )
				instantiatePart( "Tiles/road_marker_straight_n", e.origin, .0f, markerOffsetY, false );
			else if ( maskedMap == util::cycleLeft( predStraight, 2 ) )
				instantiatePart( "Tiles/road_marker_straight_n", e.origin, 90.0f, markerOffsetY, false );
			else for ( auto d=0; d<8; d+=2 ) {
				if ( maskedMap == util::cycleLeft( pred3Way, d ) ) {
					instantiatePart( "Tiles/road_marker_3way_n", e.origin, 45.0f*d, markerOffsetY, false );
					break;
				}
				if ( maskedMap == util::cycleLeft( predBend, d ) ) {
					instantiatePart( "Tiles/road_marker_turn_ne", e.origin, 45.0f*d, markerOffsetY, false );
					break;
				}
				if ( maskedMap == util::cycleLeft(   predDeadend, d ) ) {
					instantiatePart( "Tiles/road_marker_deadend_n", e.origin, 45.0f*d, markerOffsetY, false );
					break;
				}
			}
			instantiatePart( "Tiles/asphalt", e.origin );
		}
	}
	return models;
}
