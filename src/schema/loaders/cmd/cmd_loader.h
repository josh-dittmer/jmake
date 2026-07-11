#pragma once

#include "schema/object.h"
#include "schema/tag.h"

#include "util/cmd/cmd_args.h"
#include <meta>

namespace schema::cmd {

namespace detail {

// cmd flag tranformer
struct flag_transformer {
    template <std::meta::info Member> static constexpr std::string transform() {
        return "--" + std::string{std::meta::identifier_of(Member)};
    }
};

} // namespace detail

// cmd tags

// renames a command line flag from it's default
struct rename : string_tag {
    using string_tag::string_tag;
};

// disable a member from being settable via a command line flag
struct disable {};

// cmd loader
template <typename T> struct cmd_loader {
    static Result<schema::detail::in_object<T>> from(util::cmd::Args& args);

  private:
    using in_t = schema::detail::in_object<T>;
    using setter_fn = Result<> (*)(in_t&, const std::string&);

    static consteval auto create_flag_array();
    static auto create_flag_map();

    template <std::meta::info... Path> //
    static Result<> field_setter(in_t& obj, const std::string& value);
};
} // namespace schema::cmd

#include "cmd_loader.tpp"