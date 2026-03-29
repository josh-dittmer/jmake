#pragma once

#include "json_filter.h"

#include "util/json/safe_get.h"
#include <variant>

namespace core::schema::detail {

template <typename JsonType>
JsonFilter<JsonType> JsonFilter<JsonType>::from_json(const JsonType& json) {
    if (!json.is_object()) {
        return JsonFilter(std::ref(json));
    }

    DataType result;

    for (const auto& [k, v] : json.items()) {
        // clang-format off
        next_subobject(result, detail::FlatData<JsonType>{
            .m_key_remainder = k,
            .m_data = std::ref(v)
        });
        // clang-format on
    }

    return JsonFilter(std::move(result));
}

template <typename JsonType>
Opt<JsonFilter<JsonType>>
JsonFilter<JsonType>::filter(const std::string& key) const {
    if (is_value()) {
        return std::nullopt;
    }

    const auto& map = as_map();

    DataType result;

    auto [first, last] = map.equal_range(key);

    // if nothing found
    if (first == last) {
        return std::nullopt;
    }

    for (auto i = first; i != last; ++i) {
        next_subobject(result, i->second);
    }

    return JsonFilter(std::move(result));
}

template <typename JsonType>
Result<std::map<std::string, JsonFilter<JsonType>>>
JsonFilter<JsonType>::to_dict() const {
    std::map<std::string, JsonFilter> result;

    if (!is_map()) {
        return err("not an object");
    }

    const auto& map = as_map();

    // for each unique key
    for (auto it = map.begin(); it != map.end();) {
        const auto& key = it->first;

        result.emplace(key, *filter(key));

        it = map.upper_bound(key);
    }

    return ok(std::move(result));
}

template <typename JsonType>
Result<std::vector<JsonFilter<JsonType>>>
JsonFilter<JsonType>::to_array() const {
    std::vector<JsonFilter> result;

    if (!is_value()) {
        return err("is an object");
    }

    const auto& value = as_value().get();

    if (!value.is_array()) {
        return err("not an array");
    }

    result.reserve(value.size());

    for (const auto& i : value) {
        result.push_back(JsonFilter::from_json(i));
    }

    return ok(std::move(result));
}

template <typename JsonType>
template <typename T>
Result<T> JsonFilter<JsonType>::get() const {
    if (!is_value()) {
        return err("not found");
    }

    const auto& value = as_value();

    auto get_res = util::json::safe_get<T>(value.get());
    if (!get_res.ok()) {
        return err(get_res);
    }

    return ok(get_res);
}

template <typename JsonType>
auto JsonFilter<JsonType>::next_key(const std::string& key_remainder) {
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
void JsonFilter<JsonType>::next_subobject(
    DataType& data, const detail::FlatData<JsonType>& target) {
    if (std::holds_alternative<ValueType>(data)) {
        return;
    }

    auto& map = std::get<MapType>(data);

    const auto& rmd = target.m_key_remainder;
    const auto& json = target.m_data.get();

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

    // else if key remains
    else if (rmd) {
        auto [nk, nr] = next_key(*rmd);

        // clang-format off
        map.emplace(nk, detail::FlatData<JsonType>{
            .m_key_remainder = nr,
            .m_data = target.m_data
        });
        // clang-format on
    }

    // else we're dealing with a value
    else {
        data = target.m_data;
    }
}

template <typename JsonType> auto filter(const JsonType& json) {
    return JsonFilter<JsonType>::from_json(json);
}

} // namespace core::schema::detail