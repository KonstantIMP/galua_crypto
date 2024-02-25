#pragma once

#include "math/polynomial.hpp"
#include <math/deduction.hpp>
#include <math/num.hpp>
#include <math/set.hpp>
#include <utils/trait.hpp>

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace kimp::math {

template <typename T>
class IMathOperation;
template <typename T>
using IMathOperationPtr = std::shared_ptr<IMathOperation<T>>;

template <typename T>
class IMathOperation {
public:
    virtual auto Apply(const T&, const T&) const -> T = 0;

    virtual auto IsAllDeterminedFor(const ISetPtr<T>&) const -> bool = 0;
    virtual auto IsUnambiguousFor(const ISetPtr<T>&) const -> bool = 0;
    virtual auto IsClosedFor(const ISetPtr<T>&) const -> bool = 0;

    virtual ~IMathOperation() {}

protected:
    auto IsClosedForStaticSetFullCheck(const TStaticSetPtr<T>& sSet) const -> bool {
        auto setElements = sSet->GetElements();

        for (auto a : setElements) {
            for (auto b : setElements) {
                if (!sSet->contains(this->Apply(a, b))) {
                    return false;
                }
            }
        }
        return true;
    }
};

template <typename T>
class TSumOperation;
template <typename T>
using TSumOperationPtr = std::shared_ptr<TSumOperation<T>>;

template <typename T>
class TSumOperation : public IMathOperation<T> {
public:
    TSumOperation() {}

    virtual T Apply(const T& a, const T&b) const override {
        return a + b;
    }

    virtual bool IsAllDeterminedFor(const ISetPtr<T>&) const override {
        return true;
    }

    virtual bool IsUnambiguousFor(const ISetPtr<T>&) const override {
        return true;
    }

    virtual bool IsClosedFor(const ISetPtr<T>& set) const override {
        if (TIntegerSetPtr iSet = std::dynamic_pointer_cast<TIntegerSet>(set); iSet) {
            return true;
        }
        if (TStaticSetPtr<T> sSet = std::dynamic_pointer_cast<TStaticSet<T>>(set); sSet) {
            return this->IsClosedForStaticSetFullCheck(sSet);
        }
        throw std::invalid_argument("Behaviour is undefined for given set type");
    }

    virtual ~TSumOperation() {}
};

template <typename T = ui64>
class TGaluaSumOperation;
template <typename T = ui64>
using TGaluaSumOperationPtr = std::shared_ptr<TGaluaSumOperation<T>>;

template <typename T>
class TGaluaSumOperation : public IMathOperation<TPolynomial<T>> {
public:
    TGaluaSumOperation(T n) : N_{n} {
        if (N_ == 0) {
            throw std::invalid_argument("Unable to sum polynomials with mod by zero");
        }
    }

    virtual TPolynomial<T> Apply(const TPolynomial<T>& a, const TPolynomial<T>& b) const override {
        return (a + b) % N_;
    }

    virtual bool IsAllDeterminedFor(const ISetPtr<TPolynomial<T>>&) const override {
        return true;
    }

    virtual bool IsUnambiguousFor(const ISetPtr<TPolynomial<T>>&) const override {
        return true;
    }

    virtual bool IsClosedFor(const ISetPtr<TPolynomial<T>>& set) const override {
        if (TStaticSetPtr<TPolynomial<T>> sSet = std::dynamic_pointer_cast<TStaticSet<TPolynomial<T>>>(set); sSet) {
            return this->IsClosedForStaticSetFullCheck(sSet);
        }
        throw std::invalid_argument("Behaviour is undefined for given set type");
    }

    virtual ~TGaluaSumOperation() {}

private:
    const T N_;
};

template <typename T>
class TMulOperation;
template <typename T>
using TMulOperationPtr = std::shared_ptr<TMulOperation<T>>;

template <typename T>
class TMulOperation : public IMathOperation<T> {
public:
    TMulOperation() {}

    virtual T Apply(const T& a, const T&b) const override {
        return a * b;
    }

    virtual bool IsAllDeterminedFor(const ISetPtr<T>&) const override {
        return true;
    }

    virtual bool IsUnambiguousFor(const ISetPtr<T>&) const override {
        return true;
    }

    virtual bool IsClosedFor(const ISetPtr<T>& set) const override {
        if (TIntegerSetPtr iSet = std::dynamic_pointer_cast<TIntegerSet>(set); iSet) {
            return true;
        }
        if (TStaticSetPtr<T> sSet = std::dynamic_pointer_cast<TStaticSet<T>>(set); sSet) {
            return this->IsClosedForStaticSetFullCheck(sSet);
        }
        throw std::invalid_argument("Behaviour is undefined for given set type");
    }

    virtual ~TMulOperation() {}
};

template <typename T = ui64>
class TGaluaMulOperation;
template <typename T = ui64>
using TGaluaMulOperationPtr = std::shared_ptr<TGaluaMulOperation<T>>;

template <typename T>
class TGaluaMulOperation : public IMathOperation<TPolynomial<T>> {
public:
    TGaluaMulOperation(const TPolynomialPtr<i64>& p, T n) : P_{p}, N_{n} {
        if (N_ == 0) {
            throw std::invalid_argument("Unable to sum polynomials with mod by zero");
        }
    }

    virtual TPolynomial<T> Apply(const TPolynomial<T>& a, const TPolynomial<T>& b) const override {
        return (((a * b) * (N_ + 1)) % *P_) % N_;
    }

    virtual bool IsAllDeterminedFor(const ISetPtr<TPolynomial<T>>&) const override {
        return true;
    }

    virtual bool IsUnambiguousFor(const ISetPtr<TPolynomial<T>>&) const override {
        return true;
    }

    virtual bool IsClosedFor(const ISetPtr<TPolynomial<T>>& set) const override {
        if (TStaticSetPtr<TPolynomial<T>> sSet = std::dynamic_pointer_cast<TStaticSet<TPolynomial<T>>>(set); sSet) {
            return this->IsClosedForStaticSetFullCheck(sSet);
        }
        throw std::invalid_argument("Behaviour is undefined for given set type");
    }

    virtual ~TGaluaMulOperation() {}

private:
    const TPolynomialPtr<i64> P_;
    const T N_;
};

} // namespace kimp::math
