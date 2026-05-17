#pragma once

#include <meta>

namespace util::meta {

namespace detail {

template <auto... Values> struct replicator_type {
    template <typename F> constexpr void operator>>(F body) const {
        (body.template operator()<Values>(), ...);
    }
};

template <auto... Values>
inline constexpr replicator_type<Values...> replicator = {};

} // namespace detail

template <typename Range> consteval auto expand(Range range) {
    std::vector<std::meta::info> args;

    for (auto r : range) {
        args.push_back(std::meta::reflect_constant(r));
    }

    return std::meta::substitute(^^detail::replicator, args);
}

} // namespace util::meta