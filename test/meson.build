catch = dependency('catch2')

test_cases = [
    ['gcd', ['math/gcd.cpp']]
]

foreach t : test_cases
    test(t[0], executable(t[0], t[1], dependencies: [crypto, catch]))
endforeach
