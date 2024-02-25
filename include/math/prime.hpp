#pragma once

#include <math/num.hpp>

#include <cmath>

namespace kimp::math {

template <typename T> requires isUnsignedIntegral<T>
auto isPrime(T t) -> bool {
    for (T i {2}; i <= std::sqrt(t); i++) {
        if (t % i == 0) {
            return false;
        }
    }
    return true;
}

} // namespace kimp::math
