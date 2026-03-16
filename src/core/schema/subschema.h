#pragma once

#include "schema.h"

namespace core::schema {

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
// clang-format on
struct Subschema;

namespace detail {

// clang-format off
template <
    typename T,
    typename Subschema
>
// clang-format on
concept accepts_subschemas = requires(T type, Subschema subschema) {
    type.add_subschema(std::move(subschema));
};

// clang-format off
template <typename>
struct is_subschema_impl : std::false_type {};
// clang-format on

// clang-format off
template <auto ParentRef, typename... Ts>
struct is_subschema_impl<Subschema<ParentRef, Ts...>> : std::true_type {};
// clang-format on

} // namespace detail

// clang-format off
template <typename T>
// clang-format on
concept is_subschema = detail::is_subschema_impl<T>::value;

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
// clang-format on
struct Subschema : public SchemaType {
    Subschema() = default;
    Subschema(Subschema&&) noexcept = default;

    Subschema(const Subschema&) = default;
    Subschema& operator=(const Subschema&) = default;
    Subschema& operator=(Subschema&&) = default;

    ~Subschema() = default;

    // clang-format off
    explicit Subschema(SchemaType&& schema) : SchemaType(std::move(schema)) {}
    // clang-format on

    // clang-format off
    template <typename Tag_>
    auto add_tag(Tag_ tag);
    // clang-format on

    // clang-format off
    template <typename Property_>
    auto add_property(Property_ property);
    // clang-format on

    // clang-format off
    template <typename Subschema_>
    auto add_subschema(Subschema_ subschema);
    // clang-format on

    // clang-format off
    template <
        detail::accepts_subschemas<Subschema> T
    >
    // clang-format on
    auto build(T type) {
        return type.add_subschema(std::move(*this));
    }

  private:
    template <typename Schema_>
    // clang-format off
    using rebind_schema = Subschema<
        ParentRef,
        Schema_
    >;
    // clang-format on

    template <typename Schema_>
    // clang-format off
    auto set_schema(Schema_&& schema);
    // clang-format on
};

// clang-format off
template <
    auto ParentRef,
    typename... Ts
>
// clang-format on
extern auto subschema(Ts... builders);

} // namespace core::schema

#include "subschema.tpp"