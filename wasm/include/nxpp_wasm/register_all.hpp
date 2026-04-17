#pragma once

#include "attributes.hpp"
#include "centrality.hpp"
#include "components.hpp"
#include "flow.hpp"
#include "generators.hpp"
#include "graph.hpp"
#include "multigraph.hpp"
#include "sat.hpp"
#include "shortest_paths.hpp"
#include "spanning_tree.hpp"
#include "topological_sort.hpp"
#include "traversal.hpp"

namespace nxpp_wasm {

inline void register_all_bindings() {
    register_graph_bindings();
    register_multigraph_bindings();
    register_attributes_bindings();
    register_traversal_bindings();
    register_shortest_paths_bindings();
    register_topological_sort_bindings();
    register_spanning_tree_bindings();
    register_components_bindings();
    register_centrality_bindings();
    register_flow_bindings();
    register_generators_bindings();
    register_sat_bindings();
}

} // namespace nxpp_wasm
