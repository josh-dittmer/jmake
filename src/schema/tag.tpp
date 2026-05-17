#pragma once

#include "tag.h"

#include "util/meta/expand.h"

#include <concepts>
#include <meta>

namespace schema {

// clang-format off
template <
    std::meta::info Ann,
    typename... Context
>
constexpr auto tag_from_annotation(Context&&... context) {
    constexpr auto a_info = std::meta::type_of(Ann);
    using a_type = [: a_info :];

    constexpr auto tag_ext = std::meta::extract<a_type>(Ann);

    if constexpr (std::meta::has_template_arguments(a_info) &&
                  std::meta::template_of(a_info) == ^^rt) {
        constexpr bool is_valid = requires {
            tag_ext.m_fn(std::forward<Context>(context)...);
        };

        if constexpr (is_valid) {
            return tag_ext.m_fn(std::forward<Context>(context)...);
        } else {
            return tag_ext;
        }
    }

    else {
        return tag_ext;
    }
}
// clang-format on

// clang-format off
template <
    std::meta::info Member,
    typename Tag,
    typename... Context
>
// clang-format on
constexpr std::optional<Tag> get_tag(Context&&... context) {
    std::optional<Tag> tag_data;

    // clang-format off
    [: util::meta::expand(std::meta::annotations_of(Member)) :] >> [&]<auto ann>() constexpr {
        auto tag_val = tag_from_annotation<ann>(std::forward<Context>(context)...);
        using tag_type = decltype(tag_val);

        if constexpr (std::same_as<Tag, tag_type>) {
            tag_data = tag_val;
        }
    };
    // clang-format on

    return tag_data;
}

} // namespace schema