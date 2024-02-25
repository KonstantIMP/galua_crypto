#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <math/gcd.hpp>

#include <tuple>

TEST_CASE ("GCD calculating", "[gcd]") {
    auto testData = GENERATE(
        as<std::tuple<i32, i32, i32>>{}
        , std::make_tuple(0, 0, 0)
        , std::make_tuple(1, 0, 1)
        , std::make_tuple(1, 1, 1)
        , std::make_tuple(5, 5, 5)
        , std::make_tuple(8, 2, 2)
        , std::make_tuple(8, 4, 4)
        , std::make_tuple(11, 3, 1)
        , std::make_tuple(99, 98, 1)
        , std::make_tuple(100, 55, 5)
        , std::make_tuple(11, 55, 11)
    );

    SECTION ("easy implementation") {
        REQUIRE(kimp::math::gcd(std::get<0>(testData), std::get<1>(testData)) == std::get<2>(testData));
        REQUIRE(kimp::math::gcd(std::get<1>(testData), std::get<0>(testData)) == std::get<2>(testData));
    }

    SECTION ("extended implementation") {
        i32 x, y, gcdExpected;
        std::tie(x, y, gcdExpected) = testData;

        i32 gcd, a, b;
        std::tie(gcd, a, b) = kimp::math::gcdExtended(x, y);

        REQUIRE(gcd == gcdExpected);
        REQUIRE(gcd == a * x + b * y);
    }
}

auto main(int argc, char ** argv) -> int {
    return Catch::Session().run(argc, argv);
}
