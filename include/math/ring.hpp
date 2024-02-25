#pragma once

#include "math/operation.hpp"
#include <math/num.hpp>
#include <math/set.hpp>
#include <math/structure.hpp>

#include <memory>
#include <stdexcept>

namespace kimp::math {

template <typename T>
class TRing;
template <typename T>
using TRingPtr = std::shared_ptr<TRing<T>>;

template <typename T>
class TRing {
public:
    TRing(
        const ISetPtr<T> set
        , const TAlgebraicStructurePtr<T>& additive
        , const TAlgebraicStructurePtr<T>& multiplicative
    )
        : Set_{set}
        , Additive_{additive}
        , Multiplicative_{multiplicative}
    {
        if (!Additive_->IsCommutative()) {
            throw std::invalid_argument("Additive group is not commutative for given set");
        }
        if (!Additive_->HasNeutralElement()) {
            throw std::invalid_argument("Additive group for given set doesn't have a neutral element");
        }
        if (!Additive_->IsFullyReversible()) {
            throw std::invalid_argument("Additive group is not fully-reversible for given set");
        }
        if (!Additive_->IsAssociative()) {
            throw std::invalid_argument("Additive group is not associative for given set");
        }

        if (!Multiplicative_->IsAssociative()) {
            throw std::invalid_argument("Multiplicative group is not associative for given set");
        }
    }

    auto GetElements() const -> ISetPtr<T> {
        return Set_;
    }

private:
    const ISetPtr<T> Set_;
    const TAlgebraicStructurePtr<T> Additive_;
    const TAlgebraicStructurePtr<T> Multiplicative_;
};

} // namespace kimp::math
