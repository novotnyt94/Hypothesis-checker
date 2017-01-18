#ifndef PATHFINDING_
#define PATHFINDING_

#include "settings.hpp"
#include "path.hpp"
#include "errors.hpp"
#include "matchings.hpp"
#include <algorithm>
#include <array>
#include <vector>
#include <iostream>

namespace cube {
	/* Provides functions to find paths for any set of perfect matching in B(2^d) for all pair of vertices in opposite partities as its endpoints. Includes analysis of failed cases in respect to the hypothesis. */
	class pathfinding {
	public:
		/* Tries to find Hamiltonian paths for every pair of vertices in opposite partities (except the connected ones) composed of given perfect matching and edges from Q(d) for all matchings given as a parameter.
		   Returns an array of all results, either found path (has_solution=true) or information about failure (has_solution=false). Tests the failures if they fulfill the hypothesis.
		   Warning: The result size is much larger than input, thus this function is not suitable for all posible matchings. */
		static std::vector<path> find_paths(const result_set & matchings);

		/* Tries to find Hamiltonian paths for every pair of vertices in opposite partities (except the connected ones) composed of given perfect matching and edges from Q(d) for all matchings given as a parameter.
		   Returns an array of all results of those matchings, in which at least one failure was found. Tests the failures if they fulfill the hypothesis. */
		static std::vector<path> find_unsolved_paths(const result_set & matchings);

	private:
		/* Sets all_parallel to true iff all edges of the matching crosses first dimension (all matchings in minimal form with at least one edge from hypercube has edge 0->1). */
		static void check_one_dimension(const perfect_matching & matching);

		/* Tests failed path to fulfill hypothesis. */
		static void check_hypothesis(const path & failed_path);

		/* Checks if the number of unsolved paths respects expected number if any failure was found for a matching. */
		static void number_check();

		/* Tries to find Hamiltonian path for given matching and ending vertices in actual_path. */
		static void solve();

		/* Analyses actual_path and prepares data used for search. */
		static void prepare_data();

		/* Adds selected edge to partial path and properly actualizes search data.
		   Component number of second_vertex will be changed to component number of first vertex. */
		static void add_edge(const sfi first_vertex, const sfi second_vertex);

		/* Removes selected edge from partial path and properly actualizes search data 
		   The component of second vertex will get new component number. */
		static void remove_edge(const sfi first_vertex, const sfi second_vertex);

		/* Tries to recursively find edges from Q(d) to complete the path.
		   Always selects the most constrained vertex to continue - this reduces the branching factor of search. */
		static void choose_next(sfi edges_added);


		/* Used data structures: */		

		/* Structure holding actual path with initial matching and ending vertices. */
		static path actual_path;

		/* Excipient structure holding number of component for each vertex in partial path. 
		Vertices in different components will have different numbers, but used numbers are not necessarily consequent. */
		static vertices_array vertex_component;

		/* Sizes of components - for index i, there will be the number of vertices with component number i in component_sizes[i]. */
		static std::array<sfi, MATCH_SIZE> component_sizes;

		/* Bitmap of all neighbours availble to be connected to vertex i - 
		   e.g. if neighbours_bitmap[i] = 9=(01001)_2, it means that add_edge(i, i XOR 1) and add_edge(i, i XOR 8) are the only possibilities how to continue from i.*/
		static vertices_array neighbours_bitmap;	

		/* Whether all edges in the matching the matching crosses same dimension - used for determining the type of the matching. */
		static bool all_parallel;

		/* The number of unsolved paths of actual matching. */
		static size_t bad_num;

		/* Total number of configurations violating hypothesis - must be zero for proving it. */
		static llfi counterexamples;

		/* The expected number of unsolved configurations when the input matching has type 1. */
		static const sfi TYPE1_CNT = MATCH_SIZE * ((MATCH_SIZE >> 1) - 1);
	};
}
#endif //PATHFINDING_