#pragma once

#include <math/num.hpp>

#include <tuple>

namespace kimp::math {

template <typename T1, typename T2> requires isIntegral<T1> && isIntegral<T2>
auto gcd(T1 a, T2 b) -> std::common_type_t<T1, T2> {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

template <typename T1, typename T2> requires isSignedIntegral<T1> && isSignedIntegral<T2>
auto gcdExtended(T1 a, T2 b) -> std::tuple<std::common_type_t<T1, T2>, i64, i64> {
    if (a == 0) {
        return std::make_tuple(b, 0, 1);
    }

    auto intermediateResult = gcdExtended(b % a, a);
    return std::make_tuple(
      std::get<0>(intermediateResult)
      , std::get<2>(intermediateResult) - (b / a) * std::get<1>(intermediateResult)
      , std::get<1>(intermediateResult)
    );
}

} // namespace kimp::math
