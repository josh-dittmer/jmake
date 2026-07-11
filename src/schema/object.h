#pragma once

#include "detail/combiner.h"
#include "detail/in_type.h"
#include "detail/schema_error.h"
#include "detail/validator.h"

#include "util/meta/nsdm.h" // IWYU pragma: keep

#include <concepts>
#include <meta>
#include <type_traits>

namespace schema {

namespace detail {

// clang-format off
template <
    std::meta::info ClassType, 
    std::meta::info Object
>
// clang-format on
extern consteval auto create_object_in_type();

// clang-format off
template <typename T>
// clang-format on
struct in_object {
    struct data;
    consteval { create_object_in_type<^^data, ^^T>(); }

    using data_type = data;

    // pair: {constant array, member}
    using flat_pair = std::pair<std::meta::info, std::meta::info>;

    static consteval auto flatten();

    template <std::meta::info... Path> //
    static consteval auto flatten_impl();

    data_type m_data;
};

template <typename T> struct validator<in_object<T>> {
    using type = in_object<T>;

    // clang-format off
    template <
        std::meta::info,
        typename... Context
    >
    // clang-format on
    static auto validate(const std::tuple<Context...>& context,
                         const type& data) -> SchemaResult<>;
};

template <typename T> struct combiner<in_object<T>> {
    using type = in_object<T>;

    static void combine(type& base, const type& in);
};

template <typename T>
    requires std::is_class_v<T>
struct in_type<T> {
  public:
    using data_type = in_object<T>;

    static auto to_out(data_type in) -> SchemaResult<T>;
};

} // namespace detail

// clang-format off
template <
    typename... Context,
    typename T,
    typename... Ts
>
// clang-format on
extern detail::SchemaResult<T> load(const std::tuple<Context...>& context,
                                    detail::in_object<T> in1, Ts&&... rest)
    requires(std::convertible_to<Ts, detail::in_object<T>> && ...);

// clang-format off
template <
    typename T,
    typename... Ts
>
// clang-format on
extern detail::SchemaResult<T> load(detail::in_object<T> in1, Ts&&... rest)
    requires(std::convertible_to<Ts, detail::in_object<T>> && ...);

} // namespace schema

#include "object.tpp"