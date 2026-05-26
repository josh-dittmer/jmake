#pragma once

#include "schema/object.h"
#include "schema/tag.h"

#include <glaze/glaze.hpp>

#include <meta>

namespace schema::glz {

struct id : schema::string_tag {
    using string_tag::string_tag;
};

namespace detail {

template <auto... Args>
extern consteval decltype(::glz::object(Args...)) glz_object_pack();

template <auto Mem> extern consteval auto field_key();
template <typename In, auto Mem> extern consteval auto field_getter();

} // namespace detail
} // namespace schema::glz

// glz::meta specialization to enable field renaming w/ annotations for objects
template <typename T> struct glz::meta<::schema::detail::in_object<T>> {
  private:
    static consteval auto create_glz_object();
    static consteval auto create_args();

    template <std::meta::info Member> static consteval auto field_getter();

  public:
    using in_object_t = ::schema::detail::in_object<T>;
    using data_t = in_object_t::data_type;

    static constexpr auto value = create_glz_object();
};

#include "glaze_loader.tpp"
