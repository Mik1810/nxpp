#pragma once

/**
 * @file storage.hpp
 * @brief Public storage selector aliases for advanced graph configuration.
 */

#include <boost/graph/adjacency_list.hpp>

namespace nxpp::storage {

using Vec = boost::vecS;   ///< Default dense selector used by nxpp graph aliases.
using List = boost::listS; ///< Stable descriptors with slower index-oriented lookup.
using Set = boost::setS;   ///< Unique out-edges only; incompatible with Multi=true.

} // namespace nxpp::storage
