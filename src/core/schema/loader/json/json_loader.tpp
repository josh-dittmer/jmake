#pragma once

#include "json_loader.h"

#include "util/json/safe_get.h"

namespace core::schema {

template <is_generic_type T>
auto Loader<nlohmann::json>::for_generic_type(const T&,
                                              const nlohmann::json& data) {
    return util::json::safe_get<detail::out_type<T>>(data);
}

template <is_array T>
auto Loader<nlohmann::json>::for_array(const T&, const nlohmann::json& data) {
    return util::json::safe_get<std::vector<nlohmann::json>>(data);
}

template <is_dict T>
auto Loader<nlohmann::json>::for_dict(const T&, const nlohmann::json& data) {
    return util::json::safe_get<std::map<std::string, nlohmann::json>>(data);
}

template <is_object T>
auto Loader<nlohmann::json>::for_object(const T&, const nlohmann::json& data) {
    return detail::filter(data);
}

// clang-format off
template <
    is_property T
> requires T::template has_tag<tags::json_id>
Result<OptCRef<nlohmann::json>> Loader<detail::FlatFilter<>>::for_property(const T& prop, const detail::FlatFilter<>& data)
// clang-format on
{
    return ok(data.get(std::get<tags::json_id>(prop.get_tags()).m_id));
}

// clang-format off
template <
    is_property T
>
std::string Loader<detail::FlatFilter<>>::readable_id(const T& prop)
// clang-format on
{
    return std::get<tags::json_id>(prop.get_tags()).m_id;
}

} // namespace core::schema