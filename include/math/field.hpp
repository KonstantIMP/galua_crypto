#pragma once

#include "math/operation.hpp"
#include "math/polynomial.hpp"
#include "math/structure.hpp"
#include <math/set.hpp>
#include <math/deduction.hpp>
#include <math/num.hpp>
#include <math/prime.hpp>
#include <math/ring.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

namespace kimp::math {

template <typename T>
class TField;
template <typename T>
using TFieldPtr = std::shared_ptr<TField<T>>;

template <>
class TField<TDeductionClass<ui64>> {
public:
    TField(ui64 n) {
        if (!isPrime(n)) {
            throw std::invalid_argument(fmt::format("Unable to create field with non prime N (is {})", n));
        }

        std::vector<TDeductionClass<ui64>> deductionClasses;
        deductionClasses.reserve(n);

        for (ui64 i {0}; i < n; i++) {
            deductionClasses.push_back(TDeductionClass<ui64> {i, n});
        }

        auto deductionClassesSet = std::make_shared<TStaticSet<TDeductionClass<ui64>>>(deductionClasses);

        auto zero = TDeductionClass<ui64> {0, n};
        auto one = TDeductionClass<ui64> {1, n};

        DeductionClassesRing_ = TRingPtr<TDeductionClass<ui64>> {
            new TRing<TDeductionClass<ui64>> {
                deductionClassesSet,
                std::make_shared<TAlgebraicStructure<TDeductionClass<ui64>>>(
                    deductionClassesSet,
                    std::make_shared<TSumOperation<TDeductionClass<ui64>>>(),
                    zero,
                    one
                ),
                std::make_shared<TAlgebraicStructure<TDeductionClass<ui64>>>(
                    deductionClassesSet,
                    std::make_shared<TMulOperation<TDeductionClass<ui64>>>(),
                    zero,
                    one
                )
            }
        };
    }

    auto GetElements() const {
        return DeductionClassesRing_->GetElements();
    }

private:
    TRingPtr<TDeductionClass<ui64>> DeductionClassesRing_;
};

template <>
class TField<TPolynomial<i64>> {
public:
    TField(const TPolynomialPtr<i64>& base, ui64 p, ui64 n)
        : P_{p}
        , N_{n}
        , Base_{base}
        , SumOperation_{std::make_shared<TGaluaSumOperation<i64>>(p)}
        , MulOperation_{std::make_shared<TGaluaMulOperation<i64>>(base, p)}
    {
        auto polynomials = GeneratePolinomialsForRing();

        auto zero = TPolynomial<i64>::zero<i64>();
        auto one = TPolynomial<i64>::one<i64>();
    
        PolynomialsRing_ = std::make_shared<TRing<TPolynomial<i64>>>(
            polynomials,
            std::make_shared<TAlgebraicStructure<TPolynomial<i64>>>(
                polynomials,
                SumOperation_,
                zero,
                one
            ),
            std::make_shared<TAlgebraicStructure<TPolynomial<i64>>>(
                polynomials,
                MulOperation_,
                zero,
                one
            )
        );
    }

    auto GetElements() const -> std::vector<TPolynomial<i64>> {
        return std::dynamic_pointer_cast<TStaticSet<TPolynomial<i64>>>(PolynomialsRing_->GetElements())->GetElements();
    }

    auto GetSumOperation() const -> TGaluaSumOperationPtr<i64> {
        return SumOperation_;
    }

    auto GetMulOperation() const -> TGaluaMulOperationPtr<i64> {
        return MulOperation_;
    }

private:
    auto GeneratePolinomialsForRing() const -> TStaticSetPtr<TPolynomial<i64>> {
        std::vector<TPolynomial<i64>> elements;
        elements.reserve(std::pow(P_, N_));

        std::vector<i64> tempPolynomial (N_);

        const auto FillTempPolynomial = [&]() {
            for (std::size_t i {0}; i < tempPolynomial.size(); i++) {
                tempPolynomial[i] = P_ - 1;
            }
        };

        FillTempPolynomial();

        while (tempPolynomial.size() >= 1) {
            elements.push_back(TPolynomial<i64> {tempPolynomial});

            auto tempIndex = tempPolynomial.size() - 1;

            while (tempIndex > 0) {
                if (tempPolynomial[tempIndex] > 0) {
                    tempPolynomial[tempIndex]--;
                    break;
                }

                tempPolynomial[tempIndex] = P_ - 1;
                tempIndex--;
            }

            if (tempIndex == 0) {
                if (tempPolynomial[tempIndex] > 1) {
                    tempPolynomial[tempIndex]--;
                } else {
                    tempPolynomial.pop_back();
                    FillTempPolynomial();
                }
            }
        }
        elements.push_back(TPolynomial<i64>::zero<i64>());
        std::reverse(elements.begin(), elements.end());
        
        return std::make_shared<TStaticSet<TPolynomial<i64>>>(elements);
    }

private:
    const ui64 P_;
    const ui64 N_;
    const TPolynomialPtr<i64> Base_;

    const TGaluaSumOperationPtr<i64> SumOperation_;
    const TGaluaMulOperationPtr<i64> MulOperation_;

    TRingPtr<TPolynomial<i64>> PolynomialsRing_;
};

using TGaluaField = TField<TPolynomial<i64>>;
using TGaluaFieldPtr = TFieldPtr<TPolynomial<i64>>;

} // namespace kimp::math
