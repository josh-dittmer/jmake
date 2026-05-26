#pragma once

#include "util/common_types/result/result.h"

#include <meta>

namespace schema {

// common rules
template <typename T> struct range {
    constexpr range(T min, T max) : m_min(min), m_max(max) {}

    Result<> rule(T data) {
        if (data < m_min || data > m_max) {
            return err("out of range");
        }

        return ok();
    }

    T m_min;
    T m_max;
};

// clang-format off
template <
    std::meta::info Member,
    typename... Context,
    typename T
>
// clang-format on
extern Result<> rules_satisfied(const std::tuple<Context...>& context,
                                const T& data);

} // namespace schema

#include "rule.tpp"