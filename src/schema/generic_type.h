#pragma once

#include "detail/combiner.h"
#include "detail/in_type.h"
#include "detail/schema_error.h"
#include "detail/validator.h"

#include <meta>

namespace schema::detail {

template <typename T> struct generic_type_validator {
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
template <typename T> struct generic_type_combiner {
    static void combine(T& base, const T& in) { base = in; }
};

template <typename T> struct generic_type_in_type {
  public:
    using data_type = T;

    static auto to_out(data_type in) -> SchemaResult<T>;
};

// base case
template <typename T> struct validator : generic_type_validator<T> {};
template <typename T> struct combiner : generic_type_combiner<T> {};
template <typename T> struct in_type : generic_type_in_type<T> {};

// std::string
template <>
struct validator<std::string> : generic_type_validator<std::string> {};

template <>
struct combiner<std::string> : generic_type_combiner<std::string> {};

template <> struct in_type<std::string> : generic_type_in_type<std::string> {};

} // namespace schema::detail

#include "generic_type.tpp"