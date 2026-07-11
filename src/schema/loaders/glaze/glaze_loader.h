#pragma once

#include "schema/loaders/common/get_nested_ref.h"
#include "schema/object.h"
#include "schema/tag.h"

#include <glaze/glaze.hpp>

#include <meta>
#include <optional>

namespace schema::glz {

// glz tags

// renames a glz field from it's default
struct rename : string_tag {
    using string_tag::string_tag;
};

// disable a member from being settable via glz
struct disable {};

namespace detail {

template <auto... Args>
extern consteval decltype(::glz::object(Args...)) glz_object_pack();

template <std::meta::info First, std::meta::info... Path>
extern consteval void get_full_key_impl(std::string& key);

template <std::meta::info... Path> //
extern consteval auto get_full_key();

template <std::meta::info First, std::meta::info... Path>
extern auto& field_getter_impl(auto& parent);

} // namespace detail
} // namespace schema::glz

// glz::meta specialization to enable field renaming w/ annotations for objects
template <typename T> struct glz::meta<schema::detail::in_object<T>> {
  private:
    static consteval auto make_args();

    /*template <std::meta::info Member, std::meta::info... Path>
    static consteval auto make_args_from_member();

    template <typename Data, std::meta::info... Path> //
    static consteval auto make_args_impl();*/

    static consteval auto create_glz_object();

    // Concrete-signature getter that is the entity substituted to form the glz
    // field accessor. Defined inline (its return type is a dependent splice, so
    // a separate declaration/definition pair cannot be reliably matched).
    template <std::meta::info First, std::meta::info... Path>
    static typename[:std::meta::type_of(First):] ::opt_type&
    field_getter(::schema::detail::in_object<T>& self) {
        return ::schema::common::get_nested_ref<First, Path...>(self);
    }

  public:
    using in_t = ::schema::detail::in_object<T>;

    static constexpr auto value = create_glz_object();
};

#include "glaze_loader.tpp"
