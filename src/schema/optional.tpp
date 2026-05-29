#pragma once

#include "optional.h"

namespace schema::detail {

// clang-format off
template <typename T>
template <
    std::meta::info Member,
    typename... Context
>
// clang-format on
auto validator<std::optional<T>>::validate(
    const std::tuple<Context...>& context, const type& data) -> SchemaResult<> {
    // no value? optional so that's fine
    if (!data) {
        return ok();
    }

    return HUH(validator<T>::template validate<Member>(context, *data));
}

template <typename T>
void combiner<std::optional<T>>::combine(type& base, const type& in) {
    // first value? set base
    if (!base) {
        base = in;
    }

    // otherwise, combine with previous (if value is present)
    else if (in) {
        combiner<T>::combine(*base, *in);
    }
}

template <typename T>
auto in_type<std::optional<T>>::to_out(data_type in)
    -> SchemaResult<std::optional<T>> {
    std::optional<T> result{};

    // if we have value, convert to out type
    if (in) {
        result = HUH(in_type<T>::to_out(std::move(*in)));
    }

    return result;
}

} // namespace schema::detail