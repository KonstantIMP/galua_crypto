#pragma once

#include "utils/trait.hpp"
#include <math/num.hpp>
#include <math/operation.hpp>
#include <math/set.hpp>

#include <memory>
#include <optional>
#include <stdexcept>

namespace kimp::math {

template <typename T>
class TAlgebraicStructure;
template <typename T>
using TAlgebraicStructurePtr = std::shared_ptr<TAlgebraicStructure<T>>;

template <typename T>
class TAlgebraicStructure {
public:
    TAlgebraicStructure(const ISetPtr<T> set, const IMathOperationPtr<T> operation, T zero = 0, T one = 1)
        : Set_{set}
        , Operation_{operation}
        , Zero_{zero}
        , One_{one}
    {
        if (!Operation_->IsAllDeterminedFor(Set_)) {
            throw std::invalid_argument("Operation for algebraic structure should be all-determined for given set");
        }
        if (!Operation_->IsUnambiguousFor(Set_)) {
            throw std::invalid_argument("Operation for algebraic structure should be unambiguous for given set");
        }
        if (!Operation_->IsClosedFor(Set_)) {
            throw std::invalid_argument("Operation for algebraic structure should be closed for given set");
        }
    }

    auto IsAssociative() const -> bool {
        if (IsAdditive() || IsMultiplicative()) {
            return true;
        }
        throw std::invalid_argument("Behaviour is undefined for used operation type");
    }

    auto IsCommutative() const -> bool {
        if (IsAdditive() || IsMultiplicative()) {
            return true;
        }
        throw std::invalid_argument("Behaviour is undefined for used operation type");
    }

    auto HasNeutralElement() const -> bool {
        return GetNeutralElementIfExists().has_value();
    }

    auto GetNeutralElement() const -> T {
        if (!HasNeutralElement()) {
            throw std::invalid_argument("This structure doesn't have a neutral element");
        }
        return GetNeutralElementIfExists().value();
    }

    auto IsFullyReversible() const -> bool {
        if (!HasNeutralElement()) {
            return false;
        }

        if (TIntegerSetPtr iSet = std::dynamic_pointer_cast<TIntegerSet>(Set_); iSet) {
            if (IsAdditive() || IsMultiplicative()) {
                return true;
            }
            throw std::invalid_argument("Behaviour is undefined for used operation type");
        }

        if (TStaticSetPtr<T> sSet = std::dynamic_pointer_cast<TStaticSet<T>>(Set_); sSet) {
            /*if (IsAdditive()) {
                for (auto e : sSet->GetElements()) {
                    if (!sSet->contains(-e)) {
                        return false;
                    }
                }
                return true;
            }
            throw std::invalid_argument("Behaviour is undefined for used operation type");
            */
            return true;
        }
        
        throw std::invalid_argument("Behaviour is undefined for used set type");
    }

    auto IsAdditive() const -> bool {
        return (std::dynamic_pointer_cast<TSumOperation<T>>(Operation_) ? true : false) ||
            (std::dynamic_pointer_cast<TGaluaSumOperation<i64>>(Operation_) ? true : false);
    }

    auto IsMultiplicative() const -> bool {
        return (std::dynamic_pointer_cast<TMulOperation<T>>(Operation_) ? true : false) ||
            (std::dynamic_pointer_cast<TGaluaMulOperation<i64>>(Operation_) ? true : false);
    }

private:

    auto GetNeutralElementIfExists() const -> std::optional<T> {
        if (IsAdditive()) {
            return Zero_;
        }
        if (IsMultiplicative()) {
            return One_;
        }
        throw std::invalid_argument("Behaviour is undefined for used operation type");
    }

private:
    const ISetPtr<T> Set_;
    const IMathOperationPtr<T> Operation_;

    const T Zero_;
    const T One_;
};

} // namespace kimp::math
