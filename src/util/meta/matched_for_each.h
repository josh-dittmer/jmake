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
    std::size_t I,
    std::size_t N,
    typename Func,
    typename... Tuples
>
constexpr void matched_for_each_loop(
    Func&& func,
    Tuples&&... tuples
)
// clang-format on
{
    if constexpr (I < N) {
        std::forward<Func>(func)(std::get<I>(std::forward<Tuples>(tuples))...);

        // clang-format off
        matched_for_each_loop<I + 1, N>(
            std::forward<Func>(func),
            std::forward<Tuples>(tuples)...
        );
        // clang-format on
    }
}

// clang-format off
template <
    std::size_t I,
    std::size_t N,
    typename Func,
    typename... Tuples
>
constexpr auto matched_for_each_until_loop(
    Func&& func,
    Tuples&&... tuples
)
{
    using ResultType = std::invoke_result_t<Func, decltype(std::get<0>(std::declval<Tuples>()))...>;

    if constexpr (I >= N) {
        return ResultType{ok()};
    } else {
        auto step = std::forward<Func>(func)(std::get<I>(std::forward<Tuples>(tuples))...);
        if (!step.ok()) {
            return step;
        }

        if constexpr (I + 1 == N) {
            return step;
        }
        
        return matched_for_each_until_loop<I + 1, N>(
            std::forward<Func>(func), std::forward<Tuples>(tuples)...);
    }
}
// clang-format on

} // namespace detail

// clang-format off
template <
    typename Tuple1,
    typename Tuple2,
    typename... MoreTuples,
    typename Func
> requires specialization_of<std::tuple, std::remove_cvref_t<Tuple1>> &&
            specialization_of<std::tuple, std::remove_cvref_t<Tuple2>> &&
            (specialization_of<std::tuple, std::remove_cvref_t<MoreTuples>> && ...)
constexpr void matched_for_each(
    Tuple1&& tuple1,
    Tuple2&& tuple2,
    MoreTuples&&... more,
    Func&& func
)
// clang-format on
{
    using T1 = std::remove_reference_t<Tuple1>;
    constexpr std::size_t n = std::tuple_size_v<T1>;

    static_assert(std::tuple_size_v<std::remove_reference_t<Tuple2>> == n,
                  "matched_for_each: tuple sizes must match");
    static_assert(
        ((std::tuple_size_v<std::remove_reference_t<MoreTuples>> == n) && ...),
        "matched_for_each: tuple sizes must match");

    // clang-format off
    detail::matched_for_each_loop<0, n>(
        std::forward<Func>(func),
        std::forward<Tuple1>(tuple1),
        std::forward<Tuple2>(tuple2),
        std::forward<MoreTuples>(more)...);
    // clang-format on
}

// clang-format off
template <
    typename Tuple1,
    typename Tuple2,
    typename... MoreTuples,
    typename Func
> requires specialization_of<std::tuple, std::remove_cvref_t<Tuple1>> &&
         specialization_of<std::tuple, std::remove_cvref_t<Tuple2>> &&
         (specialization_of<std::tuple, std::remove_cvref_t<MoreTuples>> && ...)
constexpr auto matched_for_each_until(
    Tuple1&& tuple1,
    Tuple2&& tuple2,
    MoreTuples&&... more,
    Func&& func
)
// clang-format on
{
    using T1 = std::remove_reference_t<Tuple1>;
    constexpr std::size_t n = std::tuple_size_v<T1>;

    static_assert(std::tuple_size_v<std::remove_reference_t<Tuple2>> == n,
                  "matched_for_each_until: tuple sizes must match");
    static_assert(
        ((std::tuple_size_v<std::remove_reference_t<MoreTuples>> == n) && ...),
        "matched_for_each_until: tuple sizes must match");

    if constexpr (n <= 0) {
        return Result<>{ok()};
    } else {
        // clang-format off
        return detail::matched_for_each_until_loop<0, n>(
            std::forward<Func>(func),
            std::forward<Tuple1>(tuple1),
            std::forward<Tuple2>(tuple2),
            std::forward<MoreTuples>(more)...);
        // clang-format on
    }
}

} // namespace meta