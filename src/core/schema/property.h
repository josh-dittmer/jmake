#pragma once

#include "detail/common.h"
#include "detail/schema_error.h"
#include "detail/type_traits/holds_type.h" // IWYU pragma: keep
#include "detail/type_traits/is_type.h"
#include "tag.h"
#include "util/meta/list.h"
#include "util/optional/optional.h"

#include <tuple>

namespace core::schema {

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename TagsList = meta::List<>
>
// clang-format on
struct Property;

namespace detail {

// clang-format off
template <
    typename T, 
    typename Property
>
concept accepts_properties = requires(T type, Property property) {
    type.add_property(std::move(property));
};
// clang-format on

// clang-format off
template <typename>
struct is_property_impl : std::false_type {};
// clang-format on

// clang-format off
template <auto OutRef, typename... Ts>
struct is_property_impl<Property<OutRef, Ts...>> : std::true_type {};
// clang-format on

} // namespace detail

// clang-format off
template <typename T>
// clang-format on
concept is_property = detail::is_property_impl<T>::value;

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept property_tag = is_property<T> && tag_for<T, Tag>;
// clang-format on

// clang-format off
template <
    auto OutRef,
    typename Type
>
// clang-format on
struct PropertyIn {
    static constexpr auto out_ref = OutRef;

    Opt<typename Type::in_type> m_data;
};

} // namespace detail

// clang-format off
template <
    auto OutRef,
    typename Type,
    typename... Tags
>
struct Property<
    OutRef,
    Type,
    meta::List<Tags...>
> : public detail::ApplyTraits<
    Property<OutRef, Type, meta::List<Tags...>>,
    detail::HoldsType<
        typename Type::out_type,
        detail::PropertyIn<OutRef, Type>
    >::template Impl
>
// clang-format on
{
    static constexpr auto out_ref = OutRef;

    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::property_tag<Property, T>;

    Property() = default;
    Property(Property&&) noexcept = default;

    Property(const Property&) = default;
    Property& operator=(const Property&) = default;
    Property& operator=(Property&&) = default;

    ~Property() = default;

    // clang-format off
    explicit Property(
        Type&& type,
        std::tuple<Tags...>&& tags = {}
    )
        : m_type(std::move(type)), 
            m_tags(std::move(tags)) {}
    // clang-format on

    template <typename... Ts>
    // clang-format off
    auto load(const Ts&... load_from) const -> detail::SchemaResult<typename Property::in_type>;
    // clang-format on

    // clang-format off
    template <typename Tag_>
    auto add_tag(Tag_ tag);
    // clang-format on

    const auto& get_type() const { return m_type; }
    const auto& get_tags() const { return m_tags; }

    auto to_out(typename Property::in_type& base) const
        -> detail::SchemaResult<typename Property::out_type>;

    auto get_default_value() const -> Opt<typename Property::out_type>;

    // clang-format off
    template <
        detail::accepts_properties<Property> T
    >
    // clang-format on
    auto build(T type) {
        return type.add_property(std::move(*this));
    }

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = Property<
        OutRef,
        Type,
        meta::List<Tag_, Tags...>
    >;
    // clang-format on

    Type m_type;
    std::tuple<Tags...> m_tags;
};

// clang-format off
template <
    auto OutRef,
    detail::is_type Type,
    typename... Ts
>
// clang-format on
extern auto property(Type type = Type{}, Ts... builders);

// clang-format off
template <
    auto OutRef,
    typename... Ts
>
// clang-format on
extern auto property(Ts... builders);

} // namespace core::schema

#include "property.tpp"