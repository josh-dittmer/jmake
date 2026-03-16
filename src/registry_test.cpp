#include "registry_test.h"

#include "core/schema/detail/schema_error.h"
#include "core/schema/schema.h"
#include "util/json/safe_parse.h"

#include <nlohmann/json.hpp>
#include <print>
#include <string>

struct MainConfig { // NOLINT
    struct DatabaseConfig {
        std::string m_host;
        std::string m_username;
        std::string m_password;
    } m_database_config;

    struct LogConfig {
        int m_log_level;
        bool m_show_timestamp;
        bool m_show_thread;
    };

    LogConfig m_log_config;

    std::string m_opt1;
    std::vector<std::string> m_opt2;
    std::map<std::string, int> m_opt3;
};

struct SpecificCommandConfig1 {
    MainConfig m_parent;

    std::string m_opt1;
    std::string m_opt2;
};

struct SpecificCommandConfig2 {
    MainConfig m_parent;

    std::string m_opt1;
    std::string m_opt2;
};

struct EvenDeeper {
    SpecificCommandConfig1 m_parent;

    std::string m_opt;
};

/*struct Poop {
    static inline int count;

    Poop() { count = 0; };
    Poop(const Poop&) = delete;
    Poop(Poop&&) noexcept { std::println("{}", ++count); }
};*/

struct cmd_id {
    std::string m_data;
};

enum TestEnum : uint8_t { INVALID, ONE, TWO, THREE, FOUR };

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(TestEnum, {
                                           {INVALID, nullptr},
                                           {ONE, "one1"},
                                           {TWO, "two2"},
                                           {THREE, "three3"},
                                           {FOUR, "four4"},
                                       })

void test() {
    // clang-format off
    using namespace core::schema;
    using namespace core::schema::tags;

    /*
    
    auto my_schema = schema<...>(...);

    auto res = my_schema.load<Subconfig>(
        json(my_json),
        cmd(my_cmd),
        env(),
        defaults()
    );

    */

    /*auto test_obj = object<MainConfig>(
        property<&MainConfig::m_opt1>(
            dict(
                type<std::string>(
                    default_value("val1"),
                    value("val2"),
                    value("val3")
                )
            ),
            tag<json_id>("opt1"),
            tag<cmd_id>("--opt1")
        ),
        property<&MainConfig::m_opt2>(
            array(
                type<std::string>(
                    value("val1"),
                    value("val2"),
                    value("val3")
                )
            ),
            tag<json_id>("opt2"),
            tag<cmd_id>("--opt2")
        )
    );

    auto test_schema = schema<MainConfig>(
        property<&MainConfig::m_opt1>(
            array<std::string>()
        ),
        property<&MainConfig::m_opt2>(
            dict<std::string>()
        ),
        subschema<&SpecificCommandConfig1::m_parent>(
            subschema<&EvenDeeper::m_parent>(
                property<&EvenDeeper::m_opt>(
                    type<std::string>(
                        value("val1"),
                        value("val2"),
                        value("val3")
                    )
                )
            )
        )
    );*/

    auto test_object = object<MainConfig>(
        property<&MainConfig::m_opt1>(
            tag<json_id>("opt1")
        ),
        property<&MainConfig::m_opt2>(
            array<std::string>(),
            tag<json_id>("opt2")
        ),
        property<&MainConfig::m_opt3>(
            dict(
                type<int>(
                    tag(min(1)),
                    tag(max(50))
                )
            ),
            tag<json_id>("opt3")
        )
    );

    auto json1_res = util::json::safe_parse_file("./resources/test1.json");
    if (!json1_res.ok()) {
        std::println("JSON 1 failed: {}", json1_res.unwrap_err());
        return;
    }

    auto json2_res = util::json::safe_parse_file("./resources/test2.json");
    if (!json2_res.ok()) {
        std::println("JSON 2 failed: {}", json2_res.unwrap_err());
        return;
    }

    const auto& json1 = json1_res.unwrap();
    const auto& json2 = json2_res.unwrap();

    auto res = test_object.load(json2, json1);
    if (!res.ok()) {
        std::println("Error: {:t}", res.unwrap_err());
    } else {
        const auto& val = res.unwrap();

        std::println("{}", val.m_opt1);
        std::println("{}", val.m_opt2);
        std::println("{}", val.m_opt3);
    }
    // clang-format on
}