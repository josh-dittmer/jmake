#pragma once

#include <meta>

namespace util::meta {

consteval auto ann_arr(std::meta::info info) -> auto {
    return std::define_static_array(std::meta::annotations_of(info));
}

} // namespace util::meta