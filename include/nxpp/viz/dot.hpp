#pragma once

/**
 * @file dot.hpp
 * @brief Graphviz DOT export helpers.
 *
 * @defgroup nxpp_viz Visualization
 * @brief Optional graph export helpers that do not require Graphviz at build time.
 */

#include "../graph.hpp"

#include <any>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace nxpp::viz {

/// Graphviz layout engines commonly used to render DOT output.
enum class DotLayout {
    Dot,
    Neato,
    Fdp,
    Sfdp,
    Circo
};

/// Controls the lightweight DOT export surface.
struct DotOptions {
    bool show_node_labels = true;
    bool show_edge_labels = true;
    bool show_weights = true;
    bool show_edge_ids = false;
    bool show_user_attrs = false;
    std::map<std::string, std::string> graph_attrs;
    std::optional<DotLayout> layout = std::nullopt;
    std::string graph_name = "G";
};

namespace detail {

inline std::string quote_dot_string(const std::string& value) {
    std::string out = "\"";
    for (char c : value) {
        if (c == '\\' || c == '"') {
            out += '\\';
        }
        out += c;
    }
    out += '"';
    return out;
}

inline bool is_plain_dot_id(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    const auto first = static_cast<unsigned char>(value.front());
    if (!(std::isalpha(first) || value.front() == '_')) {
        return false;
    }
    for (char c : value) {
        const auto ch = static_cast<unsigned char>(c);
        if (!(std::isalnum(ch) || c == '_')) {
            return false;
        }
    }
    return true;
}

inline bool is_dot_number(const std::string& value) {
    bool has_digit = false;
    bool has_exponent = false;
    bool has_dot = false;

    for (std::size_t i = 0; i < value.size(); ++i) {
        const char c = value[i];
        if (std::isdigit(static_cast<unsigned char>(c))) {
            has_digit = true;
            continue;
        }
        if (c == '-' && i == 0) {
            continue;
        }
        if ((c == '+' || c == '-') && i > 0 && (value[i - 1] == 'e' || value[i - 1] == 'E')) {
            continue;
        }
        if (c == '.' && !has_dot && !has_exponent) {
            has_dot = true;
            continue;
        }
        if ((c == 'e' || c == 'E') && has_digit && !has_exponent) {
            has_exponent = true;
            has_digit = false;
            continue;
        }
        return false;
    }

    return has_digit;
}

inline std::string graph_id(const std::string& value) {
    return is_plain_dot_id(value) ? value : quote_dot_string(value);
}

inline std::string attr_value(const std::string& value) {
    return is_plain_dot_id(value) || is_dot_number(value) ? value : quote_dot_string(value);
}

inline const char* dot_layout_name(DotLayout layout) {
    switch (layout) {
    case DotLayout::Dot:
        return "dot";
    case DotLayout::Neato:
        return "neato";
    case DotLayout::Fdp:
        return "fdp";
    case DotLayout::Sfdp:
        return "sfdp";
    case DotLayout::Circo:
        return "circo";
    }
    return "dot";
}

using DotAttrList = std::vector<std::pair<std::string, std::string>>;

inline bool has_dot_attr(const DotAttrList& attrs, const std::string& key) {
    return std::any_of(
        attrs.begin(),
        attrs.end(),
        [&](const auto& attr) { return attr.first == key; }
    );
}

inline void append_dot_attr(DotAttrList& attrs, const std::string& key, const std::string& value) {
    if (!has_dot_attr(attrs, key)) {
        attrs.emplace_back(key, value);
    }
}

inline std::string format_dot_attrs(const DotAttrList& attrs) {
    std::string out;
    for (const auto& [key, value] : attrs) {
        if (!out.empty()) {
            out += " ";
        }
        out += graph_id(key) + "=" + attr_value(value);
    }
    return out;
}

template <typename T>
std::string to_string(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

template <typename T>
std::optional<std::string> any_attr_as_string(const std::any& value) {
    if (const auto* typed = std::any_cast<T>(&value)) {
        return to_string(*typed);
    }
    return std::nullopt;
}

inline std::string any_attr_value(const std::any& value) {
    if (const auto* typed = std::any_cast<std::string>(&value)) {
        return *typed;
    }
    if (const auto* typed = std::any_cast<bool>(&value)) {
        return *typed ? "true" : "false";
    }

    if (auto converted = any_attr_as_string<short>(value)) return *converted;
    if (auto converted = any_attr_as_string<unsigned short>(value)) return *converted;
    if (auto converted = any_attr_as_string<int>(value)) return *converted;
    if (auto converted = any_attr_as_string<unsigned int>(value)) return *converted;
    if (auto converted = any_attr_as_string<long>(value)) return *converted;
    if (auto converted = any_attr_as_string<unsigned long>(value)) return *converted;
    if (auto converted = any_attr_as_string<long long>(value)) return *converted;
    if (auto converted = any_attr_as_string<unsigned long long>(value)) return *converted;
    if (auto converted = any_attr_as_string<float>(value)) return *converted;
    if (auto converted = any_attr_as_string<double>(value)) return *converted;
    if (auto converted = any_attr_as_string<long double>(value)) return *converted;

    throw std::runtime_error("DOT export failed: unsupported user attribute type.");
}

} // namespace detail

/**
 * @brief Serializes a graph to Graphviz DOT.
 *
 * The visualization layer uses public node IDs directly as quoted DOT node IDs.
 * For weighted graphs, the built-in weight can be emitted as both `weight` and
 * `label` so Graphviz layout tools retain weight semantics and SVG output also
 * shows the numeric weight.
 */
template <
    typename NodeID,
    typename EdgeWeight,
    bool Directed,
    bool Multi,
    bool Weighted,
    typename OutEdgeSelector,
    typename VertexSelector
>
std::string to_dot(
    const Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>& graph,
    const DotOptions& options = {}
) {
    const char* graph_keyword = Directed ? "digraph" : "graph";
    const char* edge_operator = Directed ? " -> " : " -- ";

    std::ostringstream out;
    out << graph_keyword << " " << detail::graph_id(options.graph_name) << " {\n";
    detail::DotAttrList graph_attrs;
    if (options.layout.has_value()) {
        detail::append_dot_attr(graph_attrs, "layout", detail::dot_layout_name(*options.layout));
    }
    for (const auto& [key, value] : options.graph_attrs) {
        detail::append_dot_attr(graph_attrs, key, value);
    }
    for (const auto& [key, value] : graph_attrs) {
        out << "  " << detail::graph_id(key) << "=" << detail::attr_value(value) << ";\n";
    }

    for (const auto& node : graph.nodes()) {
        const std::string node_text = detail::to_string(node);
        out << "  " << detail::quote_dot_string(node_text);
        std::string attrs;
        if (options.show_node_labels) {
            attrs += "label=" + detail::quote_dot_string(node_text);
        }
        if (options.show_user_attrs) {
            for (const auto& [key, value] : graph.node_attrs(node)) {
                if (options.show_node_labels && key == "label") {
                    continue;
                }
                if (!attrs.empty()) {
                    attrs += " ";
                }
                attrs += detail::graph_id(key) + "=" + detail::attr_value(detail::any_attr_value(value));
            }
        }
        if (!attrs.empty()) {
            out << " [" << attrs << "]";
        }
        out << ";\n";
    }

    for (const auto edge_id : graph.edge_ids()) {
        const auto [source, target] = graph.get_edge_endpoints(edge_id);
        out << "  " << detail::quote_dot_string(detail::to_string(source))
            << edge_operator
            << detail::quote_dot_string(detail::to_string(target));

        detail::DotAttrList attrs;
        if constexpr (Weighted) {
            if (options.show_weights) {
                const std::string weight = detail::to_string(graph.get_edge_weight(edge_id));
                detail::append_dot_attr(attrs, "weight", weight);
                if (options.show_edge_labels) {
                    detail::append_dot_attr(attrs, "label", weight);
                }
            }
        }

        if (options.show_edge_ids) {
            detail::append_dot_attr(attrs, "edge_id", std::to_string(edge_id));
            if constexpr (!Weighted) {
                if (options.show_edge_labels) {
                    detail::append_dot_attr(attrs, "label", "edge_id=" + std::to_string(edge_id));
                }
            }
        }

        if (options.show_user_attrs) {
            for (const auto& [key, value] : graph.edge_attrs(edge_id)) {
                detail::append_dot_attr(attrs, key, detail::any_attr_value(value));
            }
        }

        if (!attrs.empty()) {
            out << " [" << detail::format_dot_attrs(attrs) << "]";
        }
        out << ";\n";
    }

    out << "}\n";
    return out.str();
}

/// Writes Graphviz DOT output to a filesystem path.
template <
    typename NodeID,
    typename EdgeWeight,
    bool Directed,
    bool Multi,
    bool Weighted,
    typename OutEdgeSelector,
    typename VertexSelector
>
void write_dot(
    const Graph<NodeID, EdgeWeight, Directed, Multi, Weighted, OutEdgeSelector, VertexSelector>& graph,
    const std::filesystem::path& output_path,
    const DotOptions& options = {}
) {
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("DOT export failed: could not open output file.");
    }
    out << to_dot(graph, options);
}

} // namespace nxpp::viz
