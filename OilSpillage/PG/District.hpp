#pragma once

#include <cassert>
#include "defs.hpp"

enum class District : U8 { residential, park, metropolitan, suburban, downtown, /* <- insert here */ size };

F32 get_building_density(  District const d ) noexcept;
U8  get_building_min_area( District const d ) noexcept;
U8  get_building_max_area( District const d ) noexcept;
U8  get_min_height(        District const d ) noexcept;
U8  get_max_height(        District const d ) noexcept;
Str stringify(             District const d ) noexcept;
