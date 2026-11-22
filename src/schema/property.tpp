#pragma once

#include "property.h"

namespace schema {

namespace detail {

// clang-format off
template <
    std::meta::info OutMember
>
// clang-format on
consteval auto Property<OutMember>::flatten()
    requires(is_in_object)
{
    // iterate through object
    return data_type::flatten();
}

// clang-format off
template <
    std::meta::info OutMember
>
// clang-format on
consteval auto Property<OutMember>::flatten()
    requires(!is_in_object)
{
    return;
}

} // namespace detail

template <std::meta::info Property> //
constexpr auto get_out_member() {
    using prop_t = [:std::meta::type_of(Property):];
    return prop_t::member;
}

template <std::meta::info Property> //
extern constexpr auto get_data_type() {
    using p_t [[maybe_unused]] = [:std::meta::type_of(Property):];
    using pd_t [[maybe_unused]] = typename p_t::data_type;

    return std::meta::dealias(^^pd_t);
}

} // namespace schema