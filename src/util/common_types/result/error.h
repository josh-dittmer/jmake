#pragma once

#include <format>
#include <source_location>
#include <string>
#include <vector>

namespace detail {

using ErrorBacktrace = std::vector<std::source_location>;

} // namespace detail

template <typename T> struct ErrorInitializer {
    T m_data;
    detail::ErrorBacktrace m_bt;
};

template <typename T = std::string> class Error {
  public:
    template <typename V>
    Error(ErrorInitializer<V> initializer) // NOLINT
        : m_data(std::move(initializer.m_data)),
          m_bt(std::move(initializer.m_bt)) {}

    constexpr T& get() { return m_data; }
    constexpr const T& get() const { return m_data; }

    constexpr detail::ErrorBacktrace& get_bt() { return m_bt; }
    constexpr const detail::ErrorBacktrace& get_bt() const { return m_bt; }

  private:
    T m_data;
    detail::ErrorBacktrace m_bt;

    friend struct std::formatter<Error<T>>;
};

// formatter for error
template <typename T> struct std::formatter<Error<T>> {
    constexpr auto parse(std::format_parse_context& ctx);
    constexpr auto format(const Error<T>& error,
                          std::format_context& ctx) const;

  private:
    enum class Mode : uint8_t { TRACE, FILE, MIN } m_mode = Mode::MIN;

    constexpr auto format_trace(const Error<T>& error,
                                std::format_context& ctx) const;
    constexpr auto format_file(const Error<T>& error,
                               std::format_context& ctx) const;
    constexpr auto format_min(const Error<T>& error,
                              std::format_context& ctx) const;
};

#include "error.tpp"