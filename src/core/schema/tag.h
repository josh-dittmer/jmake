#pragma once

#include <string>
#include <utility>

namespace core::schema {

// common tags
namespace tags {

// if missing, use an empty container as the default value
struct empty_as_default {};

// provide a readable name
struct name {
    std::string m_name;
};

} // namespace tags

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept accepts_tags = requires(T type, Tag tag) {
    type.add_tag(std::move(tag));
};
// clang-format on

template <typename T, typename Tag>
concept tag_for = requires(const T& type, const T::out_type& value, Tag tag) {
    tag.check(type, value);
};

// clang-format off
template <
    typename Tag
>
// clang-format on
struct TagBuilder {
    explicit TagBuilder(Tag&& tag) : m_tag(std::move(tag)) {}

    // clang-format off
    template <
        accepts_tags<Tag> T
    >
    auto build(T type);
    // clang-format on

  private:
    Tag m_tag;
};

} // namespace detail

// clang-format off
template <
    typename Tag, 
    typename... Args
>
extern auto tag(Args... args);
// clang-format on

// clang-format off
template <
    typename Tag
>
extern auto tag(Tag tag);
// clang-format on

} // namespace core::schema

#include "tag.tpp"