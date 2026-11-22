#pragma once

#include "detail/in_type.h"
#include "util/meta/template_of.h"

#include <meta>

namespace schema {

namespace detail {

// forward declaration
// clang-format off
template <
    typename T,
    std::meta::info Parent = {}
>
// clang-format on
struct in_object;

// clang-format off
template <
    std::meta::info OutMember
>
// clang-format on
struct Property {
  public:
    // old api (to be removed)
    constexpr static auto member = OutMember;
    using type = [:std::meta::type_of(OutMember):];
    using data_type = in_data_t<type>;
    using opt_type = std::optional<data_type>;

    // modern api
    using parent_t = typename[:std::meta::parent_of(OutMember):];
    using data_t = in_data_t<typename[:std::meta::type_of(OutMember):]>;

    constexpr static bool is_in_object =
        util::meta::template_of<^^in_object, std::meta::dealias(^^data_t)>;

    auto& get() { return m_data; }
    const auto& get() const { return m_data; }

    static consteval auto flatten()
        requires(is_in_object);

    static consteval auto flatten()
        requires(!is_in_object);

  private:
    std::optional<data_t> m_data;
};

} // namespace detail

template <std::meta::info Property> //
extern constexpr auto get_out_member();

template <std::meta::info Property> //
extern constexpr auto get_data_type();

} // namespace schema

#include "property.tpp"