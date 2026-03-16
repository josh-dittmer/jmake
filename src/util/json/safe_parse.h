#pragma once

#include "util/file/io.h"
#include "util/result/result.h"

#include <nlohmann/json.hpp>

namespace util::json {

using opt = uint8_t;

// clang-format off
static const opt comments           = (uint8_t)1;
static const opt trailing_commas    = (uint8_t)1 << (uint8_t)1;
static const opt lax                = comments | trailing_commas;
// clang-format on

template <typename T>
inline Result<nlohmann::json> safe_parse(T&& from, opt opts = 0x00) {
    try {
        // clang-format off
        nlohmann::json result = nlohmann::json::parse(
            std::forward<T>(from),
            nullptr,
            true, // yes exceptions
            (opts & comments),
            (opts & trailing_commas)
        );
        // clang-format on

        return ok(std::move(result));
    } catch (const nlohmann::json::parse_error& e) {
        return err(e.what());
    }
}

inline Result<nlohmann::json> safe_parse_file(std::string_view path,
                                              opt opts = 0x00) {
    auto open_res = file::open(path, std::ios::in);
    if (!open_res.ok()) {
        return err(open_res);
    }

    return safe_parse(std::move(open_res.unwrap()), opts);
}

} // namespace util::json