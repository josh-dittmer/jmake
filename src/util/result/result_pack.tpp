#pragma once

#include "result_pack.h"

#include "util/meta/for_each.h"

// clang-format off
template <
    typename... Ts,
    typename E
>
constexpr Result<None, E> ResultPack<
    meta::List<Ts...>,
    E
>::ok()
// clang-format on
{
    return meta::for_each_until(m_results, [](auto& res) -> Result<> {
        if (!res.ok()) {
            return err(res);
        }

        return ::ok();
    });
}

// clang-format off
template <
    typename... Ts,
    typename E
>
template <
    typename Func
>
constexpr auto ResultPack<
    meta::List<Ts...>,
    E
>::unwrap(Func&& func)
// clang-format on
{
    return std::apply(
        [&](auto&&... result) {
            return std::forward<Func>(func)(result.unwrap()...);
        },
        m_results);
}

// clang-format off
template <
    typename... Ts,
    typename E
>
constexpr ResultPack<
    meta::List<Ts...>,
    E
>
// clang-format on
result_pack(Result<Ts, E>... results) {
    // clang-format off
    return ResultPack<
        meta::List<Ts...>,
        E
    >(std::move(results)...);
    // clang-format on
}