#pragma once

#include "generic_type.h"
#include "schema/rule.h"

namespace schema {

namespace detail {

// clang-format off
template <typename T>
auto out_type<T>::from_in_type(in_type<T> in_type) -> Result<T>
// clang-format on
{
    return std::move(in_type);
}

} // namespace detail

// clang-format off
template <
    std::meta::info Member, 
    typename T,
    typename... Context
>
// clang-format on
Result<detail::in_type<T>> load(T&& load_from, Context&&... context) {
    auto rule_res = rules_satisfied<Member>(std::forward<T>(load_from),
                                            std::forward<Context>(context)...);

    if (!rule_res) {
        return err(rule_res);
    }

    return load_from;
}

} // namespace schema