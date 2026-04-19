#pragma once

#include "util/common_types/optional/optional.h"
#include "util/common_types/result/result.h"

#include <charconv>
#include <concepts>

namespace util::str {

template <typename T> struct Parser;

// clang-format off
template <typename T>
concept parsable = requires {
    { Parser<T>::parse(std::declval<std::string>()) } -> std::same_as<Result<T>>;
};
// clang-format on

// some basic parsers
template <typename T> struct Parser<std::optional<T>> {
    static Result<std::optional<T>> parse(std::string str) {
        auto p_res = Parser<T>::parse(std::move(str));
        if (!p_res.ok()) {
            return err(p_res);
        }

        return ok(opt(p_res.unwrap()));
    }
};

template <> struct Parser<std::string> {
    static Result<std::string> parse(std::string str) {
        return ok(std::move(str));
    }
};

template <std::integral T>
    requires(!std::same_as<T, bool>)
struct Parser<T> {
    static Result<T> parse(std::string str) {
        T value{};

        const char* const first = str.data();
        const char* const last = first + str.size();

        const auto [ptr, ec] = std::from_chars(first, last, value);

        if (ec != std::errc{} || ptr != last) {
            return err("invalid integer");
        }

        return ok(value);
    }
};

template <std::floating_point T> struct Parser<T> {
    static Result<T> parse(std::string str) {
        T value{};

        const char* const first = str.data();
        const char* const last = first + str.size();

        const auto [ptr, ec] = std::from_chars(first, last, value);

        if (ec != std::errc{} || ptr != last) {
            return err("invalid floating-point");
        }

        return ok(value);
    }
};

} // namespace util::str