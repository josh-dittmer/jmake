#pragma once

#include "key.h"

#include "schema/tag.h"

namespace schema::common {

// clang-format off
template <
    std::meta::info Member,
    typename Tag,
    typename Transformer
>
// clang-format on
constexpr std::string key_from_member() {
    constexpr auto id_tag = get_tag<Member, Tag>();
    if constexpr (id_tag) {
        return std::string{id_tag->m_str};
    }

    return Transformer::template transform<Member>();
}

} // namespace schema::common