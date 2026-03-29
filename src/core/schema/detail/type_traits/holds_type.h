#pragma once

namespace core::schema::detail {

// clang-format off
template <
    typename OutType,
    typename InType
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
    typename OutType,
    typename InType
>
template <
    typename Derived
>
struct HoldsType<
    OutType, 
    InType
>::Impl
// clang-format on
{
    using in_type = InType;
    using out_type = OutType;

    auto check_against_tags(const InType& value) const;
};

} // namespace core::schema::detail

#include "holds_type.tpp"