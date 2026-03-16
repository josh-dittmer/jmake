#pragma once

#include "util/result/result_initializer.h"

#include <format>
#include <source_location>
#include <vector>

template <typename T> struct ErrorInitializer {
    T m_data;
    std::source_location m_location;
};

template <typename T = std::string> class Error {
  public:
    template <typename V>
    constexpr explicit Error(ErrorInitializer<V> initializer)
        : Error(T{std::move(initializer.m_data)},
                std::move(initializer.m_location)) {}

    constexpr Error(T&& data, std::source_location location)
        : m_data(std::move(data)) {
        m_backtrace.emplace_back(std::move(location));
    }

    constexpr Error(Error&& error, std::source_location location)
        : Error(std::move(error)) {
        m_backtrace.emplace_back(std::move(location));
    }

    constexpr Error(Error&&) = default;
    Error(Error&) = delete;
    Error(const Error&) = delete;
    Error& operator=(const Error&) = delete;
    Error& operator=(Error&&) = default;

    ~Error() = default;

    constexpr T& get() { return m_data; }

  private:
    std::vector<std::source_location> m_backtrace;
    T m_data;

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

// error creation helper functions
template <typename T>
extern constexpr ResultInitializer<ErrorInitializer<T>, false>
err(T value, std::source_location location = std::source_location::current());

#include "error.tpp"