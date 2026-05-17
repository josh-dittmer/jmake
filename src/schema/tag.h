#pragma once

#include <meta>
#include <string_view>

namespace schema {

template <typename Fn> struct rt {
    explicit consteval rt(Fn fn) : m_fn(fn) {}

    Fn m_fn;
};

struct string_tag {
    consteval string_tag(std::string_view str) // NOLINT
        : m_str(std::define_static_string(str)) {}

    string_tag(const char* str) : m_str(str) {} // NOLINT

    const char* m_str;
};

// clang-format off
template <
    std::meta::info Ann,
    typename... Context
>
// clang-format on
extern constexpr auto tag_from_annotation(Context&&... context);

// clang-format off
template <
    std::meta::info Member,
    typename Tag,
    typename... Context
>
// clang-format on
extern constexpr std::optional<Tag> get_tag(Context&&... context);

} // namespace schema

#include "tag.tpp"