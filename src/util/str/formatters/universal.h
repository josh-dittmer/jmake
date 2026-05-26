#pragma once

#include <format>

template <typename T> struct universal_formatter {
    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const T& in, FormatContext& ctx) const;
};

#include "universal.tpp"