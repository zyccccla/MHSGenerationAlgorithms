/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "shd-base.hpp"

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

#include <cassert>

namespace agdmhs {
    void update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const Hypergraph& T,
                               const bitset& S,
                               const hindex v) {
        /*
          Update crit[] and uncov to reflect S+v.
          (Assumes crit[] and uncov were correct for S.)

          NOTE: Raises a vertex_violating_exception if any w in S is not
          critical in S+v. In this case, the state of crit[] and uncov
          is undefined!
         */
        // Input specification
        assert(not S.test(v));

        // v is critical for edges it hits which were previously uncovered
        bitset v_edges = T[v];
        crit[v] = v_edges & uncov;

        // Remove anything v hits from uncov
        uncov -= v_edges;

        // Remove anything v hits from the other crit[w]s
        hindex w = S.find_first();
        while (w != bitset::npos) {
            crit[w] -= v_edges;
            if (crit[w].none()) {
                throw vertex_violating_exception();
            }
            w = S.find_next(w);
        }
    }

    void restore_crit_and_uncov(Hypergraph& crit,
                                bitset& uncov,
                                const Hypergraph& H,
                                const bitset& S,
                                const hindex v) {
        crit[v].reset();

        bitset edges = H.edges_containing_vertex(v);
        hindex e = edges.find_first();
        while (e != bitset::npos) {
            // If the vertex is in this edge, proceed
            bitset hitting_vertices = H[e] & S;
            hindex first_hit_vertex = hitting_vertices.find_first();
            if (first_hit_vertex == bitset::npos) {
                // No other vertex in S hits this edge
                uncov.set(e);
            } else if (hitting_vertices.find_next(first_hit_vertex) == bitset::npos) {
                // Exactly one other vertex in S hits this edge
                crit[first_hit_vertex].set(e);
            } // Otherwise, no change required

            e = edges.find_next(e);
        }
    }
}
