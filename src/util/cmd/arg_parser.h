#pragma once

#include "util/common_types/result/result.h"

#include <concepts>
#include <string>

namespace util::cmd {

template <typename T> //
struct arg_parser {
    static Result<T> parse(const std::string& value);
};

// some basic parsers

template <> struct arg_parser<std::string> {
    static Result<std::string> parse(const std::string& value);
};

template <std::integral T>
    requires(!std::same_as<T, bool>)
struct arg_parser<T> {
    static Result<T> parse(const std::string& value);
};

template <std::floating_point T> struct arg_parser<T> {
    static Result<T> parse(const std::string& value);
};

} // namespace util::cmd

#include "arg_parser.tpp"