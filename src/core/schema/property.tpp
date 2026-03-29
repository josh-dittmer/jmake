#pragma once

#include "core/schema/detail/common.h"
#include "core/schema/detail/schema_error.h"
#include "core/schema/tag.h"
#include "detail/builder.h"
#include "generic_type.h"
#include "property.h"
#include "util/meta/for_each.h"
#include "util/optional/optional.h"

namespace core::schema {

namespace detail {

// implementation for IsType::as_prop(), declared in
// detail/type_traits/is_type.h

// clang-format off
template <
    typename Derived
>
template <
    auto OutRef,
    typename... Ts
>
auto IsType<
    Derived
>::as_prop(Ts... builders)
// clang-format on
{
    // clang-format off
    return detail::apply_builders(
        Property<
            OutRef,
            Derived
        >{std::move(*static_cast<Derived*>(this))},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace detail

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename... Tags
>
template <
    typename... Ts
>
auto Property<
    OutRef,
    Type,
    meta::List<Tags...>
>::load(const Ts&... load_from) const -> detail::SchemaResult<typename Property::in_type>
// clang-format on
{
    Opt<typename Type::in_type> base;

    bool satisfied = false;

    // for each loader
    auto load_res = meta::for_each_until<const Ts&...>(
        load_from..., [&](const auto& load_from) -> detail::SchemaResult<> {
            using T = std::remove_cvref_t<decltype(load_from)>;

            // satisfied? do nothing
            if (satisfied) {
                return ok();
            }

            // otherwise load value
            auto load_res = Loader<T>::for_property(*this, load_from);
            if (!load_res.ok()) {
                return err(load_res);
            }

            // value is an std::optional
            const auto& value = load_res.unwrap();

            // no value? move on to next loader (no combine step)
            if (!value) {
                return ok();
            }

            // use property's held type to load actual value
            auto val_res = m_type.load(*value);
            if (!val_res.ok()) {
                // record readable id for debugging purposes
                std::string readable_id = Loader<T>::readable_id(*this);

                return detail::schema_err(val_res, std::move(readable_id));
            }

            // first value? set base
            if (!base) {
                base = val_res.unwrap();
            }

            // otherwise, combine with previous
            else {
                Type::combine(*base, val_res.unwrap());
            }

            // check if we're satisfied
            satisfied = Type::satisfied(*base);

            return ok();
        });

    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Property::in_type prop_in{std::move(base)};

    // make sure property itself is valid
    auto check_res = this->check_against_tags(prop_in);
    if (!check_res.ok()) {
        return err(check_res);
    }

    return ok(std::move(prop_in));
}

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename... Tags
>
template <
    typename Tag_
>
auto Property<
    OutRef,
    Type,
    meta::List<Tags...>
>::add_tag(Tag_ tag) {
    // clang-format off
    return typename Property::append_tag<Tag_>{
        std::move(this->m_type),
        std::tuple_cat(
            std::make_tuple(std::move(tag)),
            std::move(this->m_tags)
        )
    };
    // clang-format on
}

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename... Tags
>
auto Property<
    OutRef,
    Type,
    meta::List<Tags...>
>::to_out(typename Property::in_type& base) const -> detail::SchemaResult<typename Property::out_type>
// clang-format off
{
    // no value? try using default value
    if (!base.m_data) {
        auto default_value = get_default_value();

        // if not, error out
        if (!default_value) {
            if constexpr(has_tag<tags::name>) {
                const auto& name = std::get<tags::name>(m_tags).m_name;
                return detail::schema_err("missing required value", name);
            } else {
                return detail::schema_err("missing required value", "[?]");
            }
        }

        return ok(std::move(*default_value));
    }

    // try converting to output type
    auto out_res = m_type.to_out(*base.m_data);
    if (!out_res.ok()) {
        if constexpr(has_tag<tags::name>) {
            const auto& name = std::get<tags::name>(m_tags).m_name;
            return detail::schema_err(out_res, name);
        } else {
            return detail::schema_err(out_res, "[?]");
        }
    }

    return ok(out_res);
}

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename... Tags
>
auto Property<
    OutRef,
    Type,
    meta::List<Tags...>
>::get_default_value() const -> Opt<typename Property::out_type>
// clang-format on
{
    return m_type.get_default_value();
}

// clang-format off
template <
    auto OutRef,
    detail::is_type Type,
    typename... Ts
>
// clang-format on
auto property(Type type, Ts... builders) {
    // clang-format off
    return type.template as_prop<OutRef>(std::move(builders)...);
    // clang-format on
}

// clang-format off
template <
    auto OutRef,
    typename... Ts
>
// clang-format on
auto property(Ts... builders) {
    // clang-format off
    return type<
        decltype(detail::deduce_member_type(OutRef))
    >().template as_prop<OutRef>(std::move(builders)...);
    // clang-format on
}

} // namespace core::schema