#pragma once

#include "glaze_loader.h"

#include "util/meta/nsdm.h" // IWYU pragma: keep

#include <meta>

namespace schema::glz::detail {

template <auto... Args>
consteval decltype(::glz::object(Args...)) glz_object_pack() {
    return ::glz::object(Args...);
}

template <auto Mem> consteval auto field_key() {
    using mem_t = [:std::meta::type_of(Mem):];
    constexpr auto obj_mem = mem_t::member;

    constexpr auto id_tag = get_tag<obj_mem, id>();
    if constexpr (id_tag) {
        return std::meta::reflect_constant(id_tag->m_str);
    }

    return std::meta::reflect_constant_string(std::meta::identifier_of(Mem));
}

} // namespace schema::glz::detail

template <typename T>
consteval auto glz::meta<::schema::detail::in_object<T>>::create_glz_object() {
    using namespace ::schema::glz::detail;

    constexpr auto pack_fn =
        std::meta::substitute(^^glz_object_pack, create_args());

    return [:pack_fn:]();
}

template <typename T>
consteval auto glz::meta<::schema::detail::in_object<T>>::create_args() {
    using namespace ::schema::glz::detail;

    std::vector<std::meta::info> args;

    template for (constexpr auto mem : util::meta::nsdm_arr(^^data_t)) {
        args.push_back(field_key<mem>());
        args.push_back(field_getter<mem>());
    }

    return args;
}

template <typename T>
template <std::meta::info Member>
consteval auto glz::meta<::schema::detail::in_object<T>>::field_getter() {
    [[maybe_unused]] constexpr auto getter = [](in_object_t& self) -> auto& {
        //     - in_object<T>
        //     |    - in_object<T>::data_type
        //     |    |      - Property<...>
        //     |    |      |          - Property<...>::data_type
        return self.m_data.[:Member:].get();
    };

    return ^^getter;
}