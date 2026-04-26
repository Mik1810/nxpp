#pragma once

/**
 * @file dot.hpp
 * @brief Graphviz DOT export helpers.
 *
 * @defgroup nxpp_viz Visualization
 * @brief Optional graph export helpers that do not require Graphviz at build time.
 */

#include "../graph.hpp"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

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
        if ((c == '+' || c == '-') && (i == 0 || value[i - 1] == 'e' || value[i - 1] == 'E')) {
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

template <typename T>
std::string to_string(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
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

    for (const auto& node : graph.nodes()) {
        const std::string node_text = detail::to_string(node);
        out << "  " << detail::quote_dot_string(node_text);
        if (options.show_node_labels) {
            out << " [label=" << detail::quote_dot_string(node_text) << "]";
        }
        out << ";\n";
    }

    for (const auto edge_id : graph.edge_ids()) {
        const auto [source, target] = graph.get_edge_endpoints(edge_id);
        out << "  " << detail::quote_dot_string(detail::to_string(source))
            << edge_operator
            << detail::quote_dot_string(detail::to_string(target));

        std::string attrs;
        if constexpr (Weighted) {
            if (options.show_weights) {
                const std::string weight = detail::to_string(graph.get_edge_weight(edge_id));
                const std::string formatted_weight = detail::attr_value(weight);
                attrs += "weight=" + formatted_weight;
                if (options.show_edge_labels) {
                    attrs += " label=" + formatted_weight;
                }
            }
        }

        if (options.show_edge_ids) {
            if (!attrs.empty()) {
                attrs += " ";
            }
            attrs += "edge_id=" + std::to_string(edge_id);
            if constexpr (!Weighted) {
                if (options.show_edge_labels) {
                    attrs += " label=" + detail::quote_dot_string("edge_id=" + std::to_string(edge_id));
                }
            }
        }

        if (!attrs.empty()) {
            out << " [" << attrs << "]";
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
