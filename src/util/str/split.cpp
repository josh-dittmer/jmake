#include "split.h"

#include <algorithm>
#include <ranges>

namespace util::str {

std::vector<std::string> split(std::string_view str, char delim) {
    std::vector<std::string> res;
    res.reserve(1 + std::ranges::count(str, delim));

    for (auto part : str | std::views::split(delim)) {
        res.emplace_back(std::string_view(part));
    }

    return res;
}

} // namespace util::str