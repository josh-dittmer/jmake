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
    typename Type,
    typename TagsList = meta::List<>
>
// clang-format on
struct Array;

// clang-format off
template <typename T>
// clang-format on
concept is_array = meta::is_specialization_of_v<Array, T>;

namespace detail {

// clang-format off
template <typename T, typename Tag>
concept array_tag = is_array<T> && tag_for<T, Tag>;
// clang-format on

} // namespace detail

// clang-format off
template <
    typename Type,
    typename... Tags
>
struct Array<
    Type,
    meta::List<Tags...>
> : public detail::ApplyTraits<
    Array<Type, meta::List<Tags...>>,
    detail::IsType,
    detail::HoldsType<
        std::vector<typename Type::out_type>,
        std::vector<typename Type::in_type>
    >::template Impl
>
// clang-format on
{
    template <typename T>
    static constexpr bool has_tag = meta::has_type_v<T, Tags...>;

    template <typename T>
    static constexpr bool valid_tag = detail::array_tag<Array, T>;

    Array() = default;
    Array(Array&&) noexcept = default;

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;
    Array& operator=(Array&&) = delete;

    ~Array() = default;

    // clang-format off
    explicit Array(
        Type&& type,
        std::tuple<Tags...>&& tags = {}
    )
        : m_type(std::move(type)),
            m_tags(std::move(tags)) {}
    // clang-format on

    template <typename T>
    // clang-format off
    auto load(const T& load_from) const -> detail::SchemaResult<typename Array::in_type>;
    // clang-format on

    // clang-format off
    template <typename Tag_> 
    auto add_tag(Tag_ tag);
    // clang-format on

    const auto& get_type() const { return m_type; }
    const auto& get_tags() const { return m_tags; }

    static void combine(Array::in_type& base, Array::in_type& input);
    static bool satisfied(const Array::in_type&) { return false; }

    auto to_out(Array::in_type& base) const
        -> detail::SchemaResult<typename Array::out_type>;

    auto get_default_value() const -> Opt<typename Array::out_type>;

  private:
    template <typename Tag_>
    // clang-format off
    using append_tag = Array<
        Type,
        meta::List<Tag_, Tags...>
    >;
    // clang-format on

    Type m_type;
    std::tuple<Tags...> m_tags;
};

// clang-format off
template <
    detail::is_type Type,
    typename... Ts
>
// clang-format on
extern auto array(Type type, Ts... builders);

// clang-format off
template <
    typename T,
    typename... Ts
>
// clang-format on
extern auto array(Ts... builders);

} // namespace core::schema

#include "array.tpp"