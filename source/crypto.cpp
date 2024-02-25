#include <cmath>
#include <crypto.hpp>

#include "math/deduction.hpp"
#include "math/field.hpp"
#include "math/set.hpp"
#include <math/polynomial.hpp>

#include <exception>
#include <iostream>

#include <argparse/argparse.hpp>
#include <fmt/format.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace kimp {

TCryptoApp::TCryptoApp() {}

auto TCryptoApp::run(int argc, char ** argv) -> int {
    switch (ParseCommandLineArguments(argc, argv)) {
        case EAppMode::GaluaAppMode:
            return RunGaluaMode();
        case EAppMode::CipherAppMode:
            return RunCipherMode();
        default:
            return -1;
    }
}

auto TCryptoApp::RunGaluaMode() const -> int {
    std::cout << "You are in the galua field building mode" << std::endl;
    std::cout << fmt::format("Will work with F (n = {}, p = {})", GaluaN_, GaluaP_) << std::endl;
    std::cout << std::endl;

    std::cout << "[Step 1] Generate a simple endless field" << std::endl;
    auto simpleEndlessFied = BuildSimpleEndlessField();
    if (!simpleEndlessFied) return -1;    
    std::cout << "[Step 1] Generation done, F(n) is " << simpleEndlessFied->GetElements() << std::endl;
    std::cout << std::endl;

    std::cout << "[Step 2] Getting a polynomial" << std::endl;
    TPolynomialPtr<i64> polynomial;
    if (GaluaAutogen_) {
        std::cout << "[Step 2] Performing automatically polynomial generation" << std::endl;
        polynomial = GeneratePolynomialForGaluaField(simpleEndlessFied);
    } else {
        std::cout << "[Step 2] You need to manually enter a polynomial..." << std::endl;
        polynomial = EnterPolynomialForGaluaField(); 
    }
    if (!polynomial) {
        std::cout << "[Step 2] There is no suitable polynomial" << std::endl;
        return -1;
    }
    std::cout << "[Step 2] Your polynomial is " << *polynomial << std::endl;
    std::cout << "[Step 2] Checking is it suitable for Galua field building..." << std::endl;
    if (!IsPolynomialSuitableForGaluaFieldBuilding(simpleEndlessFied, polynomial)) {
        std::cout << "[Step 2] Sorry, your polynomial is invalid, good bye" << std::endl;
        return -1;
    }
    std::cout << "[Step 2] Everything is ok, go next" << std::endl;
    std::cout << std::endl;

    std::cout << "[Step 3] Building Galua field..." << std::endl;
    TGaluaFieldPtr galuaField = BuildGaluaField(polynomial);

    std::cout << "[Step 4] Exploring of multiplicative group of the galua field" << std::endl;
    ExploreMultiplicativeGroup(galuaField);

    std::cout << std::endl << "[Step 5] Done!" << std::endl << std::flush;

    return 0;
}

auto TCryptoApp::BuildSimpleEndlessField() const -> TFieldPtr<TDeductionClass<ui64>> {
    try {
        return std::make_shared<TField<TDeductionClass<ui64>>>(GaluaP_);
    } catch (const std::exception& e) {
        std::cerr << fmt::format("Something got wrong: {}", e.what()) << std::endl;
        std::cerr << "Try to change some parametrs, good bye" << std::endl;
        return nullptr;
    }
}

auto TCryptoApp::GeneratePolynomialForGaluaField(
    const TFieldPtr<TDeductionClass<ui64>>& simpleEndlessFied
) const -> TPolynomialPtr<i64> {
    if (GaluaP_ == 3 && GaluaN_ == 2) {
        return std::make_shared<TPolynomial<i64>>(std::vector<i64> {2, 1, 1});
    }
    for (int d1 {1}; d1 < GaluaN_; d1++) {
        for (int k1 {1}; k1 < GaluaP_; k1++) {
            for (int k0 {1}; k0 < GaluaP_; k0++) {
                TPolynomial<i64> temp = TPolynomial<i64>::one<i64>().Pow(GaluaN_);
                temp = temp + (TPolynomial<i64>::one<i64>().Pow(d1) * k1);
                temp = temp + TPolynomial<i64> {k0};

                auto tempPtr = std::make_shared<TPolynomial<i64>>(temp);
                if (IsPolynomialSuitableForGaluaFieldBuilding(simpleEndlessFied, tempPtr)) {
                    return tempPtr;
                }
            }
        }
    }
    return nullptr;
}

auto TCryptoApp::EnterPolynomialForGaluaField() const -> TPolynomialPtr<i64> {
    std::vector<i64> coefficients;
    for (std::size_t i {0}; i <= GaluaN_; i++) {
        i64 temp;
        std::cout << "[Step 2] Enter " << GaluaN_ - i << "nt coefficient: ";
        std::cin >> temp;
        coefficients.push_back(temp);
    }
    return std::make_shared<TPolynomial<i64>>(coefficients);
}

