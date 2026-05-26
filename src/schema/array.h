#pragma once

#include "detail/combiner.h"
#include "detail/in_type.h"
#include "detail/validator.h"
#include "schema/detail/schema_error.h"

#include <meta>

namespace schema::detail {

template <typename T> struct array_validator {
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

template <typename T> struct array_combiner {
    using type = T;

    static void combine(type& base, const type& in);
};
// for std::vector

template <typename T>
struct validator<std::vector<T>> : array_validator<std::vector<T>> {};

template <typename T>
struct combiner<std::vector<T>> : array_combiner<std::vector<T>> {};

template <typename T> struct in_type<std::vector<T>> {
  public:
    using data_type = std::vector<in_data_t<T>>;

    static auto to_out(data_type in) -> SchemaResult<std::vector<T>>;
};

// for std::array

template <typename T, std::size_t N>
struct validator<std::array<T, N>> : array_validator<std::array<T, N>> {};

template <typename T, std::size_t N>
struct combiner<std::array<T, N>> : array_combiner<std::array<T, N>> {};

template <typename T, std::size_t N> struct in_type<std::array<T, N>> {
  public:
    using data_type = std::array<in_data_t<T>, N>;

    static auto to_out(data_type in) -> SchemaResult<std::array<T, N>>;
};

} // namespace schema::detail

#include "array.tpp"