#pragma once

#include "arg_parser.h"

#include <charconv>
#include <meta>
#include <string>

namespace util::cmd {

template <typename T>
inline Result<T> arg_parser<T>::parse(const std::string&) {
    return err(std::format("unimplemented parser for {}",
                           std::meta::display_string_of(^^T)));
}

inline Result<std::string>
arg_parser<std::string>::parse(const std::string& value) {
    return value;
}

template <std::integral T>
    requires(!std::same_as<T, bool>)
Result<T> arg_parser<T>::parse(const std::string& value) {
    T parsed{};

    const char* const first = value.data();
    const char* const last = first + value.size();

    const auto [ptr, ec] = std::from_chars(first, last, parsed);

    if (ec != std::errc{} || ptr != last) {
        return err("invalid integer");
    }

    return parsed;
}

template <std::floating_point T>
Result<T> arg_parser<T>::parse(const std::string& value) {
    T parsed{};

    const char* const first = value.data();
    const char* const last = first + value.size();

    const auto [ptr, ec] = std::from_chars(first, last, parsed);

    if (ec != std::errc{} || ptr != last) {
        return err("invalid floating-point");
    }

    return parsed;
}

} // namespace util::cmd
