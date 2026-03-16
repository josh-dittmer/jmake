#pragma once

#include "tag.h"

namespace core::schema {

namespace detail {

// clang-format off
template <
    typename Tag
>
template <
    accepts_tags<Tag> T
>    
auto TagBuilder<Tag>::build(T type)
// clang-format on
{
    return type.add_tag(std::move(m_tag));
}

} // namespace detail

// clang-format off
template <
    typename Tag, 
    typename... Args
>
// clang-format on
auto tag(Args... args) {
    // clang-format off
    return detail::TagBuilder<Tag>{Tag{std::move(args)...}};
    // clang-format on
}

// clang-format off
template <
    typename Tag
>
// clang-format on
auto tag(Tag tag) {
    // clang-format off
    return detail::TagBuilder<Tag>{std::move(tag)};
    // clang-format on
}

} // namespace core::schema