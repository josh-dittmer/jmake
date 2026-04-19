#pragma once

#include "result_initializer.h"

#include <utility>

template <typename T> constexpr ResultInitializer<T, true> ok(T value) {
    return ResultInitializer<T, true>{std::move(value)};
}

template <typename T> constexpr ResultInitializer<T, true> ok() {
    return ResultInitializer<T, true>{};
}

template <typename T> constexpr ResultInitializer<T, false> bad(T value) {
    return ResultInitializer<T, false>{std::move(value)};
}
