#pragma once

#include "object.h"

#include "array.h"        // IWYU pragma: keep
#include "context.h"      // IWYU pragma: keep
#include "generic_type.h" // IWYU pragma: keep

#include "detail/combiner.h"
#include "schema/detail/schema_error.h"

#include <meta>

namespace schema {

namespace detail {

// clang-format off
template <
    std::meta::info ClassType, 
    std::meta::info Object
>
// clang-format on
consteval auto create_object_in_type() {
    std::vector<std::meta::info> mem_specs;

    template for (constexpr auto mem : util::meta::nsdm_arr(Object)) {
        std::vector<std::meta::info> prop_args;
        prop_args.push_back(^^mem); // reflection of a reflection

        // clang-format off
        mem_specs.push_back(std::meta::data_member_spec(
            std::meta::substitute(^^Property, prop_args),
            { .name = std::meta::identifier_of(mem) }
        ));
        // clang-format on
    }

    return std::meta::define_aggregate(ClassType, mem_specs);
}

// clang-format off
template <typename T>
template <
    std::meta::info,
    typename... Context
>
// clang-format on
auto validator<in_object<T>>::validate(const std::tuple<Context...>& context,
                                       const type& data) -> SchemaResult<> {
    using data_t [[maybe_unused]] = type::data_type;

    template for (constexpr auto mem : util::meta::nsdm_arr(^^data_t)) {
        // get member type (Property<...>)
        using p_t = [:std::meta::type_of(mem):];
        using pd_t = p_t::data_type;

        constexpr auto out_member = p_t::member;

        const auto& value_opt = data.m_data.[:mem:].get();

        // if no value, skip
        if (!value_opt) {
            continue;
        }

        const auto& value = *value_opt;

        // validate the property
        auto val_res =
            validator<pd_t>::template validate<out_member>(context, value);

        if (!val_res) {
            // validation failed? trace the property
            constexpr auto prop_id = std::meta::identifier_of(out_member);

            push_bt(val_res, ObjectTrace{});
            push_bt(val_res, PropertyTrace{std::string{prop_id}});

            return err(val_res);
        }
    }

    return ok();
}

template <typename T>
void combiner<in_object<T>>::combine(type& base, const type& in) {
    using body_t [[maybe_unused]] = type::data_type;

    template for (constexpr auto mem : util::meta::nsdm_arr(^^body_t)) {
        // get member type (Property<...>)
        using p_t = [:std::meta::type_of(mem):];
        using pd_t = p_t::data_type;

        const auto& in_value_opt = in.m_data.[:mem:].get();
        auto& base_value_opt = base.m_data.[:mem:].get();

        // if no value, skip
        if (!in_value_opt) {
            continue;
        }

        const auto& in_value = *in_value_opt;

        // first value? set base
        if (!base_value_opt) {
            base_value_opt = std::move(in_value);
        }

        // otherwise, combine with previous
        else {
            combiner<pd_t>::combine(*base_value_opt, in_value);
        }
    }
}

template <typename T>
    requires std::is_aggregate_v<T>
auto in_type<T>::to_out(data_type in) -> SchemaResult<T> {
    using body_t [[maybe_unused]] = data_type::data_type;

    T output{};

    template for (constexpr auto mem : util::meta::nsdm_arr(^^body_t)) {
        // get member type (Property<...>)
        using p_t = [:std::meta::type_of(mem):];
        using po_t = p_t::type;

        constexpr auto out_mem = p_t::member;
        constexpr auto prop_id = std::meta::identifier_of(out_mem);

        constexpr bool has_default =
            std::meta::has_default_member_initializer(out_mem);

        constexpr bool is_optional =
            std::meta::has_template_arguments(std::meta::type_of(out_mem)) &&
            std::meta::template_of(std::meta::type_of(out_mem)) ==
                ^^std::optional;

        auto& value_opt = in.m_data.[:mem:].get();

        // check for value
        if (!value_opt) {
            // if default exists or is optional, continue
            if constexpr (has_default || is_optional) {
                continue;
            }

            // otherwise error out
            else {
                return err(ErrorInfo(
                    "missing required value",
                    {ObjectTrace{}, PropertyTrace{std::string{prop_id}}}));
            }
        }

        auto& value = *value_opt;

        // recursively convert to output type
        auto out_res = in_type<po_t>::to_out(std::move(value));
        if (!out_res) {
            // trace the property
            push_bt(out_res, ObjectTrace{});
            push_bt(out_res, PropertyTrace{std::string{prop_id}});

            return err(out_res);
        }

        output.[:out_mem:] = std::move(*out_res);
    }

    return output;
}

} // namespace detail

// clang-format off
template <
    typename... Context,
    typename T,
    typename... Ts
>
// clang-format on
detail::SchemaResult<T> load(const std::tuple<Context...>& context,
                             detail::in_object<T> in1,
                             Ts&&... rest) // NOLINT
    requires(std::convertible_to<Ts, detail::in_object<T>> && ...)
{
    using namespace detail;

    using data_t = detail::in_object<T>;
    constexpr auto null_refl = std::meta::info{};

    // validate first input
    HUH(validator<data_t>::template validate<null_refl>(context, in1));

    // merge in the rest of the inputs
    template for (auto&& l : {std::forward<Ts>(rest)...}) {
        // validate input
        HUH(validator<data_t>::template validate<null_refl>(context, l));

        // merge it into first input
        combiner<data_t>::combine(in1, l);
    }

    //  convert to out type and return
    return HUH(in_type<T>::to_out(std::move(in1)));
}

} // namespace schema