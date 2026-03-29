#pragma once

#include "specialization_of.h"
#include "util/result/result.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace meta {

namespace detail {

// clang-format off
template <
    typename Tuple, 
    typename Func, 
    std::size_t... Is
>
constexpr auto for_each_until_impl(
    Tuple&& tuple,
    Func&& func, 
    std::index_sequence<Is...>
) 
{
    using ResultType = std::invoke_result_t<Func, decltype(std::get<0>(std::declval<Tuple>()))>;

    ResultType res = ok();
    (
        (res = res.and_then([&](auto) { 
            return std::forward<Func>(func)(std::get<Is>(std::forward<Tuple>(tuple))); 
        })), 
        ...
    );

    return res;
}
// clang-format on

} // namespace detail

// clang-format off
template <
    typename Tuple, 
    typename Func
> requires specialization_of<std::tuple, std::remove_cvref_t<Tuple>>
constexpr auto for_each_until(
    Tuple&& tuple, 
    Func&& func
)
 {
    constexpr std::size_t n = std::tuple_size_v<std::remove_reference_t<Tuple>>;

    if constexpr(n <= 0) {
        return Result<>{ok()};
    }

    else {
        return detail::for_each_until_impl(
            std::forward<Tuple>(tuple),
            std::forward<Func>(func),
            std::make_index_sequence<n>{}
        );
    }
}
// clang-format on

// clang-format off
template <
    typename... Args, 
    typename Func
>
constexpr auto for_each_until(Args&&... args, Func&& func) {
    using ResultType = std::invoke_result_t<Func, std::tuple_element_t<0, std::tuple<Args...>>>;
    
    ResultType res = ok();
    (
        (res = res.and_then([&](auto) {
            return std::forward<Func>(func)(std::forward<Args>(args));
        })),
        ...
    );

    return res;
}
// clang-format on

template <typename Tuple, typename Func>
    requires specialization_of<std::tuple, std::remove_cvref_t<Tuple>>
constexpr void for_each(Tuple&& tuple, Func&& func) {
    std::apply(
        [&func](auto&&... args) {
            (func(std::forward<decltype(args)>(args)), ...);
        },
        std::forward<Tuple>(tuple));
}

template <typename... Args, typename Func>
constexpr void for_each(Args&&... args, Func&& func) {
    (std::forward<Func>(func)(std::forward<Args>(args)), ...);
}

} // namespace meta
