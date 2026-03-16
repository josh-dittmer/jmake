#pragma once

#include "schema.h"

#include "detail/builder.h"

namespace core::schema {

// clang-format off
template <
    is_object ObjectType,
    typename... Subschemas
>
template <
    typename Tag_
>
auto Schema<
    ObjectType,
    meta::List<Subschemas...>
>::add_tag(Tag_ tag)
// clang-format on
{
    return set_object(ObjectType::template add_tag<Tag_>(std::move(tag)));
}

// clang-format off
template <
    is_object ObjectType,
    typename... Subschemas
>
template <
    typename Property_
>
auto Schema<
    ObjectType,
    meta::List<Subschemas...>
>::add_property(Property_ property)
// clang-format on
{
    return set_object(
        ObjectType::template add_property<Property_>(std::move(property)));
}

// clang-format off
template <
    is_object ObjectType,
    typename... Subschemas
>
template <
    typename Subschema_
>
auto Schema<
    ObjectType,
    meta::List<Subschemas...>
>::add_subschema(Subschema_ subschema)
// clang-format on
{
    // clang-format off
    return typename Schema::append_subschema<Subschema_>{
        std::move(*static_cast<ObjectType*>(this)),
        std::tuple_cat(
            std::make_tuple(std::move(subschema)),
            std::move(this->m_subschemas)
        )
    };
    // clang-format on
}

// clang-format off
template <
    is_object ObjectType,
    typename... Subschemas
>
template <
    typename Object_
>
auto Schema<
    ObjectType,
    meta::List<Subschemas...>
>::set_object(Object_&& object)
// clang-format on
{
    // clang-format off
    return rebind_object<Object_>{
        std::forward<Object_>(object),
        std::move(this->m_subschemas)
    };
    // clang-format on
}

// clang-format off
template <
    typename OutType,
    typename... Ts
>
// clang-format on
auto schema(Ts... builders) {
    // clang-format off
    return detail::apply_builders(
        Schema<Object<OutType>>{},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace core::schema