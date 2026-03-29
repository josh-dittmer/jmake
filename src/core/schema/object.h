#pragma once

#include "detail/common.h"
#include "detail/schema_error.h"
#include "detail/type_traits/holds_type.h" // IWYU pragma: keep
#include "detail/type_traits/is_type.h"
#include "tag.h"
#include "util/meta/list.h"
#include "util/meta/specialization_of.h"
#include "util/optional/optional.h"

#include <tuple>

namespace core::schema {

// clang-format off
template <
    typename OutType,
    typename TagsList = meta::List<>,
    typename PropertiesList = meta::List<>
>
// clang-format on
struct Object;

// clang-format off
template <typename T>
concept is_object = meta::is_specialization_of_v<Object, T>;
// clang-format on

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept object_tag = is_object<T> && tag_for<T, Tag>;
// clang-format on

// clang-format off
template <
    typename... Properties
>
// clang-format on
struct ObjectIn {
    static constexpr std::size_t size = sizeof...(Properties);

    std::tuple<typename Properties::in_type...> m_data;
    std::size_t m_num_set = 0;
};

} // namespace detail

// clang-format off
template <
    typename OutType,
    typename... Tags,
    typename... Properties
>
struct Object<
    OutType,
    meta::List<Tags...>,
    meta::List<Properties...>
> : public detail::ApplyTraits<
    Object<OutType, meta::List<Tags...>, meta::List<Properties...>>,
    detail::IsType,
    detail::HoldsType<
        OutType, 
        detail::ObjectIn<Properties...>
    >::template Impl
>
// clang-format on
{
    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::object_tag<Object, T>;

    Object() = default;
    Object(Object&&) noexcept = default;

    Object(const Object&) = default;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) = default;

    ~Object() = default;

    // clang-format off
    explicit Object(
        std::tuple<Tags...>&& tags,
        std::tuple<Properties...>&& properties
    )
        : m_tags(std::move(tags)), 
            m_properties(std::move(properties)) {}
    // clang-format on

    template <typename... Ts>
    // clang-format off
    auto load(const Ts&... load_from) const -> detail::SchemaResult<typename Object::in_type>;
    // clang-format on

    // clang-format off
    template <typename Tag_>
    auto add_tag(Tag_ tag);
    // clang-format on

    // clang-format off
    template <typename Property_>
    auto add_property(Property_ property);
    // clang-format on

    const auto& get_tags() const { return m_tags; }
    const auto& get_properties() const { return m_properties; }

    static void combine(Object::in_type& base, Object::in_type& input);
    static bool satisfied(const Object::in_type&);

    auto to_out(Object::in_type& base) const
        -> detail::SchemaResult<typename Object::out_type>;

    auto get_default_value() const -> Opt<typename Object::out_type>;

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = Object<
        OutType,
        meta::List<Tag_, Tags...>,
        meta::List<Properties...>
    >;
    // clang-format on

    template <typename Property_>
    // clang-format off
    using append_property = Object<
        OutType,
        meta::List<Tags...>,
        meta::List<Property_, Properties...>
    >;
    // clang-format on

    std::tuple<Tags...> m_tags;
    std::tuple<Properties...> m_properties;
};

// clang-format off
template <
    typename OutType,
    typename... Ts
>
// clang-format on
extern auto object(Ts... builders);

} // namespace core::schema

#include "object.tpp"