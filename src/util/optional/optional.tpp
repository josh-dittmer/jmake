#pragma once

#include "optional.h"

#include <functional>
#include <utility>

template <typename T> Opt<T> opt(T value) {
    return std::make_optional(std::move(value));
}

template <typename T> Opt<T> opt() { return std::nullopt; }

template <typename T> OptRef<T> opt_ref(T& value) {
    return Opt(std::ref(value));
}

template <typename T> OptRef<T> opt_ref() { return std::nullopt; }
