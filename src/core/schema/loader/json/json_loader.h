#pragma once

#include "core/schema/array.h"
#include "core/schema/dict.h"
#include "core/schema/generic_type.h"
#include "core/schema/loader/loader.h"
#include "core/schema/object.h"
#include "core/schema/property.h"
#include "flat_filter.h"

#include <nlohmann/json.hpp>

namespace core::schema {

namespace tags {

struct json_id {
    std::string m_id;
};

} // namespace tags

template <> struct Loader<nlohmann::json> {
    template <is_generic_type T>
    static auto for_generic_type(const T&, const nlohmann::json& data);

    template <is_array T>
    static auto for_array(const T&, const nlohmann::json& data);

    template <is_dict T>
    static auto for_dict(const T&, const nlohmann::json& data);

    template <is_object T>
    static auto for_object(const T&, const nlohmann::json& data);
};

// we use a special intermediate json type to support efficient handling of
// dot-separated property keys
template <> struct Loader<detail::FlatFilter<>> {
    // clang-format off
    template <
        is_property T
    > requires T::template has_tag<tags::json_id>
    static Result<OptCRef<nlohmann::json>> for_property(const T& prop, const detail::FlatFilter<>& data);
    // clang-format on

    // clang-format off
    template <
        is_property T
    >
    // clang-format on
    static std::string readable_id(const T& prop);
};

} // namespace core::schema

#include "json_loader.tpp"