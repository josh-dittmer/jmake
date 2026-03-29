#pragma once

#include "array.h"

#include "core/schema/detail/schema_error.h"
#include "core/schema/detail/type_traits/is_type.h"
#include "detail/builder.h"
#include "generic_type.h"

#include <iterator>

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
>::as_array(Ts... builders)
// clang-format on
{
    // clang-format off
    return detail::apply_builders(
        Array<
            Derived
        >{std::move(*static_cast<Derived*>(this))},
        std::move(builders)...
    );
    // clang-format on
}

} // namespace detail

// clang-format off
template <
    typename Type,
    typename... Tags
>
template <
    typename T
>
auto Array<
    Type,
    meta::List<Tags...>
>::load(const T& load_from) const -> detail::SchemaResult<typename Array::in_type>
// clang-format on
{
    auto load_res = Loader<T>::for_array(*this, load_from);
    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Array::in_type result;

    const auto& arr = load_res.unwrap();

    // load array values
    for (std::size_t i = 0; i < arr.size(); i++) {
        auto val_res = m_type.load(arr[i]);
        if (!val_res.ok()) {
            return detail::schema_err(val_res, std::format("[{}]", i));
        }

        result.push_back(val_res.unwrap());
    }

    // make sure array itself is valid
    auto check_res = this->check_against_tags(result);
    if (!check_res.ok()) {
        return err(check_res);
    }

    return ok(result);
}

// clang-format off
template <
    typename Type,
    typename... Tags
>
template <
    typename Tag_
>
auto Array<
    Type,
    meta::List<Tags...>
>::add_tag(Tag_ tag)
// clang-format on
{
    // clang-format off
    return typename Array::append_tag<Tag_>{
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
    typename Type,
    typename... Tags
>
auto Array<
    Type,
    meta::List<Tags...>
>::to_out(Array::in_type& base) const -> detail::SchemaResult<typename Array::out_type>
// clang-format on
{
    typename Array::out_type result;
    result.reserve(base.size());

    for (std::size_t i = 0; i < base.size(); i++) {
        auto& val = base[i];

        auto out_res = m_type.to_out(val);
        if (!out_res.ok()) {
            return detail::schema_err(out_res, std::format("[{}]", i));
        }

        auto& out = out_res.unwrap();

        result.push_back(std::move(out));
    }

    return ok(result);
}

// clang-format off
template <
    typename Type,
    typename... Tags
>
void Array<
    Type,
    meta::List<Tags...>
>::combine(Array::in_type& base, Array::in_type& input)
// clang-format on
{
    // clang-format off
    base.insert(
        base.end(), 
        std::make_move_iterator(input.begin()),
        std::make_move_iterator(input.end())
    );
    // clang-format on
}

// clang-format off
template <
    typename Type,
    typename... Tags
>
auto Array<
    Type,
    meta::List<Tags...>
>::get_default_value() const -> Opt<typename Array::out_type>
// clang-format on
{
    if constexpr (has_tag<tags::empty_as_default>) {
        return typename Array::out_type{};
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
auto array(Type type, Ts... builders) {
    // clang-format off
    return type.as_array(std::move(builders)...);
    // clang-format on
}

// clang-format off
template <
    typename T,
    typename... Ts
>
// clang-format on
auto array(Ts... builders) {
    // clang-format off
    return GenericType<T>{}.as_array(std::move(builders)...);
    // clang-format on
}

} // namespace core::schema