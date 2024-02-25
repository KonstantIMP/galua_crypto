#pragma once

#include <cmath>
#include <iostream>
#include <math/num.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace kimp::math {

template <typename T> requires isNumeric<T>
class TPolynomial;
template <typename T> requires isNumeric<T>
using TPolynomialPtr = std::shared_ptr<TPolynomial<T>>;

template <typename T> requires isNumeric<T>
class TPolynomial {
public:
    TPolynomial(std::initializer_list<T> values) : Coefficients_{values} {
        if (values.size() == 0) {
            throw std::invalid_argument("Use TPolynomial::zero to get a polynomial that equals to 0");
        }

        if (values.size() != 1 && *values.begin() == 0) {
            throw std::invalid_argument("It's incorrect to have zero coefficient up to max polynomial degree");
        }
    }

    TPolynomial(const std::vector<T>& coefficients) : Coefficients_{coefficients} {
        if (coefficients.size() == 0) {
            throw std::invalid_argument("Use TPolynomial::zero to get a polynomial that equals to 0");
        }
    }

    TPolynomial(const TPolynomial<T>& p) : Coefficients_{p.Coefficients_} {}

    auto Degree() const -> std::size_t {
        return Coefficients_.size() - 1;
    }

    auto Pow(ui64 p) const -> const TPolynomial<T> {
        if (p == 0) {
            return TPolynomial<T> {1};
        }
        
        TPolynomial<T> newPolynomial {*this};
        for (std::size_t i{1}; i < p; i++) {
            newPolynomial = newPolynomial * (*this);
        }

        return newPolynomial;
    }

    auto Calc(T x) const -> T {
        T result = 0;

        for (std::size_t i {0}; i <= Degree(); i++) {
            result += (operator[](i) * std::pow(x, i));
        }

        return result;
    }

    auto operator[](const std::size_t degree) const -> const T {
        if (degree > this->Degree()) {
            throw std::out_of_range(
                fmt::format(
                    "The polynomial's degree is {}, requested {} coefficient"
                    , this->Degree()
                    , degree
                )
            );
        }

        return Coefficients_.at(this->Degree() - degree);
    }

    template <typename U> requires isNumeric<U>
    auto operator+(const TPolynomial<U>& p) const -> TPolynomial<std::common_type_t<T, U>> {
        auto resultDegree = std::max(this->Degree(), p.Degree());

        std::vector<std::common_type_t<T, U>> newPolynomial;
        newPolynomial.reserve(resultDegree);

        for (std::size_t i {0}; i <= resultDegree; i++) {
            newPolynomial.push_back(this->getCoefficientOrZero(resultDegree - i) + p.getCoefficientOrZero(resultDegree - i));
        }

        return TPolynomial<std::common_type_t<T, U>> {newPolynomial};
    }
    
    template <typename U> requires isNumeric<U>
    auto operator-(const TPolynomial<U>& p) const -> TPolynomial<std::common_type_t<T, U>> {
        auto resultDegree = std::max(this->Degree(), p.Degree());

        std::vector<std::common_type_t<T, U>> newPolynomial;
        newPolynomial.reserve(resultDegree);

        for (std::size_t i {0}; i <= resultDegree; i++) {
            auto tempCoef = this->getCoefficientOrZero(resultDegree - i) - p.getCoefficientOrZero(resultDegree - i);

            if (newPolynomial.size() || tempCoef != 0) {
                newPolynomial.push_back(tempCoef);
            }
        }

        if (newPolynomial.size() == 0) return TPolynomial<std::common_type_t<T, U>>::template zero<std::common_type_t<T, U>>();
        return TPolynomial<std::common_type_t<T, U>> {newPolynomial};
    }
    
