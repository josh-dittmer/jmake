#pragma once

#include "generic_type.h"
#include "schema/rule.h"

namespace schema::detail {

// clang-format off
template <typename T>
template <
    std::meta::info Member,
    typename... Context
>
// clang-format on
auto generic_type_validator<T>::validate(const std::tuple<Context...>& context,
                                         const type& data) -> SchemaResult<> {
    // make sure all rules are satisfied
    HUH(rules_satisfied<Member>(context, data));

    return ok();
}

template <typename T>
auto generic_type_in_type<T>::to_out(data_type in) -> SchemaResult<T> {
    return in;
}

} // namespace schema::detail