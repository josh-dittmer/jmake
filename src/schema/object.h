#pragma once

#include "detail/in_type.h"

#include "util/common_types/result/result.h"
#include "util/meta/nsdm.h"

#include <meta>
#include <type_traits>

namespace schema {

namespace detail {

// template <typename T, auto... As> //
// using Property[[= 1]] = std::optional<T>;

// clang-format off
template <
    typename T, 
    std::meta::info... As
> 
struct Property {
    int m_data;
};
// clang-format on

// clang-format off
template <
    std::meta::info ClassType, 
    std::meta::info Object
>
// clang-format on
extern consteval auto create_in_type();

template <typename T>
    requires std::is_class_v<T>
struct in_type<T> {
    struct data;
    consteval { create_in_type<^^data, ^^T>(); }

    using type = data;

    // for short-circuiting
    static constexpr std::size_t size = util::meta::num_nsdm(^^data);
    std::size_t m_num_set;

    // safely tie members to original Object members
    static consteval auto tie();
};

} // namespace detail

// clang-format off
template <
    typename Object,
    typename... Context,
    typename... Ts
> requires std::is_class_v<Object>
// clang-format on
extern Result<detail::in_type<Object>>
load(const std::tuple<Context...>& context, Ts&&... load_from);

// clang-format off
template <
typename Object,
typename... Ts
> requires std::is_class_v<Object>
// clang-format on
extern Result<detail::in_type<Object>> load(Ts&&... load_from);

} // namespace schema

#include "object.tpp"