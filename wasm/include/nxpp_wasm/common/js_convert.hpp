#pragma once

#include <any>
#include <string>
#include <vector>

#include <emscripten/val.h>

namespace nxpp_wasm::common {

std::string js_type_of(const emscripten::val& value);

bool is_integral_number(const emscripten::val& value);
bool is_string_value(const emscripten::val& value);

int as_int_node_id(const emscripten::val& value, const char* error_message);
std::string as_string_node_id(const emscripten::val& value, const char* error_message);

double as_weight(const emscripten::val& value);
std::any as_attribute_value(const emscripten::val& value);
emscripten::val to_js_attribute_value(const std::any& value);

emscripten::val to_js_array(const std::vector<int>& values);
emscripten::val to_js_array(const std::vector<std::string>& values);
emscripten::val to_js_array(const std::vector<std::size_t>& values);

} // namespace nxpp_wasm::common
