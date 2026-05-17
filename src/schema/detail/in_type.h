#pragma once

#include <meta>

namespace schema::detail {

template <typename T> struct in_type;

template <typename T> using in_data_t = in_type<T>::type;

} // namespace schema::detail