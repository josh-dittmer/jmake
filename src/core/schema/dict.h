#pragma once

#include "detail/common.h"
#include "detail/schema_error.h"
#include "detail/type_traits/holds_type.h" // IWYU pragma: keep
#include "detail/type_traits/is_type.h"
#include "tag.h"
#include "util/meta/list.h"
#include "util/meta/specialization_of.h"

#include <map>
#include <string>
#include <tuple>

namespace core::schema {

// clang-format off
template <
    typename ValueType,
    typename KeyType = std::string,
    typename TagsList = meta::List<>
>
// clang-format on
struct Dict;

// clang-format off
template <typename T>
// clang-format on
concept is_dict = meta::is_specialization_of_v<Dict, T>;

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept dict_tag = is_dict<T> && tag_for<T, Tag>;
// clang-format on

} // namespace detail

// clang-format off
template <
    typename ValueType,
    typename KeyType,
    typename... Tags
>
struct Dict<
    ValueType,
    KeyType,
    meta::List<Tags...>
> : public detail::ApplyTraits<
    Dict<ValueType, KeyType, meta::List<Tags...>>,
    detail::IsType,
    detail::HoldsType<std::map<KeyType, typename ValueType::out_type>>::template Impl
>
// clang-format on
{
    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::dict_tag<Dict, T>;

    Dict() = default;
    Dict(Dict&&) noexcept = default;

    Dict(const Dict&) = default;
    Dict& operator=(const Dict&) = default;
    Dict& operator=(Dict&&) = default;

    ~Dict() = default;

    // clang-format off
    explicit Dict(
        ValueType&& type,
        std::tuple<Tags...>&& tags = {}
    )
        : m_type(std::move(type)),
            m_tags(std::move(tags)) {}
    // clang-format on

    template <typename T>
    // clang-format off
    detail::SchemaResult<std::map<KeyType, typename ValueType::out_type>> load(const T& load_from) const;
    // calang-format on

    // clang-format off
    template <typename Tag_> 
    auto add_tag(Tag_ tag);
    // clang-format on

    const auto& get_type() const { return m_type; }
    const auto& get_tags() const { return m_tags; }

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = Dict<
        ValueType,
        KeyType,
        meta::List<Tag_, Tags...>
    >;
    // clang-format on

    ValueType m_type;
    std::tuple<Tags...> m_tags;
};

// clang-format off
template <
    detail::is_type Type,
    typename... Ts
>
// clang-format on
extern auto dict(Type type, Ts... builders);

// clang-format off
template <
    typename T,
    typename... Ts
>
// clang-format on
extern auto dict(Ts... builders);

} // namespace core::schema

#include "dict.tpp"