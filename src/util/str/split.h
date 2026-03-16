#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace util::str {

extern std::vector<std::string> split(std::string_view str, char delim);

} // namespace util::str