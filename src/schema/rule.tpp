#pragma once

#include "rule.h"

#include "schema/tag.h"
#include "util/meta/annotation.h" // IWYU pragma: keep

#include <meta>

namespace schema {

// clang-format off
template <
    std::meta::info Member,
    typename... Context,
    typename T
>
// clang-format on
Result<> rules_satisfied(const std::tuple<Context...>& context, const T& data) {
    template for (constexpr auto a : util::meta::ann_arr(Member)) {
        auto tag_val = tag_from_annotation<a>(context);
        using tag_type = decltype(tag_val);

        constexpr bool is_rule = requires(tag_type tag) {
            { tag_val.rule(data) } -> std::same_as<Result<>>;
        };

        if constexpr (is_rule) {
            HUH(tag_val.rule(data));
        }
    }

    return ok();
}

} // namespace schema