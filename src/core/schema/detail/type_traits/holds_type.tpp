#pragma once

#include "holds_type.h"

#include "util/meta/for_each.h"
#include "util/result/result.h"

namespace core::schema::detail {

// clang-format off
template <
    typename OutType
>
template <
    typename Derived
>
auto HoldsType<
    OutType
>::Impl<
    Derived
>::check_against_tags(const OutType& value) const
// clang-format on
{
    const auto& d_ref = *static_cast<const Derived*>(this);

    return meta::for_each_until(
        d_ref.get_tags(), [&](const auto& tag) -> Result<> {
            if constexpr (Derived::template valid_tag<decltype(tag)>) {
                return tag.check(d_ref, value);
            } else {
                return ok();
            }
        });
}

} // namespace core::schema::detail