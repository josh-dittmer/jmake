#pragma once

#include <utility>

namespace core::schema::detail {

// clang-format off
template <typename T, typename U>
concept builder_for = requires(T builder, U type) {
    builder.build(std::move(type));
};
// clang-format on

// clang-format off
template <
    typename Target
>
// clang-format on
inline auto apply_builders(Target target) {
    return target;
}

// clang-format off
template <
    typename Target, 
    builder_for<Target> Builder0
>
// clang-format on
inline auto apply_builders(Target target, Builder0 builder0) {
    return builder0.build(std::move(target));
}

// clang-format off
template <
    typename Target, 
    builder_for<Target> Builder0,
    typename... Rest
>
// clang-format on
inline auto apply_builders(Target target, Builder0 builder0, Rest... rest) {
    auto next_target = builder0.build(std::move(target));
    return apply_builders(std::move(next_target), std::move(rest)...);
}

// clang-format on

} // namespace core::schema::detail