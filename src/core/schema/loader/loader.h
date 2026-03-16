#pragma once

namespace core::schema {

namespace detail {

template <typename T> using out_type = typename T::out_type;

}

// clang-format off
template <
    typename T
>
// clang-format on
struct Loader;

} // namespace core::schema