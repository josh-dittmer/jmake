#pragma once

#include "detail/common.h"
#include "detail/schema_error.h"
#include "detail/type_traits/holds_type.h" // IWYU pragma: keep
#include "detail/type_traits/is_type.h"
#include "tag.h"
#include "util/meta/list.h"
#include "util/meta/specialization_of.h"

#include <optional>
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
    detail::HoldsType<typename Type::out_type>::template Impl
>
// clang-format on
{
    static constexpr auto out_ref = OutRef;

    using out_ref_type = decltype(detail::deduce_member_type(OutRef));

    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::property_tag<Property, T>;

    static constexpr bool is_optional =
        meta::is_specialization_of_v<std::optional, out_ref_type>;

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
    detail::SchemaResult<typename Type::out_type> load(const Ts&... load_from) const;
    // clang-format on

    // clang-format off
    template <typename Tag_>
    auto add_tag(Tag_ tag);
    // clang-format on

    const auto& get_type() const { return m_type; }
    const auto& get_tags() const { return m_tags; }

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