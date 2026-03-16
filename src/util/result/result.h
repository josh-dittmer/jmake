#pragma once

#include "error.h"
#include "result_initializer.h"
#include "util/none.h"

#include <variant>

template <typename T = None, typename E = Error<>> class Result {
  public:
    template <typename V>
    constexpr Result(ResultInitializer<V, true> initializer) // NOLINT
        : m_value(T{std::move(initializer.m_value)}), m_is_ok(true) {}

    template <typename V>
    constexpr Result(ResultInitializer<V, false> initializer) // NOLINT
        : m_value(E{std::move(initializer.m_value)}), m_is_ok(false) {}

    constexpr Result(Result&&) noexcept = default;
    Result(const Result&) = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) = default;

    ~Result() = default;

    constexpr bool ok();

    constexpr T& unwrap();
    constexpr E& unwrap_err();

    template <typename Func> //
    constexpr auto and_then(Func&& func);

  private:
    std::variant<T, E> m_value;
    bool m_is_ok;

    bool m_checked{};
};

// result forwarders
template <typename T, typename E>
extern constexpr ResultInitializer<T, true> ok(Result<T, E>& res);

template <typename T, typename E>
extern constexpr ResultInitializer<E, false> bad(Result<T, E>& res);

template <typename T, typename U>
extern constexpr auto
err(Result<T, Error<U>>& res,
    std::source_location location = std::source_location::current());

#include "result.tpp"