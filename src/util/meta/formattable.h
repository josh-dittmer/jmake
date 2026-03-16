#pragma once

#include <format>

namespace meta {

template <typename T>
concept formattable = requires(T t) { std::format("{}", t); };

template <typename T> inline auto fmt_or(T value, const std::string& str) {
    if constexpr (formattable<T>) {
        return std::format("{}", value);
    } else {
        return str;
    }
}

} // namespace meta