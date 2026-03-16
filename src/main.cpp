#include "registry_test.h"
#include "util/result/result.h"

#include <print>

struct Test {
    Test() = default;
    Test(Test&&) { // NOLINT
        std::println("Moved!");
    }
    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;
    Test& operator=(Test&&) = default;

    ~Test() = default;
};

struct TestError {
    TestError() { // NOLINT
        std::println("Constructed!");
    }
    TestError(TestError&&) { // NOLINT
        std::println("Moved!");
    }
    TestError(const TestError&) = delete;
    TestError& operator=(const TestError&) = delete;
    TestError& operator=(TestError&&) = delete;
    ~TestError() = default;

    // std::unique_ptr<int> m_test;
    void something() {}
};

using TestResult = Result<Test, Error<TestError>>;

namespace {

Result<Test> call_two() { return ok(Test{}); }

} // namespace

int main() {
    auto res = call_two();

    if (!res.ok()) {
        std::println("Error: {}", res.unwrap_err());
        return -1;
    }

    test();

    return 0;
}