#pragma once

#include <tuple>

namespace schema {

template <typename... Ts> std::tuple<Ts...> context(Ts&&... context) {
    return std::tuple{std::forward<Ts>(context)...};
}

} // namespace schema