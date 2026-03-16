#pragma once

#include "generic_type.h"

#include "detail/builder.h"
#include "loader/loader.h"
#include "util/meta/formattable.h"

namespace core::schema {

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
template <
    typename T
>
detail::SchemaResult<Type> GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::load(const T& load_from) const
// clang-format on
{
    Result<Type> load_res = Loader<T>::for_generic_type(*this, load_from);
    if (!load_res.ok()) {
        return err(load_res);
    }

    return load(load_res.unwrap());
}
// clang-format on

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
detail::SchemaResult<Type> GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::load(const Type& load_from) const
// clang-format on
{
    if constexpr (sizeof...(Tags) <= 0) {
        return ok(load_from);
    }

    auto check_res = this->check_against_tags(load_from);
    if (!check_res.ok()) {
        return err(check_res);
    }

    return ok(load_from);
}

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
bool GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::has_value(const Type& value) const
// clang-format on
{
    return m_value_set.contains(value);
}

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
template <
    typename Tag_
>
auto GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::add_tag(Tag_ tag)
// clang-format on
{
    // clang-format off
    return typename GenericType::append_tag<Tag_>{
        std::tuple_cat(
            std::make_tuple(std::move(tag)),
            std::move(this->m_tags)
        ),
        std::move(this->m_default_value),
        std::move(this->m_values),
        std::move(this->m_value_set)
    };
    // clang-format on
}

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
template <
    is_default_value_init ValueInit_
>
auto GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::set_default_value(ValueInit_ value_init)
// clang-format on
{
    // adds to set and stores ref in value
    auto value = value_init.to_value(m_value_set);

    // clang-format off
    return typename GenericType::rebind_default_value<decltype(value)>{
        std::move(this->m_tags),
        std::move(value),
        std::move(this->m_values),
        std::move(this->m_value_set)
    };
    // clang-format on
}

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
template <
    is_value_init ValueInit_
>
auto GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
>::add_value(ValueInit_ value_init)
// clang-format on
{
    // adds to set and stores ref in value
    auto value = value_init.to_value(m_value_set);

    // clang-format off
    return typename GenericType::append_value<decltype(value)>{
        std::move(this->m_tags),
        std::move(this->m_default_value),
        std::tuple_cat(
            std::make_tuple(std::move(value)),
            std::move(this->m_values)
        ),
        std::move(this->m_value_set)
    };
    // clang-format on
}

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename... Ts
>
// clang-format on
auto type(Ts... builders) {
    // clang-format off
    return detail::apply_builders(
        GenericType<Type, TypeComp>{},
        std::move(builders)...
    );
    // clang-format on
}

namespace tags {

template <is_generic_type T>
Result<> restricted::check(const T& type, const T::out_type& value) const {
    if (!type.has_value(value)) {
        return err("disallowed value: " + meta::fmt_or(value, "[?]"));
    }

    return ok();
}

template <typename T>
template <is_generic_type U>
Result<> min<T>::check(const U&, const U::out_type& value) const {
    if (value < m_min) {
        return err(std::format("value out of range: {} (min {})",
                               meta::fmt_or(value, "[?]"),
                               meta::fmt_or(m_min, "[?]")));
    }

    return ok();
}

template <typename T>
template <is_generic_type U>
Result<> max<T>::check(const U&, const U::out_type& value) const {
    if (value > m_max) {
        return err(std::format("value out of range: {} (max {})",
                               meta::fmt_or(value, "[?]"),
                               meta::fmt_or(m_max, "[?]")));
    }

    return ok();
}

} // namespace tags

} // namespace core::schema