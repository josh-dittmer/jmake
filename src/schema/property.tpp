#pragma once

#include "property.h"

namespace schema {

template <std::meta::info Property> //
constexpr auto get_out_member() {
    using prop_t = [:std::meta::type_of(Property):];
    return prop_t::member;
}

} // namespace schema