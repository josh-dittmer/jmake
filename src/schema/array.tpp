#pragma once

#include "array.h"

namespace schema {

namespace detail {

// clang-format off
template <typename T>
auto out_type<std::vector<T>>::from_in_type(in_type_t<T> in_type) -> type
// clang-format on
{
    return std::move(in_type);
}

} // namespace detail

} // namespace schema