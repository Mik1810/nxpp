#include "../../include/nxpp_wasm/common/js_convert.hpp"

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
