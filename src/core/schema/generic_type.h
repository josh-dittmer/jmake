#pragma once

#include "detail/common.h"
#include "detail/schema_error.h"
#include "detail/type_traits/holds_type.h" // IWYU pragma: keep
#include "detail/type_traits/is_type.h"
#include "tag.h"
#include "util/meta/list.h"
#include "util/meta/specialization_of.h"
#include "value.h"

#include <functional>
#include <set>

namespace core::schema {

// clang-format off
template <
    typename Type,
    typename TypeComp = std::less<Type>,
    typename DefaultValue = None,
    typename Values = meta::List<>,
    typename TagsList = meta::List<>
>
// clang-format on
struct GenericType;

// clang-format off
template <typename T>
// clang-format on
concept is_generic_type = meta::is_specialization_of_v<GenericType, T>;

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept generic_type_tag = is_generic_type<T> && tag_for<T, Tag>;
// clang-format on

} // namespace detail

// clang-format off
template <
    typename Type,
    typename TypeComp,
    typename DefaultValue,
    typename... Values,
    typename... Tags
>
struct GenericType<
    Type,
    TypeComp,
    DefaultValue,
    meta::List<Values...>,
    meta::List<Tags...>
> : public detail::ApplyTraits<
    GenericType<Type, TypeComp, DefaultValue, meta::List<Values...>, meta::List<Tags...>>,
    detail::IsType,
    detail::HoldsType<Type>::template Impl
>
// clang-format on
{
    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::generic_type_tag<GenericType, T>;

    GenericType() = default;
    GenericType(GenericType&&) noexcept = default;

    GenericType(const GenericType&) = default;
    GenericType& operator=(const GenericType&) = default;
    GenericType& operator=(GenericType&&) = default;

    ~GenericType() = default;

    // clang-format off
    explicit GenericType(
        std::tuple<Tags...>&& tags,
        DefaultValue&& default_value,
        std::tuple<Values...>&& values,
        std::set<Type, TypeComp>&& value_set
    )
        : m_tags(std::move(tags)),
            m_default_value(std::move(default_value)),
            m_values(std::move(values)),
            m_value_set(std::move(value_set)) {}
    // clang-format on

    template <typename T>
    // clang-format off
    detail::SchemaResult<Type> load(const T& load_from) const;
    // clang-format on

    // already at correct type
    detail::SchemaResult<Type> load(const Type& load_from) const;

    bool has_value(const Type& value) const;

    // clang-format off
    template <typename Tag_> 
    auto add_tag(Tag_ tag);
    // clang-format on

    // clang-format off
    template <is_default_value_init ValueInit_>
    auto set_default_value(ValueInit_ value_init);
    // clang-format on

    // clang-format off
    template <is_value_init ValueInit_>
    auto add_value(ValueInit_ value_init);
    // clang-format on

    const auto& get_tags() const { return m_tags; }
    const auto& get_default_value() const { return m_default_value; }
    const auto& get_values() const { return m_values; }

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = GenericType<
        Type,
        TypeComp,
        DefaultValue,
        meta::List<Values...>,
        meta::List<Tag_, Tags...>
    >;
    // clang-format on

    template <is_value Value_>
    // clang-format off
    using rebind_default_value = GenericType<
        Type,
        TypeComp,
        Value_,
        meta::List<Values...>,
        meta::List<Tags...>
    >;
    // clang-format on

    template <is_value Value_>
    // clang-format off
    using append_value = GenericType<
        Type,
        TypeComp,
        DefaultValue,
        meta::List<Value_, Values...>,
        meta::List<Tags...>
    >;
    // clang-format on

    std::tuple<Tags...> m_tags;

    DefaultValue m_default_value;
    std::tuple<Values...> m_values;

    // for quick lookup at runtime
    std::set<Type, TypeComp> m_value_set;
};

// clang-format off
template <
    typename Type,
    typename TypeComp = std::less<Type>,
    typename... Ts
>
// clang-format on
extern auto type(Ts... builders);

namespace tags {

// allow any value
struct restricted {
    template <is_generic_type T>
    Result<> check(const T& type, const T::out_type& value) const;
};

// minimum value
template <typename T> struct min {
    explicit min(T min) : m_min(min) {}

    template <is_generic_type U>
    Result<> check(const U&, const U::out_type& value) const;

  private:
    T m_min;
};

// maximum value
template <typename T> struct max {
    explicit max(T max) : m_max(max) {}

    template <is_generic_type U>
    Result<> check(const U&, const U::out_type& value) const;

  private:
    T m_max;
};

} // namespace tags

} // namespace core::schema

#include "generic_type.tpp"