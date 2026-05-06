#pragma once

#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

namespace nxpp::test {

constexpr const char* green = "\033[32m";
constexpr const char* red = "\033[31m";
constexpr const char* reset = "\033[0m";

inline void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Fn>
void expect_throws(Fn&& fn, const std::string& message) {
    try {
        fn();
    } catch (const std::runtime_error&) {
        return;
    }

    throw std::runtime_error(message);
}

template <typename Fn>
void expect_runtime_error_message(Fn&& fn, const std::string& expected_message, const std::string& failure_message) {
    try {
        fn();
    } catch (const std::runtime_error& ex) {
        if (std::string(ex.what()) == expected_message) {
            return;
        }
        throw std::runtime_error(
            failure_message + ": expected \"" + expected_message + "\", got \"" + ex.what() + "\""
        );
    }

    throw std::runtime_error(failure_message + ": no std::runtime_error thrown");
}

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

inline bool run_test(const TestCase& test) {
    try {
        test.fn();
        std::cout << "[TEST] " << test.name << " | " << green << "PASS" << reset << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cout << "[TEST] " << test.name << " | " << red << "FAIL" << reset
                  << " (" << ex.what() << ")\n";
        return false;
    }
}

inline int run_tests(std::initializer_list<TestCase> tests) {
    int passed = 0;
    for (const auto& test : tests) {
        passed += run_test(test) ? 1 : 0;
    }
    return passed == static_cast<int>(tests.size()) ? 0 : 1;
}

} // namespace nxpp::test
