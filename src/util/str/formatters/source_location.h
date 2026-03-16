#pragma once

#include <format>
#include <source_location>

template <> struct std::formatter<std::source_location> {
    static constexpr auto parse(std::format_parse_context& ctx);
    static constexpr auto format(const std::source_location& location,
                                 std::format_context& ctx);

  private:
    static constexpr std::string_view strip_filename(std::string_view path);
};

#include "source_location.tpp"