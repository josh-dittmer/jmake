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
    typename KeyType,
    typename... Tags
>
template <
    typename T
>
detail::SchemaResult<std::map<KeyType, typename ValueType::out_type>> Dict<
    ValueType,
    KeyType,
    meta::List<Tags...>
>::load(const T& load_from) const
// clang-format on
{
    auto load_res = Loader<T>::for_dict(*this, load_from);
    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Dict::out_type result;

    // load dict values
    for (const auto& [key, value] : load_res.unwrap()) {
        auto val_res = m_type.load(value);
        if (!val_res.ok()) {
            val_res.unwrap_err().get().add_bt(key);
            return err(val_res);
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
    typename KeyType,
    typename... Tags
>
template <
    typename Tag_
>
auto Dict<
    ValueType,
    KeyType,
    meta::List<Tags...>
>::add_tag(Tag_ tag) {
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