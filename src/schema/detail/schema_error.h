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
    explicit ErrorInfo(std::string msg) //
        : m_msg(std::move(msg)), m_bt({}) {}

    explicit ErrorInfo(std::string msg, std::vector<Trace> bt)
        : m_msg(std::move(msg)), m_bt(std::move(bt)) {}

    std::string m_msg;
    std::vector<Trace> m_bt;
};

using SchemaError = Error<ErrorInfo>;

template <typename T = None> using SchemaResult = Result<T, SchemaError>;

template <typename T>
extern SchemaResult<T>& push_bt(SchemaResult<T>& result, Trace trace);

} // namespace schema::detail

// formatter for schema error
template <> struct std::formatter<schema::detail::ErrorInfo> {
    static constexpr auto parse(std::format_parse_context& ctx);

    static constexpr auto format(const schema::detail::ErrorInfo& error_info,
                                 std::format_context& ctx);
};

#include "schema_error.tpp"