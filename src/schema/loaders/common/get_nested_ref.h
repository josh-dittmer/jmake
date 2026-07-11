#pragma once

#include <meta>

namespace schema::common {

// clang-format off
template <
    std::meta::info First, 
    std::meta::info... Path
>
// clang-format on
extern auto& get_nested_ref(auto& parent);

} // namespace schema::common

#include "get_nested_ref.tpp"