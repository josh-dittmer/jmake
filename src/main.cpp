#include "schema/loaders/glaze/glaze_loader.h"
#include "schema/object.h"
#include "util/file/io.h"
#include "util/str/formatters/optional.h" // IWYU pragma: keep
#include "util/str/formatters/universal.h"

#include <glaze/glaze.hpp>

#include <meta>
#include <print>
#include <type_traits>

using namespace std::string_view_literals;

struct cmd_id : schema::string_tag {
    using string_tag::string_tag;
};

struct help_str : schema::string_tag {
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

struct Parent {
    std::string m_test;
};

// clang-format off
struct Test2 {
    [[= schema::glz::id("test_rename"sv) ]]
    [[= schema::range(1, 5) ]]
    int m_field;

    std::optional<std::vector<int>> m_field2;
};
// clang-format on

// clang-format off
struct Test {
    [[= schema::glz::id("123"sv), = cmd_id("--cmd-id"sv) ]]
    std::string m_opt1;

    [[= schema::range(1, 10) ]]
    int m_opt2;

    [[= schema::range(1, 5) ]]
    std::vector<int> m_opt3;

    std::optional<Test2> m_opt4;

    struct Nested {
        [[= schema::glz::id("test_rename"sv) ]]
        std::string m_nested_1;

        [[= schema::range(1, 10) ]]
        int m_nested_2 = 5;

        [[= schema::range(1, 8)]]
        std::vector<int> m_nested_3;
    } m_nested;

    std::vector<Test2> m_obj_array;
};
// clang-format on

template <typename T>
    requires std::is_aggregate_v<T>
struct std::formatter<T> : universal_formatter<T> {};

using in_t = schema::detail::in_type<Test>;
using in_object_t = schema::detail::in_object<Test>;

namespace {

Result<in_object_t> read_test(std::string_view path) {
    const auto& file = HUH(util::file::read<std::string>(path));

    auto json_res = glz::read_json<in_object_t>(file);
    if (!json_res) {
        return err(glz::format_error(json_res.error()));
    }

    return *json_res;
}

void print_test(const Test& test) { std::println("{:n}", test); }

schema::detail::SchemaResult<> load_tests() {
    auto test1 = HUH(read_test("./resources/test1.json"));
    auto test2 = HUH(read_test("./resources/test2.json"));

    auto test_loaded = HUH(schema::load(schema::context(), test2, test1));

    print_test(test_loaded);

    return ok();
}

} // namespace

int main() {
    auto test_res = load_tests();
    if (!test_res) {
        std::println("Error: {}", test_res.error());
        return -1;
    }

    return 0;
}