#pragma once

#include "array.h"

#include "core/schema/detail/type_traits/is_type.h"
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
detail::SchemaResult<std::vector<typename Type::out_type>> Array<
    Type,
    meta::List<Tags...>
>::load(const T& load_from) const
// clang-format on
{
    auto load_res = Loader<T>::for_array(*this, load_from);
    if (!load_res.ok()) {
        return err(load_res);
    }

    typename Array::out_type result;

    const auto& arr = load_res.unwrap();

    // load array values
    for (std::size_t i = 0; i < arr.size(); i++) {
        auto val_res = m_type.load(arr[i]);
        if (!val_res.ok()) {
            val_res.unwrap_err().get().add_bt(std::format("[{}]", i));
            return err(val_res);
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