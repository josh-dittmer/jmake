#pragma once

#include <type_traits>

namespace meta {

template <template <typename...> class Template, typename>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
inline constexpr bool is_specialization_of_v =
    is_specialization_of<Template, T>::value;

template <template <typename...> class Template, typename T>
concept specialization_of = is_specialization_of_v<Template, T>;

} // namespace meta
