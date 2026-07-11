#pragma once

#include "glaze_loader.h"

#include "schema/loaders/common/get_nested_ref.h" // IWYU pragma: keep
#include "schema/loaders/common/key.h"
#include "schema/object.h"
#include "schema/tag.h"
#include "util/meta/nsdm.h" // IWYU pragma: keep

namespace schema::glz::detail {

template <auto... Args>
consteval decltype(::glz::object(Args...)) glz_object_pack() {
    return ::glz::object(Args...);
}

template <std::meta::info First, std::meta::info... Path>
consteval void get_full_key_impl(std::string& key) {
    // clang-format off
    key += common::key_from_member<get_out_member<First>(), rename>();
    ((key += "." + common::key_from_member<get_out_member<Path>(), rename>()), ...);
    // clang-format on
}

template <std::meta::info... Path> consteval auto get_full_key() {
    std::string key{};

    static constexpr auto il = {std::meta::reflect_constant(Path)...};

    // clang-format off
    [:std::meta::substitute(^^get_full_key_impl, 
        il 
        | std::views::reverse
        | std::ranges::to<std::vector>()
    ):](key);
    // clang-format on

    return std::meta::reflect_constant_string(std::string_view{key});
}

/*template <std::meta::info First, std::meta::info... Path>
auto& field_getter_impl(auto& parent) {
    if constexpr (sizeof...(Path) <= 0) {
        // in base case, parent is not an optional
        return parent.m_data.[:First:].get();
    } else {
        auto& p_ref = field_getter_impl<Path...>(parent);
        using p_type = std::remove_reference_t<decltype(p_ref)>::value_type;

        if (!p_ref) {
            // construct default if we don't have it
            p_ref = p_type{};
        }

        return p_ref->m_data.[:First:].get();
    }
}*/

} // namespace schema::glz::detail

template <typename T>
consteval auto glz::meta<schema::detail::in_object<T>>::make_args() {
    using namespace ::schema;
    using namespace ::schema::glz;

    /*using data_t = in_object_t::data_type;
    return make_args_impl<data_t>();*/

    static constexpr auto flattened = in_t::flatten();

    std::vector<std::meta::info> args;

    template for (constexpr auto p : flattened) {
        constexpr auto path = [:p:];
        constexpr auto mem = get_out_member<path[0]>();

        // skip if member is disabled
        if constexpr (has_tag<mem, disable>() || has_tag<^^T, disable>()) {
            continue;
        }

        // get glz key
        constexpr auto key = std::meta::reflect_constant_string(
            common::key_from_member<mem, ::schema::glz::rename>());

        // get getter fn pointer
        // clang-format off
        constexpr auto g_fn = 
            &[:std::meta::substitute(^^meta::field_getter,
                path
                | std::views::transform(std::meta::reflect_constant<std::meta::info>)
                | std::ranges::to<std::vector>()
            ):];
        // clang-format on

        args.push_back(key);
        args.push_back(^^g_fn);
    }

    return args;
}

/*template <typename T>
template <std::meta::info Member, std::meta::info... Path>
consteval auto
glz::meta<::schema::detail::in_object<T>>::make_args_from_member() {
    using p_t [[maybe_unused]] = [:std::meta::type_of(Member):];
    using pd_a [[maybe_unused]] = typename p_t::data_type;
    using pd_t = [:std::meta::dealias(^^pd_a):];
    using data_t = typename pd_t::data_type;

    return make_args_impl<data_t, Member, Path...>();
}

template <typename T>
template <typename Data, std::meta::info... Path>
consteval auto glz::meta<schema::detail::in_object<T>>::make_args_impl() {
    using namespace ::schema::glz::detail;
    using namespace ::schema::detail;

    std::vector<std::meta::info> args;

    template for (constexpr auto mem : util::meta::nsdm_arr(^^Data)) {
        // get member type (Property<...>)
        using p_t [[maybe_unused]] = [:std::meta::type_of(mem):];
        using pd_t = typename p_t::data_type;

        constexpr auto pd_info = std::meta::dealias(^^pd_t);

        // if property itself is an in_object, recurse
        if constexpr (util::meta::template_of<^^in_object, pd_info>) {
            auto n = make_args_from_member<mem, Path...>();
            args.insert(args.end(), n.begin(), n.end());
        }

        // add arg to list
        args.push_back(get_full_key<mem, Path...>());
        args.push_back(field_getter<mem, Path...>());
    }

    return args;
}*/

template <typename T>
consteval auto glz::meta<schema::detail::in_object<T>>::create_glz_object() {
    using namespace ::schema::glz::detail;

    constexpr auto pack_fn =
        std::meta::substitute(^^glz_object_pack, make_args());

    return [:pack_fn:]();
}

/*template <typename T>
template <std::meta::info... Path>
consteval auto glz::meta<schema::detail::in_object<T>>::field_getter() {
    using namespace ::schema::glz::detail;

    [[maybe_unused]] constexpr auto getter = [](in_object_t& self) -> auto& {
        return field_getter_impl<Path...>(self);
    };

    return ^^getter;
}*/