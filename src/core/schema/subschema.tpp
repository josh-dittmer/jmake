#pragma once

#include "core/schema/detail/common.h"
#include "subschema.h"

namespace core::schema {

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
template <
    typename Tag_
>
auto Subschema<
    ParentRef,
    SchemaType
>::add_tag(Tag_ tag)
// clang-format on
{
    return set_schema(SchemaType::template add_tag<Tag_>(std::move(tag)));
}

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
template <
    typename Property_
>
auto Subschema<
    ParentRef,
    SchemaType
>::add_property(Property_ property)
// clang-format on
{
    return set_schema(
        SchemaType::template add_property<Property_>(std::move(property)));
}

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
template <
    typename Subschema_
>
auto Subschema<
    ParentRef,
    SchemaType
>::add_subschema(Subschema_ subschema)
// clang-format on
{
    return set_schema(
        SchemaType::template add_subschema<Subschema_>(std::move(subschema)));
}

// clang-format off
template <
    auto ParentRef,
    is_schema SchemaType
>
template <
    typename Schema_
>
auto Subschema<
    ParentRef,
    SchemaType
>::set_schema(Schema_&& schema)
// clang-format on
{
    return rebind_schema<Schema_>{std::forward<Schema_>(schema)};
}

// clang-format off
template <
    auto ParentRef,
    typename... Ts
>
// clang-format on
auto subschema(Ts... builders) {
    using out_type = decltype(detail::deduce_class_from_member(ParentRef));

    // clang-format off
    return detail::apply_builders(
        Subschema<ParentRef, Schema<Object<out_type>>>{},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace core::schema