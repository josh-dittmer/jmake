#pragma once

namespace schema::detail {

template <typename T> struct out_type;

template <typename T> using out_type_t = out_type<T>::type;

} // namespace schema::detail