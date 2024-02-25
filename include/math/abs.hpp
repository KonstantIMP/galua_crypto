#pragma once

#include <math/num.hpp>

namespace kimp::math {

template <typename T> requires isNumeric<T>
auto abs(T v) -> T {
    if (v < 0) {
        return -v;
    }
    return v;
}

} // namespace kimp::math
