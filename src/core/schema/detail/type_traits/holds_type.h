#pragma once

namespace core::schema::detail {

// clang-format off
template <
    typename OutType
>
// clang-format on
struct HoldsType {
    // clang-format off
    template <
        typename Derived
    >
    // clang-format on
    struct Impl;
};

// clang-format off
template <
    typename OutType
>
template <
    typename Derived
>
// clang-format on
struct HoldsType<OutType>::Impl {
    using out_type = OutType;

    auto check_against_tags(const OutType& value) const;
};

} // namespace core::schema::detail

#include "holds_type.tpp"