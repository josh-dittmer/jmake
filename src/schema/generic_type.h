#pragma once

#include "detail/in_type.h"
#include "util/common_types/result/result.h"

#include <meta>

namespace schema {

namespace detail {

template <typename T> struct in_type {
    using type = T;
};

template <typename T> struct out_type {
    using type = T;

    static auto from_in_type(in_type<T> in_type) -> Result<type>;
};

} // namespace detail

// clang-format off
template <
    std::meta::info Member, 
    typename T,
    typename... Context
>
// clang-format on
extern Result<detail::in_type<T>> load(T&& load_from, Context&&... context);

} // namespace schema

#include "generic_type.tpp"