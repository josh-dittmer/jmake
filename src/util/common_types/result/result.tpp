#pragma once

#include "result.h"
#include "util/common_types/result/error.h"

#include <cassert>
#include <utility>

template <typename T, typename E> constexpr bool Result<T, E>::ok() {
    m_checked = true;
    return m_is_ok;
}

template <typename T, typename E> constexpr T& Result<T, E>::unwrap() {
    assert(m_checked && "unwrap() not allowed before ok()");
    assert(m_is_ok && "unwrap() not allowed on an error-result");

    return std::get<0>(m_value);
}

template <typename T, typename E> constexpr E& Result<T, E>::unwrap_err() {
    assert(m_checked && "unwrap_err() not allowed before ok()");
    assert(!m_is_ok && "unwrap_err() not allowed on an value-result");

    return std::get<1>(m_value);
}

template <typename T, typename E>
template <typename Func>
constexpr auto Result<T, E>::and_then(Func&& func) {
    if (!this->ok()) {
        return std::move(*this);
    }

    return std::forward<Func>(func)(unwrap());
}

template <typename T, typename E>
constexpr ResultInitializer<T, true> ok(Result<T, E>& res) {
    return ResultInitializer<T, true>{std::move(res.unwrap())};
}

template <typename T, typename E>
constexpr ResultInitializer<E, false> bad(Result<T, E>& res) {
    return ResultInitializer<E, false>{std::move(res.unwrap_err())};
}

template <typename T, typename U>
constexpr auto err(Result<T, Error<U>>& res, std::source_location location) {
    return bad(ErrorInitializer{std::move(res.unwrap_err().get()),
                                std::move(location)});
}