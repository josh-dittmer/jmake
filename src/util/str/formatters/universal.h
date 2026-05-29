#pragma once

#include <format>
#include <meta>

struct universal_format_context {
    enum class Mode : uint8_t { NEWLINE, MIN } m_mode = Mode::MIN;
    std::size_t m_num_tabs = 0;
};

namespace uf_detail {

template <std::meta::info R> //
extern consteval auto get_identifier();

template <typename Out> //
extern void tabs(Out out, std::size_t num_tabs);

template <typename Out> //
extern void newline(Out out, std::size_t num_tabs);

} // namespace uf_detail

template <typename T> struct universal_format_wrapper {
    const T& m_target; // NOLINT
    universal_format_context m_ctx;
};

template <typename T> struct sequence_universal_formatter {
    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const universal_format_wrapper<T>& in,
                          FormatContext& ctx) const;
};

template <typename T> struct map_universal_formatter {};

template <typename T> struct optional_universal_formatter {
    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const universal_format_wrapper<T>& in,
                          FormatContext& ctx) const;
};

template <typename T> struct universal_formatter {
    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const T& in, FormatContext& ctx) const;

  private:
    universal_format_context m_ctx;
};

template <typename T>
concept universal_formattable =
    std::meta::is_base_of_type(^^universal_formatter<T>, ^^std::formatter<T>);

// default formatter
template <typename T> struct std::formatter<universal_format_wrapper<T>> {
    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const universal_format_wrapper<T>& in,
                          FormatContext& ctx) const;
};

template <typename T>
    requires universal_formattable<T>
struct std::formatter<universal_format_wrapper<T>> {
    using type = universal_format_wrapper<T>;

    constexpr auto parse(std::format_parse_context& ctx);

    template <typename FormatContext>
    constexpr auto format(const type& in, FormatContext& ctx) const;
};

// specializations for stl containers

// sequential containers
template <typename T>
    requires universal_formattable<T>
struct std::formatter<universal_format_wrapper<std::vector<T>>>
    : sequence_universal_formatter<std::vector<T>> {};

template <typename T, std::size_t N>
    requires universal_formattable<T>
struct std::formatter<universal_format_wrapper<std::array<T, N>>>
    : sequence_universal_formatter<std::array<T, N>> {};

// // todo: add map containers
// template <typename K, typename T>
//     requires universal_formattable<T>
// struct std::formatter<universal_format_wrapper<std::map<K, T>>>
//     : map_universal_formatter<std::map<K, T>> {};

// template <typename K, typename T>
//     requires universal_formattable<T>
// struct std::formatter<universal_format_wrapper<std::unordered_map<K, T>>>
//     : map_universal_formatter<std::unordered_map<K, T>> {};

// // optional
template <typename T>
    requires universal_formattable<T>
struct std::formatter<universal_format_wrapper<std::optional<T>>>
    : optional_universal_formatter<std::optional<T>> {};

#include "universal.tpp"