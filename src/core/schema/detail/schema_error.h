#pragma once

#include "util/result/result.h"

#include <format>
#include <string>
#include <vector>

namespace core::schema::detail {

struct SchemaError {
    std::string m_msg;
    std::vector<std::string> m_bt;
};

template <typename T = None> //
using SchemaResult = Result<T, Error<SchemaError>>;

// creates a new SchemaError
extern constexpr auto
schema_err(std::string msg, std::string bt,
           std::source_location location = std::source_location::current());

// creates a new SchemaError with an empty backtrace
template <typename T, typename U>
extern constexpr auto
schema_err(Result<T, Error<U>>& res,
           std::source_location location = std::source_location::current());

// creates a new SchemaError
template <typename T, typename U>
extern constexpr auto
schema_err(Result<T, Error<U>>& res, std::string bt,
           std::source_location location = std::source_location::current());

// adds to backtrace of existing SchemaError
template <typename T>
extern constexpr auto
schema_err(SchemaResult<T>& res, std::string bt,
           std::source_location location = std::source_location::current());

} // namespace core::schema::detail

// formatter for schema error
template <> struct std::formatter<core::schema::detail::SchemaError> {
    static constexpr auto parse(std::format_parse_context& ctx);
    static constexpr auto
    format(const core::schema::detail::SchemaError& schema_error,
           std::format_context& ctx);
};

#include "schema_error.tpp"