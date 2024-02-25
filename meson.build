project(
    'crypto'
    , ['cpp']
    , default_options: 'cpp_std=c++20'
    , version: '0.0.1'
)

crypto_sources = [
    'source/crypto.cpp'
]

crypto_dependencies = [
    dependency('argparse')
    , dependency('fmt')
]

crypto_include = include_directories('./include')
crypto_so = shared_library(
    'libcrypto'
    , crypto_sources
    , dependencies: crypto_dependencies
    , include_directories: crypto_include
)

crypto = declare_dependency(
    link_with: crypto_so
    , dependencies: crypto_dependencies
    , include_directories: crypto_include
)

executable(
    'app'
    , ['source/main.cpp']
    , dependencies: [crypto]
)

subdir('./test/')
