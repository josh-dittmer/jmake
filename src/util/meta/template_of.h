#pragma once

#include <meta>

namespace util::meta {

template <std::meta::info T, std::meta::info R>
concept template_of =
    std::meta::has_template_arguments(R) && std::meta::template_of(R) == T;

}