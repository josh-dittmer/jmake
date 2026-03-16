#pragma once

#include "flat_filter.h"

namespace core::schema::detail {

template <typename JsonType>
Result<FlatFilter<JsonType>>
FlatFilter<JsonType>::from_json(const JsonType& json) {
    if (!json.is_object()) {
        return err("not an object");
    }

    MapType result;

    for (const auto& [k, v] : json.items()) {
        // clang-format off
        next_subobject(result, detail::FlatData<JsonType>{
            .m_key_remainder = k,
            .m_data = std::ref(v)
        });
        // clang-format on
    }

    return ok(FlatFilter(std::move(result)));
}

template <typename JsonType>
FlatFilter<JsonType>
FlatFilter<JsonType>::filter(const std::string& key) const {
    MapType result;

    auto range = m_data.equal_range(key);

    for (auto i = range.first; i != range.second; ++i) {
        next_subobject(result, i->second);
    }

    return Filtered(std::move(result));
}

template <typename JsonType>
OptCRef<JsonType> FlatFilter<JsonType>::get(const std::string& key) const {
    auto mit = m_data.find(key);
    if (mit == m_data.end()) {
        return std::nullopt;
    }

    if (mit->second.m_key_remainder) {
        return std::nullopt;
    }

    return mit->second.m_data;
}

template <typename JsonType>
auto FlatFilter<JsonType>::next_key(const std::string& key_remainder) {
    auto pos = key_remainder.find('.');

    if (pos == std::string::npos) {
        return std::make_pair(key_remainder, opt<std::string>());
    }

    std::string next_key{key_remainder.substr(0, pos)};
    std::string next_rmd{
        key_remainder.substr(pos + 1, key_remainder.size() - pos - 1)};

    return std::make_pair(std::move(next_key), opt(std::move(next_rmd)));
}

template <typename JsonType>
void FlatFilter<JsonType>::next_subobject(
    MapType& map, const detail::FlatData<JsonType>& data) {

    const auto& rmd = data.m_key_remainder;
    const auto& json = data.m_data.get();

    // flatten and insert if nested object
    if (!rmd && json.is_object()) {
        for (const auto& [k, v] : json.items()) {
            auto [nk, nr] = next_key(k);

            // clang-format off
            map.emplace(nk, detail::FlatData<JsonType>{
                .m_key_remainder = nr,
                .m_data = std::ref(v)
            });
            // clang-format on
        }
    }

    // otherwise just insert
    else if (rmd) {
        auto [nk, nr] = next_key(*rmd);

        // clang-format off
        map.emplace(nk, detail::FlatData<JsonType>{
            .m_key_remainder = nr,
            .m_data = data.m_data
        });
        // clang-format on
    }
}

template <typename JsonType> auto filter(const JsonType& json) {
    return FlatFilter<JsonType>::from_json(json);
}

} // namespace core::schema::detail