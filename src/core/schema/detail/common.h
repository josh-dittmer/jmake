#pragma once

#include <set>

namespace core::schema::detail {

// apply traits helper
template <typename Derived, template <typename> class... Traits>
struct ApplyTraits : Traits<Derived>... {};

template <typename M, typename C> M deduce_member_type(M C::*) { return M{}; }
template <typename M, typename C> C deduce_class_from_member(M C::*) {
    return C{};
}

template <typename T>
concept set_compatible = requires { std::set<T>{}; };

} // namespace core::schema::detail