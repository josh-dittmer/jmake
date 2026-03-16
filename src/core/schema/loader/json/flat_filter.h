#pragma once

#include "util/optional/optional.h"
#include "util/result/result.h"

#include <functional>
#include <map>
#include <nlohmann/json.hpp>

namespace core::schema::detail {

template <typename JsonType> struct FlatData {
    Opt<std::string> m_key_remainder;
    std::reference_wrapper<const JsonType> m_data;
};

template <typename JsonType = nlohmann::json> class FlatFilter {
  public:
    static Result<FlatFilter> from_json(const JsonType& json);

    FlatFilter filter(const std::string& key) const;
    OptCRef<JsonType> get(const std::string& key) const;

  private:
    using MapType = std::multimap<std::string, detail::FlatData<JsonType>>;

    template <typename T>
        requires(!std::same_as<std::remove_cvref_t<T>, FlatFilter>)
    explicit FlatFilter(T&& data) : m_data(std::forward<T>(data)) {}

    static auto next_key(const std::string& key_remainder);
    static void next_subobject(MapType& map,
                               const detail::FlatData<JsonType>& data);

    MapType m_data;
};

template <typename JsonType> extern auto filter(const JsonType& json);

} // namespace core::schema::detail

#include "flat_filter.tpp"