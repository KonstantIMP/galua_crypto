#pragma once

#include <math/deduction.hpp>
#include <math/field.hpp>
#include <math/num.hpp>
#include <math/polynomial.hpp>

namespace kimp {

using namespace kimp::math;

class TCryptoApp {
public:
    TCryptoApp();

    auto run(int argc, char ** argv) -> int;

private:
    enum class EAppMode {
        GaluaAppMode
        , CipherAppMode
        , NoAppMode
    };

private:
    auto ParseCommandLineArguments(int argc, char ** argv) -> EAppMode;

private:
    auto RunGaluaMode() const -> int;

    auto BuildSimpleEndlessField() const -> TFieldPtr<TDeductionClass<ui64>>;

    auto GeneratePolynomialForGaluaField(const TFieldPtr<TDeductionClass<ui64>>& simpleEndlessFied) const -> TPolynomialPtr<i64>;
    auto EnterPolynomialForGaluaField() const -> TPolynomialPtr<i64>;

    auto IsPolynomialSuitableForGaluaFieldBuilding(const TFieldPtr<TDeductionClass<ui64>>& simpleEndlessFied, const TPolynomialPtr<i64>& p) const -> bool;

    auto BuildGaluaField(const TPolynomialPtr<i64>& p) const -> TGaluaFieldPtr;
    auto ExploreMultiplicativeGroup(const TGaluaFieldPtr&) const -> void;

    auto RunCipherMode() const -> int;

private:
    ui64 GaluaN_;
    ui64 GaluaP_;
    bool GaluaAutogen_;

    std::string CipherValue_;
    bool CipherIsEncoding_;
};

} // namespace kimp

