#include "MinimapTextureGenerator.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../STB/stb_image_write.h"
#include "MapConfig.hpp"

Str createMinimapTexture( Map const &map ) {
   Size const  WIDTH  = map.width,
               HEIGHT = map.height;
   RGBA       *pixels = new RGBA[WIDTH * HEIGHT];

   for ( U16 y = 0;  y < WIDTH;  ++y )
      for ( U16 x = 0;  x < HEIGHT;  ++x )
         pixels[y * WIDTH + x] = minimapColorLookUpTable( map.getTileAt(x,y) );

   auto path = Str("/data/map/") + mapConfigToFilename( map.config, ".tga" );
   stbi_write_tga( path.c_str(), static_cast<I32>(WIDTH), static_cast<I32>(HEIGHT), 3, reinterpret_cast<F32*>(pixels) );
   return path;   
}
