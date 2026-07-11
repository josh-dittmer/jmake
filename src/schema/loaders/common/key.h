#pragma once

#include <meta>
#include <string>

namespace schema::common {

namespace key_transformer {

struct use_name {
    template <std::meta::info Member> //
    static constexpr std::string transform() {
        return std::string{std::meta::identifier_of(Member)};
    }
}; // namespace schema::common

} // namespace key_transformer

// clang-format off
template <
    std::meta::info Member,
    typename Tag,
    typename Transformer = key_transformer::use_name
>
// clang-format on
extern constexpr std::string key_from_member();

} // namespace schema::common

#include "key.tpp"