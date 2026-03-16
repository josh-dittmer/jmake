#pragma once

#include <concepts>

namespace core::schema::detail {

// clang-format off
template <
    typename Derived
>
// clang-format on
struct IsType {
    // clang-format off
    template <
        auto OutRef,
        typename... Ts
    >
    // clang-format on
    auto as_prop(Ts... builders);

    // clang-format off
    template <
        typename... Ts
    >
    // clang-format on
    auto as_array(Ts... builders);

    // clang-format off
    template <
        typename... Ts
    >
    // clang-format on
    auto as_dict(Ts... builders);
};

template <typename T>
concept is_type = std::derived_from<T, IsType<T>>;

} // namespace core::schema::detail