auto TCryptoApp::IsPolynomialSuitableForGaluaFieldBuilding(
    const TFieldPtr<TDeductionClass<ui64>>& simpleEndlessFied
    , const TPolynomialPtr<i64>& p
) const -> bool {
    for (const auto& dc : std::dynamic_pointer_cast<TStaticSet<TDeductionClass<ui64>>>(simpleEndlessFied->GetElements())->GetElements()) {
        if (p->Calc(dc.GetA()) % GaluaP_ == 0) return false;
    }
    return true;
}

auto TCryptoApp::BuildGaluaField(const TPolynomialPtr<i64>& p) const -> TGaluaFieldPtr {
    auto f = std::make_shared<TGaluaField>(p, GaluaP_, GaluaN_);

    std::size_t maxPolynomialDisplaySize = 0;

    std::cout << "[Step 3] Built Galua field with " << f->GetElements().size() << " elements, display them: ";
    for (const auto& e : f->GetElements()) {
        maxPolynomialDisplaySize = std::max({maxPolynomialDisplaySize, e.ToString().size()});
        std::cout << e << " ";
    } std::cout << std::endl << std::endl;

    auto printHLine = [&] () {
        for (std::size_t i{0}; i < (maxPolynomialDisplaySize + 1) * (f->GetElements().size() + 1) + 1; i++) {
            std::cout << '-';
        }
        std::cout << std::endl;
    };
    auto printCell = [maxPolynomialDisplaySize] (const std::string& content = "") {
        std::cout << '|' << content;
        for (std::size_t i {content.size()}; i < maxPolynomialDisplaySize; i++) {
            std::cout << ' ';
        }
    };
    auto printHeader = [&] () {
        printHLine(); printCell();
        for (const auto& e : f->GetElements()) {
            printCell(e.ToString());
        }
        std::cout << '|' << std::endl;
        printHLine();
    };
    auto printOperationRow = [&] (const TPolynomial<i64>& p, const IMathOperationPtr<TPolynomial<i64>>& op) {
        printCell(p.ToString());
        for (const auto& e : f->GetElements()) {
            printCell(op->Apply(p, e).ToString());
        }
        std::cout << '|' << std::endl;
        printHLine();
    };

    std::cout << "[Step 3] [SubStep 1] Gonna display the additive table" << std::endl;
    printHeader(); 
    for (const auto& e : f->GetElements()) {
        printOperationRow(e, f->GetSumOperation());
    } std::cout << std::endl;
    
    std::cout << "[Step 3] [SubStep 2] Gonna display the multiplicative table" << std::endl;
    printHeader(); 
    for (const auto& e : f->GetElements()) {
        printOperationRow(e, f->GetMulOperation());
    } std::cout << std::endl;

    return f;
}

auto TCryptoApp::ExploreMultiplicativeGroup(const TGaluaFieldPtr& gf) const -> void {
    std::cout << "[Step 4] F*(n) is like {1";
    for (std::size_t i {1}; i < gf->GetElements().size(); i++) {
        std::cout << ", a^" << i; 
    } std::cout << "}" << std::endl;

    std::unordered_map<i64, std::vector<TPolynomial<i64>>> orderToElements;
    for (const auto& e : gf->GetElements()) {
        auto isOne = [] (const TPolynomial<i64>& p) {
            return !p.isZero() && p.Degree() == 0 && p[0] == 1;
        };
        
        if (e.isZero() || isOne(e)) {
            continue;
        }

        TPolynomial<i64> temp {e};
        i64 order = 1;
        
        while (!isOne(temp)) {
            temp = gf->GetMulOperation()->Apply(temp, e);
            order++;
        }

        if (!orderToElements.contains(order)) {
            orderToElements[order] = std::vector<TPolynomial<i64>> ();
        }
        orderToElements[order].push_back(e);
    }

    i64 baseOrder = gf->GetElements().size() - 1;
    if (!orderToElements.contains(baseOrder)) {
        std::cout << "Something got wrong... Abort" << std::endl;
        return;
    }

    std::cout << "[Step 4] F*(n)";
    for (const auto& e : orderToElements[baseOrder]) {
        std::cout << " = <" << e << ">";
    } std::cout << std::endl;

    std::vector<i64> subOrders;
    subOrders.reserve(orderToElements.size() - 1);

    for (const auto& it : orderToElements) {
        if (it.first != baseOrder) {
            subOrders.push_back(it.first);
        }
    }
    std::sort(subOrders.begin(), subOrders.end());

    for (std::size_t i {1}; i <= subOrders.size(); i++) {
        std::cout << "[Step 4] H" << i;
        for (const auto& e : orderToElements[subOrders[subOrders.size() - i]]) {
            std::cout << " = <" << e << ">";
        } std::cout << std::endl;
    }
}

