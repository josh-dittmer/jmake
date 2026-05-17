#pragma once

#include "glaze_loader.h"

#include "util/meta/expand.h"
#include "util/meta/nsdm.h"

#include <meta>

namespace schema::detail {

template <std::meta::info R> //
consteval auto create_glz_object() {
    std::vector<std::meta::info> args;

    // clang-format off
    [: util::meta::expand(util::meta::nsdm_of(R)) :] >> [&]<auto member> {
        auto id = get_tag<member, json_id>();

        if (id) {
            // TODO: remove [[maybe_unused]] when compiler bug is fixed
            [[maybe_unused]] constexpr auto mem_ref = &[: member :];

            args.push_back(std::meta::reflect_constant_string(id->m_str));
            args.push_back(^^mem_ref);
        }
    };
    // clang-format on

    return std::meta::substitute(^^glz::object, args);
}

} // namespace schema::detail