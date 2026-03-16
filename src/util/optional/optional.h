#pragma once

#include <optional>

template <typename T> using Opt = std::optional<T>;
template <typename T> using OptRef = std::optional<std::reference_wrapper<T>>;

template <typename T>
using OptCRef = std::optional<std::reference_wrapper<const T>>;

template <typename T> extern Opt<T> opt(T value);
template <typename T> extern Opt<T> opt();

template <typename T> extern OptRef<T> opt_ref(T& value);
template <typename T> extern OptRef<T> opt_ref();

#include "optional.tpp"