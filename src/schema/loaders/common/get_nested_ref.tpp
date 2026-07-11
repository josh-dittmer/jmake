#pragma once

#include "get_nested_ref.h"

namespace schema::common {

// clang-format off
template <
    std::meta::info First, 
    std::meta::info... Path
>
// clang-format on
auto& get_nested_ref(auto& parent) {
    if constexpr (sizeof...(Path) <= 0) {
        // in base case, parent is not an optional
        return parent.m_data.[:First:].get();
    } else {
        auto& p_ref = get_nested_ref<Path...>(parent);
        using p_type = std::remove_reference_t<decltype(p_ref)>::value_type;

        if (!p_ref) {
            // construct default if we don't have it
            p_ref = p_type{};
        }

        return p_ref->m_data.[:First:].get();
    }
}

} // namespace schema::common