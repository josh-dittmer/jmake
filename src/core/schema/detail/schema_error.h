#pragma once

#include "util/result/result.h"

#include <format>
#include <string>
#include <vector>

namespace core::schema::detail {

struct SchemaError {
    explicit SchemaError(std::string msg) : m_msg(std::move(msg)) {}

    std::string m_msg;
    std::vector<std::string> m_bt;

    void add_bt(const std::string& bt) { m_bt.push_back(bt); }
};

template <typename T = None> //
using SchemaResult = Result<T, Error<SchemaError>>;

} // namespace core::schema::detail

// formatter for schema error
template <> struct std::formatter<core::schema::detail::SchemaError> {
    static constexpr auto parse(std::format_parse_context& ctx);
    static constexpr auto
    format(const core::schema::detail::SchemaError& schema_error,
           std::format_context& ctx);
};

#include "schema_error.tpp"