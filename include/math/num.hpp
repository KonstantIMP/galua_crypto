#pragma once

#include <cstdint>
#include <type_traits>

namespace kimp::math {

typedef std::int32_t i32;
typedef std::int64_t i64;

typedef std::uint32_t ui32;
typedef std::uint64_t ui64;

template <typename T>
concept isSignedIntegral = std::is_same_v<T, i32> || std::is_same_v<T, i64>;

template <typename T>
concept isUnsignedIntegral = std::is_same_v<T, ui32> || std::is_same_v<T, ui64>;

template <typename T>
concept isIntegral = isSignedIntegral<T> || isUnsignedIntegral<T>;

template <typename T>
concept isFloatingPoint = std::is_same_v<T, float> || std::is_same_v<T, double>;

template <typename T>
concept isNumeric = isIntegral<T> || isFloatingPoint<T>;

} // namespace kimp::math

using i32 = kimp::math::i32;
using i64 = kimp::math::i64;
using ui32 = kimp::math::ui32;
using ui64 = kimp::math::ui64;
