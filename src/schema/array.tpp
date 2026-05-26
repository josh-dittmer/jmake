#pragma once

#include "array.h"

namespace schema::detail {

// clang-format off
template <typename T>
template <
    std::meta::info Member,
    typename... Context
>
// clang-format on
auto array_validator<T>::validate(const std::tuple<Context...>& context,
                                  const type& data) -> SchemaResult<> {
    using value_t = T::value_type;

    // validate array values
    for (std::size_t i = 0; i < data.size(); i++) {
        auto val_res =
            validator<value_t>::template validate<Member>(context, data[i]);

        if (!val_res) {
            return err(push_bt(val_res, ArrayTrace{i}));
        }
    }

    return ok();
}

template <typename T>
void array_combiner<T>::combine(type& base, const type& in) {
    base.insert(base.end(), in.begin(), in.end());
}

template <typename T>
auto in_type<std::vector<T>>::to_out(data_type in)
    -> SchemaResult<std::vector<T>> {
    std::vector<T> result{};
    result.reserve(in.size());

    for (std::size_t i = 0; i < in.size(); i++) {
        auto out_res = in_type<T>::to_out(std::move(in[i]));
        if (!out_res) {
            return err(push_bt(out_res, ArrayTrace{i}));
        }

        result.push_back(std::move(*out_res));
    }

    return result;
}

template <typename T, std::size_t N>
auto in_type<std::array<T, N>>::to_out(data_type in)
    -> SchemaResult<std::array<T, N>> {
    std::array<T, N> result{};

    for (std::size_t i = 0; i < N; i++) {
        auto out_res = in_type<T>::to_out(std::move(in[i]));
        if (!out_res) {
            return err(push_bt(out_res, ArrayTrace{i}));
        }

        result[i] = std::move(*out_res);
    }

    return result;
}

} // namespace schema::detail