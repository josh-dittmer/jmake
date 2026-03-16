#pragma once

#include "object.h"

#include "detail/builder.h"
#include "loader/loader.h"
#include "util/meta/for_each.h"
#include "util/result/result_pack.h"

namespace core::schema {

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
template <
    typename... Ts
>
detail::SchemaResult<OutType> Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::load(const Ts&... load_from) const {
    auto load_pack = result_pack(Loader<Ts>::for_object(*this, load_from)...);
    
    auto load_res = load_pack.ok();
    if (!load_res.ok()) {
        return err(load_res);
    }

    OutType result;

    // for each property
    auto prop_res = meta::for_each_until(m_properties, [&](const auto& prop) -> detail::SchemaResult<> {
        using T = std::remove_cvref_t<decltype(prop)>;
        
        // attempt to load
        auto load_res = load_pack.unwrap([&](const auto&... load_from) {
            return prop.load(load_from...);
        });
        
        if (!load_res.ok()) {
            return err(load_res);
        }

        // all is good? assign
        result.*T::out_ref = std::move(load_res.unwrap());

        return ok();
    });

    if (!prop_res.ok()) {
        return err(prop_res);
    }

    // make sure object itself is valid
    auto check_res = this->check_against_tags(result);
    if (!check_res.ok()) {
        return err(check_res);
    }

    return ok(std::move(result));
}

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
template <
    typename Tag_
>
auto Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::add_tag(Tag_ tag)
// clang-format on
{
    // clang-format off
    return typename Object::append_tag<Tag_>{
        std::tuple_cat(
            std::make_tuple(std::move(tag)),
            std::move(this->m_tags)
        ),
        std::move(this->m_properties)
    };
    // clang-format on
}

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
template <
    typename Property_
>
auto Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::add_property(Property_ property)
// clang-format on
{
    // clang-format off
    return typename Object::append_property<Property_>{
        std::move(this->m_tags),
        std::tuple_cat(
            std::make_tuple(std::move(property)),
            std::move(this->m_properties)
        )
    };
    // clang-format on
}

// clang-format off
template <
    typename OutType,
    typename... Ts
>
// clang-format on
auto object(Ts... builders) {
    // clang-format off
    return detail::apply_builders(
        Object<OutType>{},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace core::schema