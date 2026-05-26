#pragma once

#include "universal.h"

#include "util/meta/nsdm.h" // IWYU pragma: keep

#include <meta>

template <typename T>
constexpr auto universal_formatter<T>::parse(std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
template <typename FormatContext>
constexpr auto universal_formatter<T>::format(const T& in,
                                              FormatContext& ctx) const {
    constexpr auto identifier = std::meta::has_identifier(^^T)
                                    ? std::meta::identifier_of(^^T)
                                    : "(unnamed-type)";

    auto out = std::format_to(ctx.out(), "{}{{", identifier);

    auto delim = [&, first = true]() mutable {
        if (!first) {
            *out++ = ',';
            *out++ = ' ';
        }
        first = false;
    };

    // template for (constexpr auto base :
    //               define_static_array(bases_of(^^T, ctx))) {
    //     delim();
    //     out = std::format_to(out, "{}", (typename[:type_of(base):]
    //     const&)(t));
    // }

    template for (constexpr auto mem : util::meta::nsdm_arr(^^T)) {
        delim();
        std::string_view mem_label = std::meta::has_identifier(mem)
                                         ? std::meta::identifier_of(mem)
                                         : "(unnamed-member)";

        out = std::format_to(out, ".{}={}", mem_label, in.[:mem:]);
    }

    *out++ = '}';

    return out;
}