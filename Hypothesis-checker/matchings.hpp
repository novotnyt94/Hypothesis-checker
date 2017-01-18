#ifndef MATCHINGS_
#define MATCHINGS_

#include "settings.hpp"
#include "errors.hpp"
#include "containers.hpp"
#include "transformation.hpp"
#include <vector>
#include <array>
#include <unordered_set>


#ifdef __linux__
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace cube {
	/* Compress given perfect matching into llfi. */
	llfi compress(perfect_matching & source_matching);

	/* Decompress given llfi back into perfect matching structure. */
	perfect_matching decompress(llfi comprimed_matching);

	/* Compress partial matching with up to 11 edges into llfi. */
	llfi compress_partial(perfect_matching & source_matching);

	/* TODO: not required, not yet implemented */
	//perfect_matching decompress_partial(llfi comprimed_matching);

	/* Class for generating all possible non-isomorphic matchings and providing functions for transforming matchings with given isomorphism. */
	class matchings {
	public:
		/* Finds all possible non-isomorphic matchings of chosen dimension and number of edges from hypercube.
		   Then it saves them into public variable results. */
		static void find_matchings();

		/* Set of all perfect matchings generated. */
		static result_set results;

	private:
		//Main recursive function, tries adding edges one-by-one into partial matching while checking, whether that matching has been yet generated and fulfills the restrictions. 
		static void find_remaining_edges(const sfi count, const sfi first_empty);

		//Gets lexicographicaly minimal matching isomorphic to 'matching' and then inserts it into results.
		static void add_new_matching();

		//Returns whether the vertex_id is yet in some edge
		static bool is_set(const sfi vertex_id) {
			return (matching[vertex_id] != INVALID);
		}

		//Resets the matching
		static void clear_matching() {
			FOR_VERTICES(vertex_id) {
				matching[vertex_id] = INVALID;
			}
		}

		//Adds the selected edge into matching
		static void add_edge(const sfi first_vertex, const sfi second_vertex);
		
		//Removes the selected edge from matching
		static void remove_edge(const sfi first_vertex, const sfi second_vertex);

		//Generates lexicographicaly minimal matching to 'matching'. Works for partial matchings as well. 
		static perfect_matching lex_min_matching();

		//Tries to set all reasonable edges of matching to 0->k and then sets the lexicographicaly minimal as best_isomorphic 
		static void find_best_transformation();

		//Tries all possible permutations of coordinates to be used in an isomorphism
		static void generate_possible_transformations(const sfi count);
		
		//transforms matching by given transformation and if the result is lexicographicaly smaller than best_isomorphic, it will be replaced
		static void try_transformation();

		//Transforms vertex_id by transform_data
		static sfi transform(sfi vertex_id);

		//Returns vertex which will give vertex_id if transformed by transform_data
		static sfi inverse_transform(sfi vertex_id);

		//Sets transform_data to most-constricted possible after mapping base_edge to transformed_edge (ordering matters).
		static void get_transformation(const edge & base_edge, const edge & transformed_edge);
		
		//Returns true iff number1 and number2 have the same number of bits set to 1 (they have the same distance to some vertex
		static bool is_same_dist(const sfi number1, const sfi number2) {
			return (one_cnt[number1] == one_cnt[number2]);
		}

		/* Used data structures */

#ifdef PROGRESS_INFO
		//Number of generated perfect matchings
		static llfi cycles;
#endif // PROGRESS_INFO	

		// Perfect matching structure used globaly during the algorithm for storing actual state. 
		static perfect_matching matching;

		//Set of visited states - helps to reduce the branching factor
		static result_set partial_matchings;

		// Number of edges in actual partial matching from Q(d) 
		static sfi hyper_edges;

		//Matching after performing actual transformation
		static vertices_array actual_isomorphic;

		//Lexicographically minimal matching found
		static vertices_array best_isomorphic; 

		//Structure holding (partial) data about transformation, which would be used on current matching.
		static transformation transform_data;

		//Holds which coordinates were set while creating transform_data
		static dimension_states used_dimensions;
	};
}
#endif //MATCHINGS_