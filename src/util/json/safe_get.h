#pragma once

#include "util/result/result.h"

#include <nlohmann/json.hpp>

namespace util::json {

template <typename T, typename JsonType>
inline Result<> safe_get_to(const JsonType& j, T& to) {
    // Type validation based on target type T
    if constexpr (std::is_same_v<T, std::string>) {
        if (!j.is_string()) {
            return err("type mismatch: expected string");
        }
    } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
        if (!j.is_number_integer()) {
            return err("type mismatch: expected integer");
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        if (!j.is_number()) {
            return err("type mismatch: expected number");
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (!j.is_boolean()) {
            return err("type mismatch: expected boolean");
        }
    }

    try {
        j.get_to(to);
        return ok();
    } catch (const std::exception& e) {
        return err(std::string("deserialization error: ") + e.what());
    }
}

template <typename T, typename JsonType>
inline Result<T> safe_get(const JsonType& j) {
    T result;

    auto res = safe_get_to(j, result);
    if (!res.ok()) {
        return err(res);
    }

    return ok(std::move(result));
}

} // namespace util::json