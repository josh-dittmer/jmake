#pragma once

#include "optional.h"

template <typename T>
constexpr auto
std::formatter<std::optional<T>>::parse(std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
template <typename FormatContext>
constexpr auto
std::formatter<std::optional<T>>::format(const std::optional<T>& in,
                                         FormatContext& ctx) const {
    if (!in) {
        return std::format_to(ctx.out(), "()");
    }

    return std::format_to(ctx.out(), "{}", *in);
}