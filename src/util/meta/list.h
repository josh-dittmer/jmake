#pragma once

#include "has_type.h"

namespace meta {

template <typename... Types> struct List {
    // clang-format off
    template <typename T>
    using append = List<T, Types...>;
    // clang-format on

    // clang-format off
    template <typename T>
    using has = has_type<T, Types...>;
    // clang-format on
};

} // namespace meta