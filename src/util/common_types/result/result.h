#pragma once

#include "error.h"
#include "util/common_types/none.h"

#include <expected>
#include <source_location>

#define HUH(expr)                                                              \
    ({                                                                         \
        auto&& res = (expr);                                                   \
        if (!res.has_value())                                                  \
            return err(res);                                                   \
        std::move(res.value());                                                \
    })

template <typename T = None, typename E = Error<>>
using Result = std::expected<T, E>;

// error creation helper functions
template <typename T>
extern constexpr std::unexpected<ErrorInitializer<T>>
err(T value, std::source_location location = std::source_location::current());

template <typename T, typename U>
extern constexpr std::unexpected<ErrorInitializer<U>>
err(Result<T, Error<U>>& result,
    std::source_location location = std::source_location::current());

extern constexpr None ok();

#include "result.tpp"