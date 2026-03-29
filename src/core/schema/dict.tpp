#pragma once

#include "dict.h"

#include "detail/builder.h"
#include "generic_type.h"

namespace core::schema {

namespace detail {

// implementation for IsType::as_array(), declared in
// detail/type_traits/is_type.h

// clang-format off
template <
    typename Derived
>
template <
    typename... Ts
>
auto IsType<
    Derived 
>::as_dict(Ts... builders)
// clang-format on
{
    // clang-format off
    return detail::apply_builders(
        Dict<
            Derived
        >{std::move(*static_cast<Derived*>(this))},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace detail

// clang-format off
template <
    typename ValueType,
    typename... Tags
>
template <
    typename T
>
auto Dict<
    ValueType,
    meta::List<Tags...>
>::load(const T& load_from) const -> detail::SchemaResult<typename Dict::in_type>
// clang-format on
{
    auto load_res = Loader<T>::for_dict(*this, load_from);
    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Dict::in_type result;

    // load dict values
    for (const auto& [key, value] : load_res.unwrap()) {
        auto val_res = m_type.load(value);
        if (!val_res.ok()) {
            return detail::schema_err(val_res, key);
        }

        result.insert(std::make_pair(key, val_res.unwrap()));
    }

    // make sure dict itself is valid
    auto check_res = this->check_against_tags(result);
    if (!check_res.ok()) {
        return err(check_res);
    }

    return ok(result);
}

// clang-format off
template <
    typename ValueType,
    typename... Tags
>
template <
    typename Tag_
>
auto Dict<
    ValueType,
    meta::List<Tags...>
>::add_tag(Tag_ tag)
// clang-format on
{
    // clang-format off
    return typename Dict::append_tag<Tag_>{
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
    typename ValueType,
    typename... Tags
>
void Dict<
    ValueType,
    meta::List<Tags...>
>::combine(Dict::in_type& base, Dict::in_type& input)
// clang-format on
{
    for (auto& [key, val] : input) {
        if (base.contains(key)) {
            ValueType::combine(base[key], val);
        }

        else {
            base.insert({key, std::move(val)});
        }
    }
}

// clang-format off
template <
    typename ValueType,
    typename... Tags
>
auto Dict<
    ValueType,
    meta::List<Tags...>
>::to_out(Dict::in_type& base) const -> detail::SchemaResult<typename Dict::out_type>
// clang-format on
{
    typename Dict::out_type result;

    for (auto& [key, val] : base) {
        auto out_res = m_type.to_out(val);
        if (!out_res.ok()) {
            return detail::schema_err(out_res, key);
        }

        auto& out = out_res.unwrap();

        result.insert({key, std::move(out)});
    }

    return ok(std::move(result));
}

// clang-format off
template <
    typename ValueType,
    typename... Tags
>
auto Dict<
    ValueType,
    meta::List<Tags...>
>::get_default_value() const -> Opt<typename Dict::out_type>
// clang-format on
{
    if constexpr (has_tag<tags::empty_as_default>) {
        return typename Dict::out_type{};
    }

    else {
        return std::nullopt;
    }
}

// clang-format off
template <
    detail::is_type Type,
    typename... Ts
>
// clang-format on
auto dict(Type type, Ts... builders) {
    // clang-format off
    return type.as_dict(std::move(builders)...);
    // clang-format on
}

// clang-format off
template <
    typename T,
    typename... Ts
>
// clang-format on
auto dict(Ts... builders) {
    // clang-format off
    return GenericType<T>{}.as_dict(std::move(builders)...);
    // clang-format on
}

} // namespace core::schema