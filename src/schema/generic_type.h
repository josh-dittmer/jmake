#pragma once

#include "detail/schema_error.h"

#include <meta>

namespace schema::detail {

template <typename T> struct validator {
    using type = T;

    // clang-format off
    template <
        std::meta::info Member,
        typename... Context
    >
    // clang-format on
    static auto validate(const std::tuple<Context...>& context,
                         const type& data) -> SchemaResult<>;
};

// default combiner simply overwrites
template <typename T> struct combiner {
    static void combine(T& base, const T& in) { base = in; }
};

template <typename T> struct in_type {
  public:
    using data_type = T;

    static auto to_out(data_type in) -> SchemaResult<T>;
};

} // namespace schema::detail

#include "generic_type.tpp"