    template <typename U> requires isNumeric<U>
    auto operator*(const TPolynomial<U>& p) const -> TPolynomial<std::common_type_t<T, U>> {
        if (this->isZero() || p.isZero()) {
            return TPolynomial<std::common_type_t<T, U>>::template zero<std::common_type_t<T, U>>();
        }

        auto resultDegree = this->Degree() + p.Degree();
        std::vector<std::common_type_t<T, U>> newPolynomial(resultDegree + 1);

        for (std::size_t i {0}; i <= this->Degree(); i++) {
            for (std::size_t j {0}; j <= p.Degree(); j++) {
                auto currentCoef = i + j;
                newPolynomial[resultDegree - currentCoef] += ((*this)[i] * p[j]);
            }
        }
        
        return TPolynomial<std::common_type_t<T, U>> {newPolynomial};
    }

    template <typename U> requires isIntegral<U>
    auto operator%(const TPolynomial<U>& p) const -> TPolynomial<std::common_type_t<T, U>> {
        if (p.Degree() > this->Degree()) {
            return *this;
        }
        
        TPolynomial<std::common_type_t<T, U>> newPolynomial {*this};

        for (std::size_t _d {0}; _d < Degree() + 1 - p.Degree(); _d++) { std::size_t d{Degree() - _d};
            if (d > newPolynomial.Degree()) {
                continue;
            }

            auto degreeDiff = d - p.Degree();
            auto coef = newPolynomial[d] / p[p.Degree()];

            auto upPolinomial = TPolynomial<std::common_type_t<T, U>>::template one<std::common_type_t<T, U>>().Pow(degreeDiff);
            newPolynomial = newPolynomial - (p * upPolinomial * coef);
        }

        return newPolynomial;
    }
    
    template <typename U> requires isNumeric<U>
    auto operator==(const TPolynomial<U>& p) const -> bool {
        if (this->Degree() != p.Degree()) {
            return false;
        }

        for (std::size_t i {0}; i <= p.Degree(); i++) {
            if (this->operator[](i) != p[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename U> requires isNumeric<U>
    auto operator+(const U n) const -> TPolynomial<std::common_type<T, U>> {
        return *this + TPolynomial<U> {n};
    }

    template <typename U> requires isNumeric<U>
    auto operator-(const U n) const -> TPolynomial<std::common_type<T, U>> {
        return *this - TPolynomial<U> {n};
    }

    auto operator*(const T n) const -> TPolynomial<T> {
        return *this * TPolynomial<T> {n};
    }

    auto operator%(const T n) const -> TPolynomial<T> {
        std::vector<T> newPolynomial;
        newPolynomial.reserve(Coefficients_.size());
        
        for (std::size_t i {0}; i < Coefficients_.size(); i++) {
            T currentValue = ((Coefficients_[i] % n) + n) % n;
            if (currentValue || newPolynomial.size()) {
                newPolynomial.push_back(currentValue);
            }
        }

        if (newPolynomial.size() == 0) return TPolynomial<T>::zero<T>();
        return TPolynomial<T> {newPolynomial};
    }

    auto isZero() const -> bool {
        return this->Degree() == 0 && (*this)[0] == 0;
    }

    auto ToString() const -> std::string {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& out, const TPolynomial<T>& p) {
        out << '(';

        for (std::size_t i {0}; i <= p.Degree(); i++) {
            out << p.Coefficients_[i];
            if (i != p.Degree()) {
                out << ", ";
            }
        }

        return out << ')';
    }

public:
    template <typename U = i32> requires isNumeric<U>
    static auto zero() -> TPolynomial<U> {
        return TPolynomial<U> {0};
    }

    template <typename U = i32> requires isNumeric<U>
    static auto one() -> TPolynomial<U> {
        return TPolynomial<U> {1, 0};
    }

private:
    auto getCoefficientOrZero(const std::size_t degree) const -> const T {
        if (degree > this->Degree()) {
            return 0;
        }
        return (*this)[degree];
    }

private:
    std::vector<T> Coefficients_;
};

} // namespace kimp::math
