#pragma once

#include "util/optional/optional.h"
#include "util/result/result.h"

#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <variant>

namespace core::schema::detail {

template <typename JsonType> struct FlatData {
    Opt<std::string> m_key_remainder;
    std::reference_wrapper<const JsonType> m_data;
};

template <typename JsonType = nlohmann::json> class JsonFilter {
  public:
    static JsonFilter from_json(const JsonType& json);

    Opt<JsonFilter> filter(const std::string& key) const;

    Result<std::map<std::string, JsonFilter>> to_dict() const;
    Result<std::vector<JsonFilter>> to_array() const;

    template <typename T> Result<T> get() const;

  private:
    using MapType = std::multimap<std::string, detail::FlatData<JsonType>>;
    using ValueType = std::reference_wrapper<const JsonType>;
    using DataType = std::variant<MapType, ValueType>;

    explicit JsonFilter(MapType&& data) : m_data(std::move(data)) {}
    explicit JsonFilter(ValueType&& data) : m_data(std::move(data)) {}
    explicit JsonFilter(DataType&& data) : m_data(std::move(data)) {}

    static auto next_key(const std::string& key_remainder);
    static void next_subobject(DataType& data,
                               const detail::FlatData<JsonType>& target);

    const MapType& as_map() const { return std::get<MapType>(m_data); }
    const ValueType& as_value() const { return std::get<ValueType>(m_data); }

    bool is_map() const { return std::holds_alternative<MapType>(m_data); }
    bool is_value() const { return std::holds_alternative<ValueType>(m_data); }

    DataType m_data;
};

template <typename JsonType> extern auto filter(const JsonType& json);

} // namespace core::schema::detail

#include "json_filter.tpp"