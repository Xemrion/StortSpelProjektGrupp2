#include "District.hpp"

F32 get_building_density( District const d ) noexcept {
   F32 result;
   if      ( d == District::residential  ) { result = .6f; }
   else if ( d == District::park         ) { result = .0f; }
   else if ( d == District::metropolitan ) { result = .9f; }
   else if ( d == District::suburban     ) { result = .1f; }
   else if ( d == District::downtown     ) { result = .4f; }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}

U8 get_building_min_area( District const d ) noexcept {
   U8 result;
   if      ( d == District::residential  ) { result = 8; }
   else if ( d == District::park         ) { result = 0; }
   else if ( d == District::metropolitan ) { result = 3; }
   else if ( d == District::suburban     ) { result = 1; }
   else if ( d == District::downtown     ) { result = 4; }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}

U8 get_building_max_area( District const d ) noexcept {
   U8 result;
   if      ( d == District::residential  ) { result = 16; }
   else if ( d == District::park         ) { result = 0; }
   else if ( d == District::metropolitan ) { result = 6; }
   else if ( d == District::suburban     ) { result = 1; }
   else if ( d == District::downtown     ) { result = 8; }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}

U8 getMinFloorCount( District const d ) noexcept {
   U8 result;
   if      ( d == District::residential  ) { result = 4;  }
   else if ( d == District::park         ) { result = 0;  }
   else if ( d == District::metropolitan ) { result = 16; }
   else if ( d == District::suburban     ) { result = 1;  }
   else if ( d == District::downtown     ) { result = 3;  }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}


U8 getMaxFloorCount( District const d ) noexcept {
   U8 result;
   if      ( d == District::residential  ) { result = 6;  }
   else if ( d == District::park         ) { result = 0;  }
   else if ( d == District::metropolitan ) { result = 32; }
   else if ( d == District::suburban     ) { result = 2;  }
   else if ( d == District::downtown     ) { result = 4;  }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}

Str stringify( District const d ) noexcept {
   Str result;
   if      ( d == District::residential  ) { result = "residential";  }
   else if ( d == District::park         ) { result = "park";         }
   else if ( d == District::metropolitan ) { result = "metropolitan"; }
   else if ( d == District::suburban     ) { result = "suburban";     }
   else if ( d == District::downtown     ) { result = "downtown";     }
   else assert( false && "Unaccounted for enum value!" );
   return result;
}
