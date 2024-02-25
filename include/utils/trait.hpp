#pragma once

#include <type_traits>

namespace kimp::utils {

template<typename, typename>
constexpr bool is_same_template{false};

template<template<typename...> class T, typename... A, typename... B>
constexpr bool is_same_template<T<A...>, T<B...>> {true};

template < template <typename...> class Template, typename T >
constexpr bool is_specialization_of {false};

template < template <typename...> class Template, typename... Args >
constexpr bool is_specialization_of< Template, Template<Args...> > {true};

} // namespace kimp::utils
