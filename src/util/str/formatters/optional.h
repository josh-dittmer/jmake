#pragma once

#include <format>
#include <optional>

template <typename T>
    requires std::formattable<T, char>
struct std::formatter<std::optional<T>> {
    std::formatter<T> value;

    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const std::optional<T>& in, FormatContext& ctx) const;
};

#include "optional.tpp"