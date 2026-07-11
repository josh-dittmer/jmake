#pragma once

#include "cmd_loader.h"

#include "schema/loaders/common/get_nested_ref.h"
#include "schema/loaders/common/key.h"
#include "schema/object.h"
#include "schema/tag.h"
#include "util/cmd/arg_parser.h"
#include "util/cmd/cmd_args.h"

#include <meta>

namespace schema::cmd {

template <typename T> //
consteval auto cmd_loader<T>::create_flag_array() {
    static constexpr auto flattened = in_t::flatten();

    using pair_t = std::pair<const char*, setter_fn>;
    std::vector<pair_t> pairs;

    template for (constexpr auto p : flattened) {
        constexpr auto path = [:p:];
        constexpr auto mem = get_out_member<path[0]>();

        // skip if member is disabled
        if constexpr (has_tag<mem, disable>() || has_tag<^^T, disable>()) {
            continue;
        }

        // get cmd flag
        const char* flag = std::define_static_string(
            common::key_from_member<mem, rename, detail::flag_transformer>());

        // get setter fn pointer
        // clang-format off
        constexpr setter_fn s_fn =
            &[:std::meta::substitute(^^cmd_loader::field_setter, 
                path
                | std::views::transform(std::meta::reflect_constant<std::meta::info>)
                | std::ranges::to<std::vector>()
            ):];
        // clang-format on

        pairs.emplace_back(flag, s_fn);
    }

    return pairs;
}

template <typename T> //
auto cmd_loader<T>::create_flag_map() {
    static constexpr auto arr = std::define_static_array(create_flag_array());

    std::map<std::string_view, setter_fn> flag_map;

    template for (constexpr auto p : arr) {
        constexpr std::string_view flag{p.first};
        flag_map.insert(std::make_pair(flag, p.second));
    }

    return flag_map;
}

template <typename T>
template <std::meta::info... Path> //
Result<> cmd_loader<T>::field_setter(in_t& obj, const std::string& value) {
    auto& ref = common::get_nested_ref<Path...>(obj);

    // ref will be an std::optional
    using value_t = typename std::remove_reference_t<decltype(ref)>::value_type;

    ref = HUH(util::cmd::arg_parser<value_t>::parse(value));

    return ok();
}

template <typename T>
Result<schema::detail::in_object<T>>
cmd_loader<T>::from(util::cmd::Args& args) {
    static auto flag_map = create_flag_map();

    schema::detail::in_object<T> result{};

    for (const auto& a : args.get()) {
        const auto& arg = a.first;
        const auto& val = a.second;

        if (!val) {
            continue;
        }

        auto mit = flag_map.find(arg);
        if (mit == flag_map.end()) {
            continue;
        }

        HUH(mit->second(result, *val));
    }

    return result;
}

} // namespace schema::cmd