auto TCryptoApp::RunCipherMode() const -> int {
    std::string alphabet = " abcdefghijklmnopqrstuvwxyz";
    std::cout << "Gonna use alphabet of " << alphabet.length() << " symbols '" << alphabet << "'" << std::endl;

    auto basePolynomial = std::make_shared<TPolynomial<i64>> (std::vector<i64> {1, 0, 2, 1});
    std::cout << "Gonna use F(n = 3, p = 3) with " << *basePolynomial << " base" << std::endl;

    auto galuaField = std::make_shared<TGaluaField> (basePolynomial, 3, 3);

    char aKey, bKey;
    std::cout << "Enter A part of key: ";
    std::cin >> aKey;
    
    std::cout << "Enter B part of key: ";
    std::cin >> bKey;

    auto charToPol = [&] (char ch) {
        return galuaField->GetElements().at(alphabet.find(ch));
    };

    auto polToChar = [&] (const TPolynomial<i64>& p) {
        for (std::size_t i {0}; i < galuaField->GetElements().size(); i++) {
            if (galuaField->GetElements().at(i) == p) {
                return alphabet[i];
            }
        }
        return ' ';
    };

    TPolynomial<i64> a = charToPol(aKey), b = charToPol(bKey);

    std::cout << "Key is (" << a << " x " << b << ")" << std::endl;

    if (CipherIsEncoding_) {
        std::cout << "Going to hide the text: " << CipherValue_ << std::endl;
        std::string result = "";
        for (char from : CipherValue_) {
            auto resultPolynomial = galuaField->GetSumOperation()->Apply(
                galuaField->GetMulOperation()->Apply(
                    a,
                    charToPol(from)
                ),
                b
            );
            char to = polToChar(resultPolynomial);
            std::cout << from << " -> " << charToPol(from) << " * " << a << " + " << b << " = " << resultPolynomial << " -> " << to << std::endl;
            result += to;
        }
        std::cout << "Encoding done, your cipher text is '" << result << "'" << std::endl;
    } else {
        auto getReversed = [&] (const TPolynomial<i64>& p) {
            for (const auto& e : galuaField->GetElements()) {
                auto mulOp = galuaField->GetMulOperation()->Apply(e, p);
                if (!mulOp.isZero() && mulOp.Degree() == 0 && mulOp[0] == 1) {
                    return e;
                }
            }
            throw std::invalid_argument("Unable to find reversed value for " + p.ToString());
        };

        std::cout << "Going to extract text from '" << CipherValue_ << "'" << std::endl;
        std::string result = "";
        for (char from : CipherValue_) {
            auto reversed = getReversed(a);
            auto resultPolynomial = galuaField->GetMulOperation()->Apply(
                (charToPol(from) - b) % 3,
                reversed
            );
            char to = polToChar(resultPolynomial);
            result += to;
            std::cout << from << " -> (" << charToPol(from) << " - " << b << ") * " << reversed << " = " << resultPolynomial << " -> " << to << "(" << reversed << " is " << a << "^-1)" << std::endl;
        }
        std::cout << "Your open text is '" << result << "'" << std::endl;
    }

    return 0;
}

auto TCryptoApp::ParseCommandLineArguments(int argc, char ** argv) -> EAppMode {
    argparse::ArgumentParser crypto {"crypto"};

    // -------------------- Configure galua cli args --------------------
    argparse::ArgumentParser galuaMode {"galua"};

    galuaMode.add_argument("p")
        .help("Power of simple endfull polynomial used for Galua field build")
        .scan<'i', ui64>();
    
    galuaMode.add_argument("n")
        .help("Power of polynomial used for Galua field build")
        .scan<'i', ui64>();

    galuaMode.add_argument("--autogen")
        .help("Automatically generate polynomial for Galua field")
        .flag();

    // -------------------- Configure cipher cli args --------------------
    argparse::ArgumentParser cipherMode {"cipher"};

    cipherMode.add_argument("value")
        .help("String for athenian coding process");

    auto& decodeEncodeGroup = cipherMode.add_mutually_exclusive_group(true);

    decodeEncodeGroup.add_argument("--decode")
        .help("Converts cipher to closed text")
        .flag();

    decodeEncodeGroup.add_argument("--encode")
        .help("Converts closed text to cipher")
        .flag();

    crypto.add_subparser(galuaMode);
    crypto.add_subparser(cipherMode);

    try {
        crypto.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << crypto << std::flush;
        return EAppMode::NoAppMode;
    }

    if (crypto.is_subcommand_used(galuaMode)) {
        GaluaN_ = galuaMode.get<ui64>("n");
        GaluaP_ = galuaMode.get<ui64>("p");
        GaluaAutogen_ = galuaMode.get<bool>("--autogen");

        return EAppMode::GaluaAppMode;
    }

    if (crypto.is_subcommand_used(cipherMode)) {
        CipherValue_ = cipherMode.get("value");
        CipherIsEncoding_ = cipherMode.is_used("--encode");

        return EAppMode::CipherAppMode;
    }

    return EAppMode::NoAppMode;
}

} // namespace kimp
