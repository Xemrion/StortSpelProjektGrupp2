#pragma once

#include <type_traits>
#include "defs.hpp"

namespace util {
   // constexpr integral power function for computing
   // (main use: base as 2 to compute bit values at compile-time)
   template <class Integer>
   inline Integer constexpr  pow(Integer base, Integer exp) noexcept
	{
	   static_assert( std::is_integral_v<Integer>, "Template arg Integer must be integral!" );
	   Integer  retval(1);
	   while ( exp --> 0 )
		   retval *= base;
	   return retval;
   }

   template <class T_Vector>
   T_Vector  blend( T_Vector const &a, T_Vector const &b, F32 factor )
	{
      // TODO: clamp factor?
      return   a * factor   +   b * (1.0f-factor);
   }

   template <class T>
   inline T constexpr  minValue( T const &a, T const &b ) noexcept
	{
      return a < b ? a : b;
   }

   template <class T>
   inline T constexpr  maxValue( T const &a, T const &b ) noexcept
	{
      return a > b ? a : b;
   }

	// |x|
   template <class T>
   inline T constexpr  abs( T const &x ) noexcept
	{
      return x < T(0) ? -x : x;
   }

	// Bitwise sizeof.
	template <class T>
	Size constexpr nBits( T const & ) noexcept
	{
		return sizeof(T) * 8;
	}

	// Cyclic >> implementation.
	template <class Integer>
	Integer constexpr cycleLeft( Integer x, Size n ) noexcept
	{
		static_assert( std::is_integral_v<Integer>, "Template arg Integer must be integral!" );
		n %= nBits(x);
		return (x<<n) + (x>>(nBits(x)-n));
	}

	// Cyclic << implementation.
	template <class Integer>
	Integer constexpr cycleRight( Integer x, Size n ) noexcept
	{
		static_assert( std::is_integral_v<Integer>, "Template arg Integer must be integral!" );
		n %= nBits(x);
		return (x>>n) + (x<<(nBits(x)-n));
	}

	template <class Numeral>
	Numeral constexpr degToRad( Numeral deg ) noexcept {
		return 3.1415926535f / 180.0f * deg;
	}

	template <class Container>//, class T_RNG>
	auto &randomElementOf( Container &c, RNG &rng ) noexcept {
		auto const idx = U32_Dist{}( rng ) % c.size();
		return c[idx];
	}

	template <class Enum>
	auto constexpr randomEnumOf( RNG &rng ) noexcept {
	  return (Enum)( U32_Dist{}(rng) % (size_t)Enum::size );
	}

	template <class Container, class T>
	Bool isElementOf( Container const &c, T const &e ) noexcept {
		return std::find( c.begin(), c.end(), e) != c.end();
	}

	RGBA constexpr blendColor( RGBA a, RGBA b, F32 fac ) noexcept {
		return (RGBA(((a & 0x00'00'00'FF) >>  0) * fac) + RGBA(((b & 0x00'00'00'FF) >>  0) * (1.0f - fac)) <<  0)+ // R
             (RGBA(((a & 0x00'00'FF'00) >>  8) * fac) + RGBA(((b & 0x00'00'FF'00) >>  8) * (1.0f - fac)) <<  8)+ // G
             (RGBA(((a & 0x00'FF'00'00) >> 16) * fac) + RGBA(((b & 0x00'FF'00'00) >> 16) * (1.0f - fac)) << 16)+ // B
             (RGBA(((a & 0xFF'00'00'00) >> 24) * fac) + RGBA(((b & 0xFF'00'00'00) >> 24) * (1.0f - fac)) << 24); // A
	}
}
