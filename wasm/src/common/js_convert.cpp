#include "../../include/nxpp_wasm/common/js_convert.hpp"

#include <any>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace nxpp_wasm::common {

std::string js_type_of(const emscripten::val& value) {
    return value.typeOf().as<std::string>();
}

bool is_integral_number(const emscripten::val& value) {
    if (js_type_of(value) != "number") {
        return false;
    }

    const double numeric = value.as<double>();
    return ::isfinite(numeric)
        && std::floor(numeric) == numeric
        && numeric >= static_cast<double>(std::numeric_limits<int>::min())
        && numeric <= static_cast<double>(std::numeric_limits<int>::max());
}

bool is_string_value(const emscripten::val& value) {
    return js_type_of(value) == "string";
}

int as_int_node_id(const emscripten::val& value, const char* error_message) {
    if (!is_integral_number(value)) {
        throw std::runtime_error(error_message);
    }
    return value.as<int>();
}

std::string as_string_node_id(const emscripten::val& value, const char* error_message) {
    if (!is_string_value(value)) {
        throw std::runtime_error(error_message);
    }
    return value.as<std::string>();
}

double as_weight(const emscripten::val& value) {
    if (js_type_of(value) != "number") {
        throw std::runtime_error("WASM graph edge weights must be numeric.");
    }
    const double numeric = value.as<double>();
    if (!::isfinite(numeric)) {
        throw std::runtime_error("WASM graph edge weights must be finite.");
    }
    return numeric;
}

std::any as_attribute_value(const emscripten::val& value) {
    const std::string type = js_type_of(value);
    if (type == "string") {
        return std::any(value.as<std::string>());
    }
    if (type == "number") {
        const double numeric = value.as<double>();
        if (!::isfinite(numeric)) {
            throw std::runtime_error("WASM graph attribute values must be finite when numeric.");
        }
        return std::any(numeric);
    }
    if (type == "boolean") {
        return std::any(value.as<bool>());
    }
    throw std::runtime_error("WASM graph attribute values must be string, number, or boolean.");
}

emscripten::val to_js_attribute_value(const std::any& value) {
    if (const auto* string_value = std::any_cast<std::string>(&value)) {
        return emscripten::val(*string_value);
    }
    if (const auto* char_value = std::any_cast<const char*>(&value)) {
        return emscripten::val(std::string(*char_value));
    }
    if (const auto* bool_value = std::any_cast<bool>(&value)) {
        return emscripten::val(*bool_value);
    }
    if (const auto* int_value = std::any_cast<int>(&value)) {
        return emscripten::val(*int_value);
    }
    if (const auto* long_value = std::any_cast<long>(&value)) {
        return emscripten::val(static_cast<double>(*long_value));
    }
    if (const auto* long_long_value = std::any_cast<long long>(&value)) {
        return emscripten::val(static_cast<double>(*long_long_value));
    }
    if (const auto* float_value = std::any_cast<float>(&value)) {
        return emscripten::val(*float_value);
    }
    if (const auto* double_value = std::any_cast<double>(&value)) {
        return emscripten::val(*double_value);
    }
    if (value.type() == typeid(std::nullptr_t)) {
        return emscripten::val::null();
    }

    throw std::runtime_error("WASM graph attribute conversion failed: unsupported stored attribute type.");
}

emscripten::val to_js_array(const std::vector<int>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(values[i]));
    }
    return array;
}

emscripten::val to_js_array(const std::vector<std::string>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(values[i]));
    }
    return array;
}

emscripten::val to_js_array(const std::vector<std::size_t>& values) {
    emscripten::val array = emscripten::val::array();
    for (std::size_t i = 0; i < values.size(); ++i) {
        array.set(i, emscripten::val(static_cast<double>(values[i])));
    }
    return array;
}

} // namespace nxpp_wasm::common
