#pragma once

#include "object.h"

#include "schema/detail/in_type.h"
#include "schema/rule.h"
#include "util/meta/expand.h"
#include "util/meta/nsdm.h"

#include <meta>
#include <optional>
#include <print>
#include <ranges>

namespace schema {

namespace detail {

// clang-format off
template <
    std::meta::info ClassType, 
    std::meta::info Object
>
// clang-format off
consteval auto create_in_type() {
    std::vector<std::meta::info> mem_specs;

    // clang-format off
    [: util::meta::expand(util::meta::nsdm_of(Object)) :] >> [&]<auto member> {        
        using m_type [[maybe_unused]] = [: std::meta::type_of(member) :];

        auto mem_anns = std::meta::annotations_of(member);

        std::vector<std::meta::info> prop_args;
        prop_args.push_back(^^m_type);

        // append annotations
        for (auto a : mem_anns) {
            prop_args.push_back(std::meta::reflect_constant(a));
        }

        mem_specs.push_back(std::meta::data_member_spec(
            std::meta::substitute(^^Property, prop_args),
            { .name = std::meta::identifier_of(member) }
        ));
    };
    // clang-format on

    return std::meta::define_aggregate(ClassType, mem_specs);
}

template <typename T>
    requires std::is_class_v<T>
consteval auto in_type<T>::tie() {
    // TODO: remove [[maybe_unused]] when compiler bug is fixed
    using data_t [[maybe_unused]] = in_type<T>::type;

    std::vector<std::pair<std::meta::info, std::meta::info>> members;

    for (auto [m1, m2] : std::views::zip(util::meta::nsdm_of(^^T),
                                         util::meta::nsdm_of(^^data_t))) {
        members.emplace_back(m1, m2);
    }

    return members;
}

} // namespace detail

// clang-format off
template <
    typename Object,
    typename... Context,
    typename... Ts
> requires std::is_class_v<Object>
// clang-format on
Result<detail::in_type<Object>> //
load(const std::tuple<Context...>& context, Ts&&... load_from) {
    // template for (auto&& l : {load_from...}) { // NOLINT
    //     std::println("{}", l);
    //     std::println("{}", l);
    // }

    using in_t = detail::in_type<Object>;
    using in_data_t [[maybe_unused]] = in_t::type;

    // initialize empty result
    auto output = in_t{};

    // for error checking
    Result<> result = ok();

    // clang-format off
    [: util::meta::expand(util::meta::nsdm_of(^^in_data_t)) :] >> [&]<auto member> {
        /*if (!result) {
            return;
        }*/

        std::println("{}:", std::meta::display_string_of(std::meta::type_of(member)));

        /*[: util::meta::expand(std::meta::annotations_of(member)) :] >> [&]<auto ann> {
            constexpr auto a_info = std::meta::type_of(ann);
            std::println("\t{}", std::meta::identifier_of(a_info));
        };*/
        
        // load each property
        /*auto load_res = load<og_mem, in_mem>(
            context,
            std::forward<Ts>(load_from)...
        );

        // check result
        if (!load_res) {
            result = err(load_res);
        }

        // all is good? assign
        output.[: in_mem :] = std::move(*load_res);*/
    };
    // clang-format on

    if (!result) {
        return err(result);
    }

    // make sure object itself is valid

    return output;
}

// clang-format off
template <
typename Object,
typename... Ts
> requires std::is_class_v<Object>
// clang-format on
Result<detail::in_type<Object>> load(Ts&&... load_from) {
    return load<Object>({}, std::forward<Ts>(load_from)...);
}

} // namespace schema