#pragma once

#include "detail/in_type.h"

#include <meta>

namespace schema {

namespace detail {

template <std::meta::info OutMember> //
struct Property {
  public:
    constexpr static auto member = OutMember;
    using type = [:std::meta::type_of(OutMember):];
    using data_type = in_data_t<type>;
    using opt_type = std::optional<data_type>;

    auto& get() { return m_data; }
    const auto& get() const { return m_data; }

  private:
    opt_type m_data;
};

} // namespace detail

template <std::meta::info Property> //
extern constexpr auto get_out_member();

} // namespace schema

#include "property.tpp"