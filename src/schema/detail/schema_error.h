#pragma once

#include "util/common_types/result/result.h"

#include <string>
#include <variant>
#include <vector>

namespace schema::detail {

struct GenericTypeTrace {};

struct ArrayTrace {
    std::size_t m_index;
};

struct DictTrace {
    std::string m_key;
};

struct PropertyTrace {
    std::string m_key;
};

struct ObjectTrace {};

// clang-format off
using Trace = std::variant<
    GenericTypeTrace,
    ArrayTrace,
    DictTrace,
    PropertyTrace,
    ObjectTrace
>;
// clang-format on

struct ErrorInfo {
    ErrorInfo(std::string msg, std::vector<Trace> bt)
        : m_msg(std::move(msg)), m_bt(std::move(bt)) {}

    explicit ErrorInfo(std::string msg) //
        : m_msg(std::move(msg)), m_bt({}) {}

    std::string m_msg;
    std::vector<Trace> m_bt;
};

using SchemaError = Error<ErrorInfo>;

template <typename T> using SchemaResult = Result<T, SchemaError>;

} // namespace schema::detail

// formatter for schema error
template <> struct std::formatter<schema::detail::SchemaError> {
    static constexpr auto parse(std::format_parse_context& ctx);

    static constexpr auto
    format(const schema::detail::SchemaError& schema_error,
           std::format_context& ctx);
};

#include "schema_error.tpp"