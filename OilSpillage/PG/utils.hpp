#pragma once

#include <type_traits>
#include "defs.hpp"

namespace util {
   // constexpr integral power function for computing
   // (main use: base as 2 to compute bit values at compile-time)
   template <typename T_Integer>
   inline T_Integer constexpr  pow(T_Integer base, T_Integer exp) noexcept {
	   static_assert(std::is_integral_v<T_Integer>, "Template arg Integer must be integral!");
	   T_Integer  result(1);
	   while ( exp --> 0 )
		   result *= base;
	   return result;
   }

   template <typename T_Vector>
   T_Vector  blend( T_Vector const &a, T_Vector const &b, F32 factor ) {
      // TODO: clamp factor?
      return   a * factor   +   b * (1.0f-factor);
   };

   template <class T>
   inline T constexpr  minValue( T const &a, T const &b ) noexcept {
      return (a < b) ? a : b;
   }

   template <class T>
   inline T constexpr  maxValue( T const &a, T const &b ) noexcept {
      return (a > b) ? a : b;
   }

   template <class T_Collection, class T_Predicate>
   inline void  erase_if( T_Collection &c, T_Predicate p ) noexcept {
      c.erase( std::remove_if( c.begin(), c.end(), p ),
               c.end());
   }

   template <class T_Collection, class T_Element>
   inline void  erase( T_Collection &c, T_Element &e ) noexcept {
      c.erase( std::remove( c.begin(), c.end(), e ),
               c.end());
   }
};
