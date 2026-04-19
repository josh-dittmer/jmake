#pragma once

#include "util/common_types/optional/optional.h"

#include <map>
#include <string>

namespace util::cmd {

class CmdArgs {
  public:
    using ValueType = Opt<std::string>;

    // stores args like this: arg: value?
    using DataType = std::map<std::string, ValueType>;

    static CmdArgs from_space_delimited(int argc, char* argv[]);  // NOLINT
    static CmdArgs from_equals_delimited(int argc, char* argv[]); // NOLINT

    Opt<ValueType> pop(const std::string& key);

    const DataType& get() const { return m_data; }

  private:
    explicit CmdArgs(DataType&& data) : m_data(std::move(data)) {}

    DataType m_data;
};

} // namespace util::cmd