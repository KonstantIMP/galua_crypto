#pragma once

#include <math/num.hpp>

#include <iostream>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <vector>
#include <unordered_set>

namespace kimp::math {

template <typename T>
class ISet;
template <typename T>
using ISetPtr = std::shared_ptr<ISet<T>>; 

template <typename T>
class ISet {
public:
    virtual auto contains(const T&) const -> bool = 0;

    virtual ~ISet() {};

public:
    friend std::ostream& operator<<(std::ostream& out, const ISetPtr<T>& p) {
        p->PrintTo(out);
        return out;
    }

private:
    virtual auto PrintTo(std::ostream&) const -> void = 0;
};

template <typename T>
class TStaticSet;
template <typename T>
using TStaticSetPtr = std::shared_ptr<TStaticSet<T>>;

template <typename T>
class TStaticSet : public ISet<T> {
public:
    TStaticSet(std::initializer_list<T> elems) : Elements_(elems) {}

    TStaticSet(const std::vector<T>& elems) : Elements_(elems) {}

    virtual bool contains(const T& e) const override {
        for (auto el : Elements_) {
            if (el == e) {
                return true;
            }
        }
        return false;
    }

    auto GetElements() const -> const std::vector<T>& {
        return Elements_;
    }

    virtual ~TStaticSet() {}

private:
    virtual void PrintTo(std::ostream& out) const override {
        out << '{';

        for (std::size_t i {0}; i < Elements_.size(); i++) {
            out << Elements_[i];
            if (i + 1 != Elements_.size()) {
                out << ", ";
            }
        }

        out << '}';
    }

private:
    const std::vector<T> Elements_;
};

class TIntegerSet;
using TIntegerSetPtr = std::shared_ptr<TIntegerSet>;

class TIntegerSet : public ISet<i64> {
public:
    TIntegerSet() {}

    virtual bool contains(const i64& e) const override {
        return true;
    }

    template <typename U> requires isFloatingPoint<U>
    auto contains(U e) const -> bool {
        return false;
    }

    virtual ~TIntegerSet() {}

private:
    virtual void PrintTo(std::ostream& out) const override {
        out << 'Z';
    }
};

} // namespace kimp::math
