#pragma once

#include "schema/tag.h"

#include <glaze/glaze.hpp>

#include <concepts>

namespace schema {

struct json_id : string_tag {
    using string_tag::string_tag;
};

struct enable_glz {};

namespace detail {

template <std::meta::info R> //
consteval auto create_glz_object();

} // namespace detail

} // namespace schema

// glz::meta specialization to enable field renaming w/ annotations
// clang-format off
template <typename T>
    requires std::derived_from<T, schema::enable_glz>
struct glz::meta<T> {
    static constexpr auto modify = [:::schema::detail::create_glz_object<^^T>():];
};
// clang-format onw

#include "glaze_loader.tpp"