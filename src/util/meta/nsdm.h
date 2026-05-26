#pragma once

#include <meta>

namespace util::meta {

consteval auto nsdm_of(std::meta::info info) -> std::vector<std::meta::info> {
    return std::meta::nonstatic_data_members_of(
        info, std::meta::access_context::unchecked());
}

consteval auto num_nsdm(std::meta::info info) -> std::size_t {
    return nsdm_of(info).size();
}

consteval auto nsdm_arr(std::meta::info info) -> auto {
    return std::define_static_array(nsdm_of(info));
}

} // namespace util::meta