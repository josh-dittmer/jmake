#include "schema/loaders/cmd/cmd_loader.h"
#include "schema/loaders/glaze/glaze_loader.h"
#include "schema/object.h"
#include "util/cmd/cmd_args.h"
#include "util/file/io.h"
#include "util/str/formatters/optional.h" // IWYU pragma: keep
#include "util/str/formatters/universal.h"

#include <glaze/glaze.hpp>

#include <meta>
#include <print>
#include <type_traits>

using namespace std::string_view_literals;

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
                    return "Runtime-dependent"sv;
                case ExampleContext::Language::SPANISH:
                    return "Dependiente del tiempo de ejecución"sv;
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
    [[= schema::glz::rename("test_rename") ]]
    [[= schema::range(1, 5)                ]]
    int m_field;

    std::optional<std::vector<int>> m_field2;
};
// clang-format on

// clang-format off
struct Test {
    [[= schema::glz::rename("123")   ]]
    [[= schema::cmd::rename("--opt1")]]
    std::string m_opt1;

    [[= schema::range(1, 10)   ]]
    [[= schema::cmd::disable{} ]]
    int m_opt2;

    [[= schema::range(1, 5) ]]
    std::vector<int> m_opt3;

    std::optional<Test2> m_opt4;

    struct Nested {
        [[= schema::glz::rename("test_rename") ]]
        std::string m_nested_1 = "none";

        [[= schema::cmd::rename("--nested2") ]]
        [[= schema::range(1, 10) ]]
        int m_nested_2 = 5;

        [[= schema::range(1, 20)]]
        std::optional<std::vector<int>> m_nested_3;

        struct DoubleNest {
            [[= schema::cmd::rename("--nested3")]]
            std::string m_dn_1;
        } m_nest2;
    } m_nested;

    std::vector<Test2> m_obj_array;

    int m_visible = 69;
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

schema::detail::SchemaResult<> load_tests(util::cmd::Args& args) {
    auto test1 = HUH(read_test("./resources/test1.json"));
    auto test2 = HUH(read_test("./resources/test2.json"));
    auto test3 = HUH(schema::cmd::cmd_loader<Test>::from(args));

    auto test_loaded = HUH(schema::load(test1, test2, test3));

    print_test(test_loaded);

    return ok();
}

} // namespace

int main(int argc, char* argv[]) {
    auto args = util::cmd::Args::from_space_delimited(argc, argv);

    auto test_res = load_tests(args);
    if (!test_res) {
        std::println("Error: {:t}", test_res.error());
        return -1;
    }

    return 0;
}