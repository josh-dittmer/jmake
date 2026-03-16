#pragma once

#include "array.h"    // IWYU pragma: keep
#include "dict.h"     // IWYU pragma: keep
#include "object.h"   // IWYU pragma: keep
#include "property.h" // IWYU pragma: keep
#include "tag.h"      // IWYU pragma: keep
#include "value.h"    // IWYU pragma: keep

#include "loader/json/json_loader.h" // IWYU pragma: keep

namespace core::schema {

// clang-format off
template <
    is_object ObjectType,
    typename SubschemasList = meta::List<>
>
// clang-format on
struct Schema;

// clang-format off
template <typename T>
concept is_schema = meta::is_specialization_of_v<Schema, T>;
// clang-format on

// clang-format off
template <
    is_object ObjectType,
    typename... Subschemas
>
struct Schema<
    ObjectType,
    meta::List<Subschemas...>
> : public ObjectType
// clang-format on
{
    Schema() = default;
    Schema(Schema&&) noexcept = default;

    Schema(const Schema&) = default;
    Schema& operator=(const Schema&) = default;
    Schema& operator=(Schema&&) = default;

    ~Schema() = default;

    // clang-format off
    explicit Schema(
        ObjectType&& object,
        std::tuple<Subschemas...> subschemas
    ) 
        : ObjectType(std::move(object)),
            m_subschemas(std::move(subschemas)) {}
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

    const auto& get_subschemas() const { return m_subschemas; }

  private:
    template <typename Object_>
    // clang-format off
    using rebind_object = Schema<
        Object_,
        meta::List<Subschemas...>
    >;
    // clang-format on

    template <typename Subschema_>
    // clang-format off
    using append_subschema = Schema<
        ObjectType,
        meta::List<Subschema_, Subschemas...>
    >;
    // clang-format on

    template <typename Object_>
    // clang-format off
    auto set_object(Object_&& object);
    // clang-format on

    std::tuple<Subschemas...> m_subschemas;
};

// clang-format off
template <
    typename OutType,
    typename... Ts
>
// clang-format on
extern auto schema(Ts... builders);

} // namespace core::schema

#include "schema.tpp"

#include "subschema.h" // IWYU pragma: keep