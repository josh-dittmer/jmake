#pragma once

#include "detail/in_type.h"
#include "detail/out_type.h"
#include "util/common_types/result/result.h"

namespace schema {

namespace detail {

template <typename T> struct in_type<std::vector<T>> {
    using type = T;
};

template <typename T> struct out_type<std::vector<T>> {
    using type = T;

    static auto from_in_type(in_type_t<T> in_type) -> type;
};

} // namespace detail

// clang-format off
template <auto Member, typename T>
// clang-format on
extern auto load(T load_from) -> Result<detail::in_type_t<T>>;

} // namespace schema

#include "array.tpp"