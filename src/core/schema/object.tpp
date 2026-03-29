#pragma once

#include "object.h"

#include "detail/builder.h"
#include "loader/loader.h"
#include "util/meta/matched_for_each.h"
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
auto Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::load(const Ts&... load_from) const -> detail::SchemaResult<typename Object::in_type> {
    auto load_pack = result_pack(Loader<Ts>::for_object(*this, load_from)...);
    
    auto load_res = load_pack.ok();
    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Object::in_type result;

    // for each property
    auto prop_res = meta::matched_for_each_until(m_properties, result.m_data, [&](const auto& prop, auto& in) -> detail::SchemaResult<> {        
        // attempt to load
        auto load_res = load_pack.unwrap([&](const auto&... load_from) {
            return prop.load(load_from...);
        });
        
        if (!load_res.ok()) {
            return err(load_res);
        }

        // all is good? assign
        in = std::move(load_res.unwrap());

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
    typename... Tags,
    typename... Properties
>
void Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::combine(Object::in_type& base, Object::in_type& input)
// clang-format on
{
    // loop through each input property and assign to base if set
    meta::matched_for_each(base.m_data, input.m_data, [&](auto& bp, auto& ip) {
        if (!bp.m_data && ip.m_data) {
            bp.m_data = ip.m_data;
            base.m_num_set++;
        }
    });
}

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
bool Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::satisfied(const Object::in_type& base)
// clang-format on
{
    return base.m_num_set >= base.size;
}

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
auto Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::to_out(Object::in_type& base) const -> detail::SchemaResult<typename Object::out_type>
// clang-format on
{
    typename Object::out_type result;

    // clang-format off
    auto fill_res = meta::matched_for_each_until(m_properties, base.m_data, [&](auto& prop, auto& prop_in) -> detail::SchemaResult<> {
        auto prop_res = prop.to_out(prop_in);
        if (!prop_res.ok()) {
            return err(prop_res);
        }

        // assign field in result type
        result.*(prop_in.out_ref) = std::move(prop_res.unwrap());

        return ok();
    });
    // clang-format on

    if (!fill_res.ok()) {
        return err(fill_res);
    }

    return ok(std::move(result));
}

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
auto Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
>::get_default_value() const -> Opt<typename Object::out_type>
// clang-format on
{
    typename Object::out_type result;

    bool missing = false;

    // clang-format off
    meta::for_each(m_properties, [&](auto& prop) {
        if (missing) {
            return;
        }
        
        auto dv = prop.get_default_value();

        // no default value? exit
        if (!dv) {
            missing = true;
            return;
        }

        // assign field in result type
        result.*(prop.out_ref) = std::move(*dv);
    });
    // clang-format on

    if (missing) {
        return std::nullopt;
    }

    return std::move(result);
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