#pragma once

#include "util/result/result.h"

#include <exception>
#include <functional>
#include <string_view>

namespace util::json {

template <typename JsonType>
inline Result<std::reference_wrapper<JsonType>> safe_at(JsonType& from,
                                                        std::string_view key) {
    try {
        return ok(std::ref(from.at(key)));
    } catch (const std::exception& e) {
        return err(e.what());
    }
}

} // namespace util::json
