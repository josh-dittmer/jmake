#pragma once

#include "util/common_types/result/result.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace util::file {

inline Result<std::fstream> open(std::string_view path,
                                 std::ios::openmode mode) {
    std::fstream file(std::string(path), mode);
    if (!file) {
        return err(std::format("failed to open file '{}'", path));
    }

    return std::move(file);
}

template <typename T> inline Result<T> read(std::string_view path) {
    using value_type = typename T::value_type;

    static_assert(std::is_default_constructible_v<T>,
                  "Container must be default constructible");
    static_assert(std::is_trivially_copyable_v<value_type>,
                  "Container::value_type must be trivially copyable");

    auto open_res = open(path, std::ios::in | std::ios::binary);
    if (!open_res) {
        return err(open_res);
    }

    auto& file = *open_res;

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    if (size < 0) {
        return err(std::format("failed to determine size of file '{}'", path));
    }

    T buffer;

    file.seekg(0, std::ios::beg);
    if (size > 0) {
        std::string tmp(static_cast<std::size_t>(size), '\0');
        file.read(tmp.data(), size);
        if (!file) {
            return err(std::format("failed to read file '{}'", path));
        }

        buffer.resize(tmp.size());
        std::transform(
            tmp.begin(), tmp.end(), buffer.begin(),
            [](unsigned char ch) { return static_cast<value_type>(ch); });
    }

    return std::move(buffer);
}

} // namespace util::file