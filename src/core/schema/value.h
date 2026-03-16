#pragma once

#include "util/meta/list.h"
#include "util/meta/specialization_of.h"

#include <set>
#include <tuple>

namespace core::schema {

// clang-format off
template <
    typename T,
    typename TagsLiss = meta::List<>
>
// clang-format on
struct Value;

// clang-format off
template <
    typename T,
    typename TagsList = meta::List<>
>
// clang-format on
struct ValueInit;

// clang-format off
template <
    typename T,
    typename... Tags
>
// clang-format on
struct DefaultValueInit;

namespace detail {

// clang-format off
template <
    typename T,
    typename ValueInit
>
concept accepts_values = requires(T type, ValueInit value_init) {
    type.add_value(std::move(value_init));
};
// clang-format on

// clang-format off
template <
    typename T,
    typename ValueInit
>
concept accepts_default_values = requires(T type, ValueInit value_init) {
    type.set_default_value(std::move(value_init));
};
// clang-format on

} // namespace detail

// clang-format off
template <typename T>
// clang-format on
concept is_value_init = meta::is_specialization_of_v<ValueInit, T>;

// clang-format off
template <typename T>
// clang-format on
concept is_default_value_init =
    meta::is_specialization_of_v<DefaultValueInit, T>;

// clang-format off
template <typename T>
// clang-format on
concept is_value = meta::is_specialization_of_v<Value, T>;

// clang-format off
template <
    typename T,
    typename... Tags
>
struct Value<
    T,
    meta::List<Tags...>
>
// clang-format on
{
    // clang-format off
    explicit Value(
        const T& value_ref,
        std::tuple<Tags...>&& tags = {}
    ) : m_value_ref(value_ref),
            m_tags(std::move(tags)) {}
    // clang-format on

    Value(Value&&) noexcept = default;

    Value(const Value&) = default;
    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = default;

    ~Value() = default;

    const auto& get_value() const { return m_value_ref; }
    const auto& get_tags() const { return m_tags; }

  private:
    const T& m_value_ref; // NOLINT
    std::tuple<Tags...> m_tags;
};

// clang-format off
template <
    typename T,
    typename... Tags
>
struct ValueInit<
    T,
    meta::List<Tags...>
>
// clang-format on
{
    ValueInit() = default;
    ValueInit(ValueInit&&) noexcept = default;

    ValueInit(const ValueInit&) = delete;
    ValueInit& operator=(const ValueInit&) = delete;
    ValueInit& operator=(ValueInit&&) = delete;

    ~ValueInit() = default;

    // clang-format off
    explicit ValueInit(
        T&& value,
        std::tuple<Tags...>&& tags = {}
    ) : m_value(std::move(value)),
            m_tags(std::move(tags)) {}
    // clang-format on

    // clang-format off
    template <
        typename U
    >
    auto to_value(std::set<U>& set_ref);
    // clang-format on

    // clang-format off
    template <typename Tag_>
    auto add_tag(Tag_ tag);
    // clang-format on

    // clang-format off
    template <
        detail::accepts_values<ValueInit> U
    >
    // clang-format on
    auto build(U type) {
        return type.add_value(std::move(*this));
    }

    const auto& get_value() const { return m_value; }
    const auto& get_tags() const { return m_tags; }

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = ValueInit<
        T,
        meta::List<Tag_, Tags...>
    >;
    // clang-format on

    T m_value;
    std::tuple<Tags...> m_tags;
};

// clang-format off
template <
    typename T,
    typename... Tags
>
struct DefaultValueInit : public ValueInit<
    T,
    meta::List<Tags...>
>
// clang-format on
{
    // clang-format off
    explicit DefaultValueInit(
        T&& value,
        std::tuple<Tags...>&& tags = {}
    ): ValueInit<T, meta::List<Tags...>>{std::move(value), std::move(tags)} {}
    // clang-format on

    // clang-format off
    template <
        detail::accepts_default_values<DefaultValueInit> U
    >
    // clang-format on
    auto build(U type) {
        return type.set_default_value(std::move(*this));
    }
};

// clang-format off
template <
    typename T, 
    typename... Ts
>
extern auto value(T value, Ts... builders);
// clang-format on

// clang-format off
template <
    typename T, 
    typename... Ts
>
extern auto default_value(T value, Ts... builders);
// clang-format on

} // namespace core::schema

#include "value.tpp"