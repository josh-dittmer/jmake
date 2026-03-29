#include "registry_test.h"

#include "core/schema/detail/schema_error.h"
#include "core/schema/generic_type.h"
#include "core/schema/schema.h"
#include "util/cmd/cmd_args.h"
#include "util/json/safe_parse.h"

#include <nlohmann/json.hpp>
#include <print>
#include <string>

struct NestedTest {
    std::string m_opt1;
    std::string m_opt2;
};

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
    NestedTest m_opt4;
    std::vector<NestedTest> m_opt5;
    std::map<std::string, NestedTest> m_opt6;
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

void test(int argc, char* argv[]) { // NOLINT
    // clang-format off
    using namespace core::schema;
    using namespace core::schema::tags;

    auto test_object = object<MainConfig>(
        property<&MainConfig::m_opt1>(
            tag<json_id>("opt1"),
            tag<name>("Option 1")
        ),
        property<&MainConfig::m_opt2>(
            array<std::string>(),
            tag<json_id>("opt2"),
            tag<name>("Option 2")
        ),
        property<&MainConfig::m_opt3>(
            dict(
                type<int>(
                    tag(min(1)),
                    tag(max(50))
                )
            ),
            tag<json_id>("opt3"),
            tag<name>("Option 3")
        ),
        property<&MainConfig::m_opt4>(
            object<NestedTest>(
                property<&NestedTest::m_opt1>(
                    type<std::string>(
                        default_value("default_1")
                    ),
                    tag<json_id>("opt1"),
                    tag<name>("Nested option 1")
                ),
                property<&NestedTest::m_opt2>(
                    type<std::string>(
                        default_value("default_2")
                    ),
                    tag<json_id>("opt2"),
                    tag<name>("Nested option 2")
                )
            ),
            tag<json_id>("nested"),
            tag<name>("Nested")
        ),
        property<&MainConfig::m_opt5>(
            array(object<NestedTest>(
                property<&NestedTest::m_opt1>(
                    tag<json_id>("opt1"),
                    tag<name>("Array option 1")
                ),
                property<&NestedTest::m_opt2>(
                    tag<json_id>("opt2"),
                    tag<name>("Array option 2")
                )
            )),
            tag<json_id>("nested_arr"),
            tag<name>("Array")
        ),
        property<&MainConfig::m_opt6>(
            dict(object<NestedTest>(
                property<&NestedTest::m_opt1>(
                    tag<json_id>("opt1"),
                    tag<name>("Dict option 1")
                ),
                property<&NestedTest::m_opt2>(
                    type<std::string>(
                        default_value("test_default_value")
                    ),
                    tag<json_id>("opt2"),
                    tag<name>("Dict option 1")
                )
            )),
            tag<json_id>("nested_dict"),
            tag<name>("Dict")
        )
    );

    /*auto test_object = object<MainConfig>(
        property<&MainConfig::m_opt1>(
            tag<json_id>("test")    
        )
    );*/

    auto cmd_args = CmdArgs::from_space_delimited(argc, argv);

    for (const auto& [key, val] : cmd_args.get()) {
        std::println("{}: {}", key, val.value_or("(none)"));
    }

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
        return;
    }

    auto test_out = test_object.to_out(res.unwrap());
    if (!test_out.ok()) {
        std::println("Error: {:t}", test_out.unwrap_err());
        return;
    }

    const auto& out = test_out.unwrap();

    std::println("m_opt1: {}", out.m_opt1);
    std::println("m_opt2: {}", out.m_opt2);
    std::println("m_opt3: {}", out.m_opt3);
    std::println("m_opt4:");
    std::println("\tm_opt1: {}", out.m_opt4.m_opt1);
    std::println("\tm_opt2: {}", out.m_opt4.m_opt2);
    std::println("m_opt5: [");
    for (const auto& i : out.m_opt5) {
        std::println("\t{{m_opt1: {},", i.m_opt1);
        std::println("\tm_opt2: {}}},", i.m_opt2);
    }
    std::println("]");
    std::println("m_opt6: {{");
    for (const auto& [k, v] : out.m_opt6) {
        std::println("\t{}: {{", k);
        std::println("\t\tm_opt1: {},", v.m_opt1);
        std::println("\t\tm_opt2: {}", v.m_opt2);
        std::println("\t}}");
    }
    std::println("}}");

    // clang-format on
}