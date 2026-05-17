#pragma once

#include "error.h"

#include <util/str/formatters/source_location.h>

template <typename T>
constexpr auto std::formatter<Error<T>>::parse(std::format_parse_context& ctx) {
    const auto* iter = ctx.begin();

    if (iter != ctx.end() && *iter != '}') {
        switch (*iter) {
        case 't':
            m_mode = Mode::TRACE;
            break;
        case 'f':
            m_mode = Mode::FILE;
            break;
        case 'm':
            m_mode = Mode::MIN;
            break;
        default:
            throw std::format_error("bad format specifier for Error");
        }

        ++iter;
    }

    return iter;
}

template <typename T>
constexpr auto
std::formatter<Error<T>>::format(const Error<T>& error,
                                 std::format_context& ctx) const {
    switch (m_mode) {
    case Mode::TRACE:
        return format_trace(error, ctx);
    case Mode::FILE:
        return format_file(error, ctx);
    case Mode::MIN:
    default:
        return format_min(error, ctx);
    }
}

template <typename T>
constexpr auto
std::formatter<Error<T>>::format_trace(const Error<T>& error,
                                       std::format_context& ctx) const {
    auto out = std::format_to(ctx.out(), "\n\t[START CALLSTACK]");

    for (int i = error.m_bt.size() - 1; i >= 1; i--) {
        out = std::format_to(out, "\n\t\t\342\224\234 {}", error.m_bt[i]);
    }

    out = std::format_to(out, "\n\t\t\342\224\224 {}", error.m_bt[0]);

    return std::format_to(out, " -> [{}]\n\t[END CALLSTACK]", error.m_data);
}

template <typename T>
constexpr auto
std::formatter<Error<T>>::format_file(const Error<T>& error,
                                      std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{} -> [{}]", error.m_bt[0], error.m_data);
}

template <typename T>
constexpr auto
std::formatter<Error<T>>::format_min(const Error<T>& error,
                                     std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", error.m_data);
}
