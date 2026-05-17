#pragma once

#include "rule.h"

#include "schema/tag.h"
#include "util/meta/expand.h"

namespace schema {

// clang-format off
template <
    std::meta::info Member,
    typename T,
    typename... Context
>
// clang-format on
Result<> rules_satisfied(T&& data, Context&&... context) {
    Result<> result = ok();

    // clang-format off
    [: util::meta::expand(std::meta::annotations_of(Member)) :] >> [&]<auto ann> {
        if (!result) {
            return;
        }

        auto tag_val = tag_from_annotation<ann>(std::forward<Context>(context)...);
        using tag_type = decltype(tag_val);

        constexpr bool is_rule = requires(tag_type tag) {{
            tag_val.rule(std::forward<T>(data)) 
        } -> std::same_as<Result<>>; };

        if constexpr (is_rule) {
            result = tag_val.rule(std::forward<T>(data));
        }
    };
    // clang-format on

    return result;
}

} // namespace schema