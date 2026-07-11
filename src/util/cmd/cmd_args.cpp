#include "cmd_args.h"

#include "util/str/split.h"

namespace util::cmd {

Args Args::from_space_delimited(std::size_t argc,
                                char* argv[]) { // NOLINT
    DataType data;

    for (std::size_t i = 1; i < argc; i++) {
        std::string arg(argv[i]);

        if (i + 1 >= argc) {
            data.insert({arg, std::nullopt});
        }

        else {
            std::string val(argv[i + 1]);

            if (!val.empty() && val[0] != '-') {
                data.insert({arg, val});
                i++;
            }

            else {
                data.insert({arg, std::nullopt});
            }
        }
    }

    return Args(std::move(data));
}

Args Args::from_equals_delimited(std::size_t argc,
                                 char* argv[]) { // NOLINT
    DataType data;

    for (std::size_t i = 1; i < argc; i++) {
        std::string arg(argv[i]);

        auto split = util::str::split(arg, '=');

        if (split.size() < 2) {
            data.insert({arg, std::nullopt});
        }

        else {
            data.insert({split[0], split[1]});
        }
    }

    return Args(std::move(data));
}

Opt<Args::ValueType> Args::pop(const std::string& key) {
    auto mit = m_data.find(key);
    if (mit == m_data.end()) {
        return std::nullopt;
    }

    m_data.erase(mit);
    return mit->second;
}

} // namespace util::cmd