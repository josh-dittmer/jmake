#pragma once

#include "schema_error.h"

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