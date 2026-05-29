#pragma once

#include "universal.h"

#include "util/meta/nsdm.h" // IWYU pragma: keep

namespace uf_detail {

template <std::meta::info R> consteval auto get_identifier() {
    return std::meta::has_identifier(R) ? std::meta::identifier_of(R)
                                        : "(unnamed)";
}

template <typename Out> //
void tabs(Out out, std::size_t num_tabs) {
    for (std::size_t i = 0; i < num_tabs; i++) {
        *out++ = ' ';
        *out++ = ' ';
    }
}

template <typename Out> //
void newline(Out out, std::size_t num_tabs) {
    *out++ = '\n';
    tabs(out, num_tabs);
}

} // namespace uf_detail

template <typename T>
constexpr auto
optional_universal_formatter<T>::parse(std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
template <typename FormatContext>
constexpr auto
optional_universal_formatter<T>::format(const universal_format_wrapper<T>& in,
                                        FormatContext& ctx) const {
    if (!in.m_target) {
        return std::format_to(ctx.out(), "()");
    }

    // clang-format off
    universal_format_wrapper val_wrapper{
        .m_target = *in.m_target, 
        .m_ctx = in.m_ctx
    };
    // clang-format on

    return std::format_to(ctx.out(), "{}", val_wrapper);
}

template <typename T>
constexpr auto
sequence_universal_formatter<T>::parse(std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
template <typename FormatContext>
constexpr auto
sequence_universal_formatter<T>::format(const universal_format_wrapper<T>& in,
                                        FormatContext& ctx) const {
    if (in.m_target.empty()) {
        return std::format_to(ctx.out(), "[]");
    }

    bool indent = in.m_ctx.m_mode == universal_format_context::Mode::NEWLINE;

    auto elem_ctx = in.m_ctx;
    elem_ctx.m_num_tabs++;

    auto out = ctx.out();

    auto delim = [&, first = true]() mutable {
        if (!first) {
            *out++ = ',';
            *out++ = ' ';

            if (indent) {
                uf_detail::newline(out, in.m_ctx.m_num_tabs + 1);
            }
        }
        first = false;
    };

    *out++ = '[';

    if (indent) {
        uf_detail::newline(out, in.m_ctx.m_num_tabs + 1);
    }

    for (const auto& e : in.m_target) {
        delim();

        // clang-format off
        universal_format_wrapper elem_wrapper{
            .m_target = e, 
            .m_ctx = elem_ctx
        };
        // clang-format on

        out = std::format_to(out, "{}", elem_wrapper);
    }

    if (indent) {
        uf_detail::newline(out, in.m_ctx.m_num_tabs);
    }

    *out++ = ']';

    return out;
}

template <typename T>
constexpr auto std::formatter<universal_format_wrapper<T>>::parse(
    std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
template <typename FormatContext>
constexpr auto std::formatter<universal_format_wrapper<T>>::format(
    const universal_format_wrapper<T>& in, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", in.m_target);
}

template <typename T>
    requires universal_formattable<T>
constexpr auto std::formatter<universal_format_wrapper<T>>::parse(
    std::format_parse_context& ctx) {
    return ctx.begin();
}

template <typename T>
    requires universal_formattable<T>
template <typename FormatContext>
constexpr auto
std::formatter<universal_format_wrapper<T>>::format(const type& in,
                                                    FormatContext& ctx) const {
    bool indent = in.m_ctx.m_mode == universal_format_context::Mode::NEWLINE;

    auto out = ctx.out();

    auto delim = [&, first = true]() mutable {
        if (!first) {
            *out++ = ',';
            *out++ = ' ';

            if (indent) {
                uf_detail::newline(out, in.m_ctx.m_num_tabs + 1);
            }
        }
        first = false;
    };

    out = std::format_to(out, "{}{{", uf_detail::get_identifier<^^T>());

    if (indent) {
        uf_detail::newline(out, in.m_ctx.m_num_tabs + 1);
    }

    auto mem_ctx = in.m_ctx;
    mem_ctx.m_num_tabs++;

    template for (constexpr auto mem : util::meta::nsdm_arr(^^T)) {
        delim();

        constexpr auto mem_id = uf_detail::get_identifier<mem>();
        constexpr auto mem_t_info = std::meta::type_of(mem);
        using mem_t = [:mem_t_info:];

        out = std::format_to(out, ".{}=", mem_id);

        // clang-format off
        universal_format_wrapper<mem_t> mem_wrapper{
            .m_target = in.m_target.[:mem:], 
            .m_ctx = mem_ctx
        };
        // clang-format on

        out = std::format_to(out, "{}", mem_wrapper);
    }

    if (indent) {
        uf_detail::newline(out, in.m_ctx.m_num_tabs);
    }

    *out++ = '}';

    return out;
}

template <typename T>
constexpr auto universal_formatter<T>::parse(std::format_parse_context& ctx) {
    const auto* iter = ctx.begin();

    if (iter != ctx.end() && *iter != '}') {
        switch (*iter) {
        case 'n':
            m_ctx.m_mode = universal_format_context::Mode::NEWLINE;
            break;
        case 'm':
            m_ctx.m_mode = universal_format_context::Mode::MIN;
            break;
        default:
            throw std::format_error(
                "bad format specifier for universal_formatter");
        }

        ++iter;
    }

    return iter;
}

template <typename T>
template <typename FormatContext>
constexpr auto universal_formatter<T>::format(const T& in,
                                              FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}",
                          universal_format_wrapper<T>{in, m_ctx});
}