#pragma once

#include "schema_error.h"

#include "util/common_types/overloads.h"

#include <ranges>

namespace schema::detail {

template <typename T>
SchemaResult<T>& push_bt(SchemaResult<T>& result, Trace trace) {
    result.error().get().m_bt.emplace_back(std::move(trace));
    return result;
}

} // namespace schema::detail

constexpr auto std::formatter<schema::detail::ErrorInfo>::parse(
    std::format_parse_context& ctx) {
    return ctx.begin();
}

constexpr auto std::formatter<schema::detail::ErrorInfo>::format(
    const schema::detail::ErrorInfo& error_info, std::format_context& ctx) {
    using namespace schema::detail;

    auto out = ctx.out();

    for (const auto& t : std::ranges::reverse_view(error_info.m_bt)) {
        // clang-format off
        std::visit(overloads{
            [](const auto&) {},
            [&](const GenericTypeTrace&)   {},
            [&](const ArrayTrace& t)       { std::format_to(out, "[{}]", t.m_index); },
            [&](const DictTrace& t)        { std::format_to(out, ".{}", t.m_key);},
            [&](const PropertyTrace& t)    { std::format_to(out, ".{}", t.m_key);},
            [&](const ObjectTrace&)        {}
        }, t);
        // clang-format on
    }

    return std::format_to(ctx.out(), ": {}", error_info.m_msg);
}