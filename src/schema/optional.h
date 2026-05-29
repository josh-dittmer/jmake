#pragma once

#include "detail/combiner.h"
#include "detail/in_type.h"
#include "detail/schema_error.h"
#include "detail/validator.h"

#include <optional>

namespace schema::detail {

template <typename T> struct validator<std::optional<T>> {
    using type = std::optional<T>;

    // clang-format off
        template <
        std::meta::info Member,
        typename... Context
    >
    // clang-format on
    static auto validate(const std::tuple<Context...>& context,
                         const type& data) -> SchemaResult<>;
};

template <typename T> struct combiner<std::optional<T>> {
    using type = std::optional<T>;

    static void combine(type& base, const type& in);
};

template <typename T> struct in_type<std::optional<T>> {
  public:
    using data_type = std::optional<in_data_t<T>>;

    static auto to_out(data_type in) -> SchemaResult<std::optional<T>>;
};

} // namespace schema::detail

#include "optional.tpp"