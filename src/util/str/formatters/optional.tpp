#pragma once

#include "optional.h"

template <typename T>
    requires std::formattable<T, char>
constexpr auto
std::formatter<std::optional<T>>::parse(std::format_parse_context& ctx) {
    const auto* it = ctx.begin();
    const auto* const end = ctx.end();

    if (it != end && *it == ':') {
        ctx.advance_to(++it);
    }

    return value.parse(ctx);
}

template <typename T>
    requires std::formattable<T, char>
template <typename FormatContext>
constexpr auto
std::formatter<std::optional<T>>::format(const std::optional<T>& in,
                                         FormatContext& ctx) const {
    if (!in) {
        return std::format_to(ctx.out(), "()");
    }

    return value.format(*in, ctx);
}
