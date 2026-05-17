#pragma once

#include "result.h"
#include "util/common_types/result/error.h"
#include <expected>

template <typename T>
constexpr std::unexpected<ErrorInitializer<T>>
err(T value, std::source_location location) {
    return std::unexpected(
        ErrorInitializer<T>{std::move(value), {std::move(location)}});
}

template <typename T, typename U>
constexpr std::unexpected<ErrorInitializer<U>>
err(Result<T, Error<U>>& result, std::source_location location) {
    auto& error = result.error();
    error.get_bt().push_back(location);
    return std::unexpected(
        ErrorInitializer<U>{std::move(error.get()), std::move(error.get_bt())});
}

constexpr None ok() { return None{}; }