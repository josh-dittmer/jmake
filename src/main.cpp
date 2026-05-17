#include "schema/rule.h"
#include "schema/tag.h"
#include "util/file/io.h"

#include <glaze/glaze.hpp>

#include <meta>
#include <print>

using namespace schema;
using namespace std::string_view_literals;

struct json_id : string_tag {
    using string_tag::string_tag;
};

struct cmd_id : string_tag {
    using string_tag::string_tag;
};

struct help_str : string_tag {
    using string_tag::string_tag;
};

struct ExampleContext {
    enum class Language : uint8_t { ENGLISH, SPANISH } m_lang;
};

enum class Key : uint8_t { OPT1 };

struct ExampleRegistry {
    static consteval auto get_help_str(Key key) {
        switch (key) {
        case Key::OPT1:
            return [](ExampleContext context) -> help_str {
                switch (context.m_lang) {
                case ExampleContext::Language::ENGLISH:
                    return "Runtime-dependent";
                case ExampleContext::Language::SPANISH:
                    return "Dependiente del tiempo de ejecución";
                }
            };
        };
    }
};

// clang-format off
struct Test {
    [[= json_id("123"sv), = cmd_id("--cmd-id"sv) ]]
    [[= rt(ExampleRegistry::get_help_str(Key::OPT1)) ]]
    std::string m_opt1;

    [[= range(1, 10) ]]
    int m_opt2;

    int m_opt3;
};
// clang-format on

int main() {
    auto file_res = util::file::read<std::string>("./resources/test1.json");
    if (!file_res) {
        std::println("Error: {:t}", file_res.error());
        return -1;
    }

    const auto& file = *file_res;

    auto json_res = glz::read_json<Test>(file);
    if (!json_res) {
        std::println("Error: {}", glz::format_error(json_res, file));
        return -1;
    }

    const auto& test = *json_res;

    std::println("m_opt1: {}", test.m_opt1);
    std::println("m_opt2: {}", test.m_opt2);

    // auto test1 = schema::load<Test>(test);

    // clang-format off

    // const auto& test2 = *test1;

    /*ExampleContext c{.m_lang = ExampleContext::Language::ENGLISH};

    constexpr auto test1 = get_tag<^^Test::m_opt1, json_id>();
    if constexpr (test1) {
        std::println("Value: {}", test1->m_str);
    } else {
        std::println("Value: none");
    }

    auto test2 = get_tag<^^Test::m_opt1, cmd_id>();
    if (test2) {
        std::println("Value: {}", test2->m_str);
    } else {
        std::println("Value: none");
    }

    auto test3 = get_tag<^^Test::m_opt1, help_str>(c);
    if (test3) {
        std::println("Value: {}", test3->m_str);
    } else {
        std::println("Value: none");
    }

    auto test4 = schema::load<Test>(3, c);
    if (test4) {
        std::println("yes");
    } else {
        std::println("Error: {}", test4.error());
    }*/

    return 0;
}