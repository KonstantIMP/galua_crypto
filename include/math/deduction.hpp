#pragma once

#include <math/abs.hpp>
#include <math/num.hpp>

#include <memory>
#include <ostream>
#include <stdexcept>

#include <fmt/format.h>

namespace kimp::math {

template <typename T> requires isUnsignedIntegral<T> 
class TDeductionClass;
template <typename T> requires isUnsignedIntegral<T>
using TDeductionClassPtr = std::shared_ptr<TDeductionClass<T>>;

template <typename T> requires isUnsignedIntegral<T>
class TDeductionClass {
public:
    constexpr TDeductionClass(T a, T n)
        : A_{a}
        , N_{n}
    {
        if (a >= n) {
            throw std::invalid_argument("A cannot be more than N");
        }
    }

    TDeductionClass(const TDeductionClass<T>& dc)
        : A_{dc.A_}
        , N_{dc.N_}
        {}

    auto GetA() const -> T {
        return A_;
    }

    auto GetN() const -> T {
        return N_;
    }

    template <typename U> requires isIntegral<U>
    auto isIn(U v) const -> bool {
        if (A_ == 0) {
            return abs(v) % N_ == 0;
        }
        return (((v % N_) + N_) % N_) == A_;
    }

    auto operator+(const TDeductionClass<T>& dc) const -> TDeductionClass<T> {
        return TDeductionClass<T> {(this->A_ + dc.A_) % this->N_, this->N_};
    }

    auto operator*(const TDeductionClass<T>& dc) const -> TDeductionClass<T> {
        return TDeductionClass<T> {(this->A_ * dc.A_) % this->N_, this->N_};
    }

    auto operator==(const TDeductionClass<T>& dc) const -> bool {
        return (this->N_ == dc.N_) && (this->A_ == dc.A_);
    }

    auto operator-() const -> TDeductionClass<T> {
        return TDeductionClass<T> {(this->N_ - this->A_) % this->N_, this->N_};
    }

    friend std::ostream& operator<<(std::ostream& out, const TDeductionClass<T>& dc) {
        return out << fmt::format("({} from {})", dc.A_, dc.N_);
    }

private:
    const T A_;
    const T N_;
};

} // namespace kimp::math
