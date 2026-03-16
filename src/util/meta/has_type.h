#pragma once

#include <type_traits>

namespace meta {

// Helper template to check if a type T exists in a list of types

// clang-format off
template <
    typename T, 
    typename... Ts
>
// clang-format on
struct has_type;

// Specialization for empty list

// clang-format off
template <
    typename T
> 
struct has_type<
    T
> : std::false_type
// clang-format off
{};

// Specialization for non-empty list

// clang-format off
template <
    typename T, 
    typename U, 
    typename... Rest
>
struct has_type<
    T, 
    U, 
    Rest...
> : std::conditional_t<
    std::is_same_v<T, U>, 
    std::true_type,
    has_type<
        T,
        Rest...
    >
>
// clang-format on
{};

// Inline constexpr bool version for easier use
template <typename T, typename... Ts>
inline constexpr bool has_type_v = has_type<T, Ts...>::value;

} // namespace meta
