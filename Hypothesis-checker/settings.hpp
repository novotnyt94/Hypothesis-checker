#ifndef SETTINGS_
#define SETTINGS_

#include <cstdint>
#include <array>

/* File with all required user settings, global constants and type definitions. */
namespace cube {
	
	/*-----------------------------------------*/
	/* Definition of primitive data types used */
	/*-----------------------------------------*/

	//fastest 8-bit data type, used for encoding vertices numbers and for counting over related (small) objects
	typedef uint_fast8_t sfi; 

	//fastest 64-bit data type, used for counting possibly large numbers - e. g. number of generated matchings or to store compressed matchings.
	typedef uint_fast64_t llfi; 

	/*---------------------------------*/
	/* Available compile-time settings */
	/*---------------------------------*/

	//selected dimension n of hypercube - availble dimensions are 3, 4 and 5.
	static const sfi DIMENSION = 5; 

	//Number of required edges to be from hypercube in generated matchings
	static const sfi HYPER_EDGE_CNT = 16; 

	//Whether the input correctness should be checked - if the input is guaranted to be correct, disable would speed up loading, otherwise bad unchecked input could perform undefined behaviour.
	#define CHECK_INPUT

	//Whether information about calculation should be written to std::cout periodicaly (as follows)
	#define PROGRESS_INFO

	//Period of information about the count of generated perfect matchings. Has an effect only with PROGRESS_INFO
	static const llfi GENERATED_MATCHINGS_INFO = 1000;

	//Period of information about the count of solved matchings. Has an effect only with PROGRESS_INFO
	static const llfi GENERATED_PATHS_INFO = 100;

	/*-------------------------------------------------*/
	/* Dependent constants and complex data structures */
	/*-------------------------------------------------*/

	//Number of vertices in Q_n
	static const sfi VERTICES = 1 << DIMENSION;

	//Perfect matching size
	static const sfi MATCH_SIZE = VERTICES >> 1;

	//Mask for dimension bits
	static const sfi DIMENSION_BITS = VERTICES - 1;

	//Number of distinct pairs of path endings
	static const sfi BIPARTITE_PAIRS_CNT = MATCH_SIZE * (MATCH_SIZE - 1); 

	//Global number to express invalid value (e. g. neighbour of unmatched vertices)
	static const sfi INVALID = (sfi)(-1);

	//Oriented pair of vertices
	typedef std::pair<sfi, sfi> edge;
	
	//Container for storing a perfect matching, match[i] contains the neighbour of i.
	typedef std::array<sfi, VERTICES> perfect_matching;
	
	//Same container for different purposes
	typedef std::array<sfi, VERTICES> vertices_array;

	//Boolean values for all vertices
	typedef std::array<bool, VERTICES> vertices_states;
	
	//Container for some value for every dimension 
	typedef std::array<sfi, DIMENSION> dimension_array;

	//Container for some state value for every dimension 
	typedef std::array<bool, DIMENSION> dimension_states;
	
	/*-----------------------------------*/
	/* Other useful constants and macros */
	/*-----------------------------------*/

	//Pre-counted array of Hamming weight for numbers 0 to 63 - used for fast determination of distance of two vertices in hypercube (hamming[u^v]).
	static const std::array<sfi, 64> hamming = { 
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6
	}; 

	//Pre-counted array of indices of top non-zero bit for numbers 0 to 63.
	static const std::array<sfi, 64> top_one = { 
  INVALID, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
	};

	//macros for synoptical iteration over often used arrays
	#define FOR_VERTICES(var_name) for (sfi var_name = 0; var_name < VERTICES; var_name++)
	#define FOR_DIMENSION(var_name) for (sfi var_name = 0; var_name < DIMENSION; var_name++)
	#define FOR_EDGES(var_name) for (sfi var_name = 0; var_name < MATCH_SIZE; var_name++)
}
#endif //SETTINGS_