#pragma once

#include "json_loader.h"

namespace core::schema {

template <is_generic_type T>
auto Loader<detail::JsonFilter<>>::for_generic_type(
    const T&, const detail::JsonFilter<>& data) {
    return data.get<typename T::in_type>();
}

template <is_array T>
auto Loader<detail::JsonFilter<>>::for_array(const T&,
                                             const detail::JsonFilter<>& data) {
    return data.to_array();
}

template <is_dict T>
auto Loader<detail::JsonFilter<>>::for_dict(const T&,
                                            const detail::JsonFilter<>& data) {
    return data.to_dict();
}

// clang-format off
template <
    is_property T
> requires T::template has_tag<tags::json_id>
Result<Opt<detail::JsonFilter<>>> Loader<detail::JsonFilter<>>::for_property(const T& prop, const detail::JsonFilter<>& data)
// clang-format on
{
    const auto& id = std::get<tags::json_id>(prop.get_tags()).m_id;
    return ok(data.filter(id));
}

template <is_object T>
Result<detail::JsonFilter<>>
Loader<detail::JsonFilter<>>::for_object(const T&,
                                         const detail::JsonFilter<>& data) {
    return ok(data);
}

// clang-format off
template <
    is_property T
>
std::string Loader<detail::JsonFilter<>>::readable_id(const T& prop)
// clang-format on
{
    return std::get<tags::json_id>(prop.get_tags()).m_id;
}

template <is_object T>
Result<detail::JsonFilter<>>
Loader<nlohmann::json>::for_object(const T&, const nlohmann::json& data) {
    return ok(detail::filter(data));
}

} // namespace core::schema