#pragma once

#include "core/schema/array.h"
#include "core/schema/dict.h"
#include "core/schema/generic_type.h"
#include "core/schema/loader/loader.h"
#include "core/schema/object.h"
#include "core/schema/property.h"
#include "util/cmd/cmd_args.h"

namespace core::schema {

namespace tags {

struct cmd_id {
    std::string m_id;
};

} // namespace tags

template <> struct Loader<CmdArgs> {};

} // namespace core::schema

#include "cmd_loader.tpp"