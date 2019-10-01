#pragma once

// author: Victor Falkengaard Itzel
// copyright September 2019

#include <random>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cstdint>
// using aliases

// collections
template <typename T>           using Vec = std::vector<T>;
template <typename T, size_t N> using Arr = std::array<T,N>;

// smart pointers
template <typename T> using UPtr = std::unique_ptr<T>;
template <typename T> using SPtr = std::shared_ptr<T>;

// string
using Str      = std::basic_string<char>;

// primitives
using U8   = std::uint_fast8_t;
using U16  = std::uint_fast16_t;
using U32  = std::uint_fast32_t;
using U64  = std::uint_fast64_t;
using I8   = std::int_fast8_t;
using I16  = std::int_fast16_t;
using I32  = std::int_fast32_t;
using I64  = std::int_fast64_t;
using F32  = float;
using F64  = double;
using Char = char;
using Bool = bool;
using Size = size_t;
using Void = void;

// vectors (TODO: padding alignment if sizeof(T)%4!=0?)
template <typename T> union V2 {
	T  data[2];
	struct { T  x, y; }; // anon
	struct { T  r, g; }; // anon
};
using V2f = V2<F32>;
using V2u = V2<U32>;
using V2i = V2<I32>;

template <typename T> union V3 {
	T  data[3];
	struct { T  x, y, z; }; // anon
	struct { T  r, g, b; }; // anon
};
using V3f = V3<F32>;
using V3u = V3<U32>;
using V3i = V3<I32>;

template <typename T> union V4 {
	T  data[4];
	struct { T  x, y, z, w; }; // anon
	struct { T  r, g, b, a; }; // anon
};
using V4f = V4<F32>;
using V4u = V4<U32>;
using V4i = V4<I32>;

// random number generation
using RD       = std::random_device;
using RNG      = std::mt19937; // Mersienne Twister random number generator engine
using U8_Dist  = std::uniform_int_distribution<U8>;
using U16_Dist = std::uniform_int_distribution<U16>;
using U32_Dist = std::uniform_int_distribution<U32>;
using U64_Dist = std::uniform_int_distribution<U64>;
using I8_Dist  = std::uniform_int_distribution<I8>;
using I16_Dist = std::uniform_int_distribution<I16>;
using I32_Dist = std::uniform_int_distribution<I32>;
using I64_Dist = std::uniform_int_distribution<I64>;
using F32_Dist = std::uniform_real_distribution<F32>;
using F64_Dist = std::uniform_real_distribution<F64>;

// TODO: streams?