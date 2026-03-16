#pragma once

#include "result.h"
#include "util/meta/list.h"

#include <tuple>

// clang-format off
template <
    typename TsList,
    typename ErrorType
>
// clang-format on
class ResultPack;

// clang-format off
template <
   typename... Ts,
   typename E
> 
class ResultPack<
    meta::List<Ts...>,
    E
>
// clang-format on
{
  public:
    explicit constexpr ResultPack(Result<Ts>... results)
        : m_results(std::move(results)...) {}

    constexpr Result<None, E> ok();

    template <typename Func> //
    constexpr auto unwrap(Func&& func);

  private:
    std::tuple<Result<Ts>...> m_results;
};

// clang-format off
template <
    typename... Ts,
    typename E
>
extern constexpr ResultPack<
    meta::List<Ts...>,
    E
>
// clang-format on
result_pack(Result<Ts, E>... results);

#include "result_pack.tpp"