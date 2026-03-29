#pragma once

#include "schema_error.h"

namespace core::schema::detail {

// creates a new SchemaError
constexpr auto schema_err(std::string msg, std::string bt,
                          std::source_location location) {
    // clang-format off
    return bad(ErrorInitializer{
        .m_data =
            SchemaError{
                .m_msg = std::move(msg),
                .m_bt = {std::move(bt)},
            },
        .m_location = location,
    });
    // clang-format on
}

// creates a new SchemaError with an empty backtrace
template <typename T, typename U>
constexpr auto schema_err(Result<T, Error<U>>& res,
                          std::source_location location) {
    // clang-format off
    return bad(ErrorInitializer{
        .m_data =
            SchemaError{
                .m_msg = std::move(res.unwrap_err().get()),
                .m_bt = {},
            },
        .m_location = location,
    });
    // clang-format on
}

// creates a new SchemaError
template <typename T, typename U>
constexpr auto schema_err(Result<T, Error<U>>& res, std::string bt,
                          std::source_location location) {
    // clang-format off
    return bad(ErrorInitializer{
        .m_data =
            SchemaError{
                .m_msg = std::move(res.unwrap_err().get()),
                .m_bt = {std::move(bt)},
            },
        .m_location = location,
    });
    // clang-format on
}

// adds to backtrace of existing SchemaError
template <typename T>
constexpr auto schema_err(SchemaResult<T>& res, std::string bt,
                          std::source_location location) {
    SchemaError& prev_err = res.unwrap_err().get();
    prev_err.m_bt.push_back(std::move(bt));

    // clang-format off
    return bad(ErrorInitializer{
        .m_data = std::move(prev_err),
        .m_location = location,
    });
    // clang-format on
}

} // namespace core::schema::detail

constexpr auto std::formatter<core::schema::detail::SchemaError>::parse(
    std::format_parse_context& ctx) {
    return ctx.begin();
}

constexpr auto std::formatter<core::schema::detail::SchemaError>::format(
    const core::schema::detail::SchemaError& schema_error,
    std::format_context& ctx) {
    if (schema_error.m_bt.empty()) {
        return std::format_to(ctx.out(), "{}", schema_error.m_msg);
    }

    std::size_t n = schema_error.m_bt.size();

    auto out = ctx.out();

    for (std::size_t i = n - 1; i >= 1; i--) {
        out = std::format_to(out, "{}::", schema_error.m_bt[i]);
    }

    out = std::format_to(ctx.out(), "{}", schema_error.m_bt[0]);

    return std::format_to(out, ": {}", schema_error.m_msg);
}