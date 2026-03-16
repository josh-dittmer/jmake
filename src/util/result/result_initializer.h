#pragma once

#include "util/none.h"

template <typename T, bool Ok> struct ResultInitializer {
    T m_value;
};

// helper functions

template <typename T> extern constexpr ResultInitializer<T, true> ok(T value);
template <typename T = None> extern constexpr ResultInitializer<T, true> ok();

template <typename T> extern constexpr ResultInitializer<T, false> bad(T value);

#include "result_initializer.tpp"