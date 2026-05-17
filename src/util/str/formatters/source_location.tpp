#pragma once

#include "source_location.h"

constexpr auto
std::formatter<std::source_location>::parse(std::format_parse_context& ctx) {
    return ctx.begin();
}

constexpr auto std::formatter<std::source_location>::format(
    const std::source_location& location, std::format_context& ctx) {
    std::string_view file_name = strip_filename(location.file_name());

    return std::format_to(ctx.out(), "{}({}:{})", file_name, location.line(),
                          location.column());
}

constexpr std::string_view
std::formatter<std::source_location>::strip_filename(std::string_view path) {
    std::string_view::size_type last_slash = path.find_last_of("/\\");

    if (last_slash != std::string_view::npos) {
        return path.substr(last_slash + 1);
    }

    return path;
}