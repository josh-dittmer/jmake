#pragma once

#include "value.h"

#include "detail/builder.h"

namespace core::schema {

// clang-format off
template <
    typename T,
    typename... Tags
>
template <
    typename U
>
auto ValueInit<
    T,
    meta::List<Tags...>
>::to_value(std::set<U>& set_ref)
// clang-format on
{
    auto [it, _] = set_ref.emplace(std::move(m_value));

    // clang-format off
    return Value<
        U,
        meta::List<Tags...>
    >{
        *it,
        std::move(m_tags)
    };
    // clang-format on
}

// clang-format off
template <
    typename T,
    typename... Tags
>
template <
    typename Tag_
>
auto ValueInit<
    T,
    meta::List<Tags...>
>::add_tag(Tag_ tag)
// clang-format on
{
    // clang-format off
    return typename ValueInit::append_tag<Tag_>{
        std::move(this->m_value),
        std::tuple_cat(
            std::make_tuple(std::move(tag)),
            std::move(this->m_tags)
        )
    };
    // clang-format on
}

// clang-format off
template <
    typename T, 
    typename... Ts
>
// clang-format on
auto value(T value, Ts... builders) {
    // clang-format off
    return detail::apply_builders(
        ValueInit<T>{std::move(value)},
        std::move(builders)...
    );
    // clang-format on
}
// clang-format on

// clang-format off
template <
    typename T, 
    typename... Ts
>
// clang-format on
auto default_value(T value, Ts... builders) {
    // clang-format off
    return detail::apply_builders(
        DefaultValueInit<T>{std::move(value)},
        std::move(builders)...
    );
    // clang-format on
}
// clang-format on

} // namespace core::schema