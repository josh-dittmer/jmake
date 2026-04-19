#pragma once

#include <utility>

namespace detail {

template <typename T, typename Arg, typename... Rest>
concept FirstCtorArg = requires(Arg arg, Rest&&... rest) {
    T{std::move(arg), std::forward<Rest>(rest)...};
};

} // namespace detail