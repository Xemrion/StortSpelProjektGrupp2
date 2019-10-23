#pragma once

#include <type_traits>
#include "defs.hpp"

namespace util {
   // constexpr integral power function for computing
   // (main use: base as 2 to compute bit values at compile-time)
   template <typename Integer> // TODO static_assert( is_integer_v<T_Integer> )
   inline Integer constexpr  pow(Integer base, Integer exp) noexcept {
	   static_assert(std::is_integral_v<Integer>, "Template arg Integer must be integral!");
	   Integer  retval(1);
	   while ( exp --> 0 )
		   retval *= base;
	   return retval;
   }

   template <typename T_Vector>
   T_Vector  blend( T_Vector const &a, T_Vector const &b, F32 factor ) {
      // TODO: clamp factor?
      return   a * factor   +   b * (1.0f-factor);
   };

   template <class T>
   inline T constexpr  minValue( T const &a, T const &b ) noexcept {
      return a < b ? a : b;
   }

   template <class T>
   inline T constexpr  maxValue( T const &a, T const &b ) noexcept {
      return a > b ? a : b;
   }
   template <class T>
   inline T constexpr  abs( T const &x ) noexcept {
      return x < T(0) ? -x : x;
   }

};
