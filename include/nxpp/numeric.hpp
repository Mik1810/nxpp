#pragma once

/**
 * @file numeric.hpp
 * @brief Shared numeric conversion helpers for attribute-backed APIs.
 */

#include <any>
#include <optional>

namespace nxpp::detail {

template <typename T>
std::optional<double> try_numeric_any_cast_as_double(const std::any& value) {
    if (const auto* typed_value = std::any_cast<T>(&value)) {
        return static_cast<double>(*typed_value);
    }
    return std::nullopt;
}

inline std::optional<double> try_numeric_any_cast(const std::any& value) {
    if (auto converted = try_numeric_any_cast_as_double<int>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<long>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<long long>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<unsigned int>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<unsigned long>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<unsigned long long>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<float>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<double>(value)) return converted;
    if (auto converted = try_numeric_any_cast_as_double<long double>(value)) return converted;
    return std::nullopt;
}

} // namespace nxpp::detail
