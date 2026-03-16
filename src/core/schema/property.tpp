#pragma once

#include "core/schema/detail/schema_error.h"
#include "property.h"

#include "detail/builder.h"
#include "generic_type.h"
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
detail::SchemaResult<typename Type::out_type> Property<
    OutRef,
    Type,
    meta::List<Tags...>
>::load(const Ts&... load_from) const
// clang-format on
{
    Opt<typename Type::out_type> result;

    // for each loader
    auto load_res = meta::for_each_until<const Ts&...>(
        load_from..., [&](const auto& load_from) -> detail::SchemaResult<> {
            using T = std::remove_cvref_t<decltype(load_from)>;

            // skip if we already have a value
            if (result.has_value()) {
                return ok();
            }

            // otherwise load value
            auto load_res = Loader<T>::for_property(*this, load_from);
            if (!load_res.ok()) {
                return err(load_res);
            }

            const auto& value = load_res.unwrap();

            // no value? move on to next loader (without setting result)
            if (!value) {
                return ok();
            }

            // load actual value
            auto val_res = m_type.load((*value).get());
            if (!val_res.ok()) {
                // record readable id for error handling purposes
                std::string readable_id = Loader<T>::readable_id(*this);
                val_res.unwrap_err().get().add_bt(std::move(readable_id));

                return err(val_res);
            }

            // all is good? set result
            result = val_res.unwrap();

            return ok();
        });

    if (!load_res.ok()) {
        return err(load_res);
    }

    // is this property optional? if so we can unconditionally return
    if constexpr (is_optional) {
        // if we have value, make sure property itself is valid
        if (result) {
            auto check_res = this->check_against_tags(*result);
            if (!check_res.ok()) {
                return err(check_res);
            }

            return ok(std::move(*result));
        }

        return ok(std::nullopt);
    }

    // otherwise make sure we have a value
    else {
        if (!result) {
            // clang-format off
            return err(detail::SchemaError{
                "missing required value"
            });
            // clang-format on
        }

        // make sure property itself is valid
        auto check_res = this->check_against_tags(*result);
        if (!check_res.ok()) {
            return err(check_res);
        }

        return ok(std::move(*result));
    }
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
    return GenericType<
        decltype(detail::deduce_member_type(OutRef))
    >{}.template as_prop<OutRef>(std::move(builders)...);
    // clang-format on
}

} // namespace core::schema