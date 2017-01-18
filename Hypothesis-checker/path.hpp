#ifndef PATH_
#define PATH_

#include "settings.hpp"

namespace cube {
	/* Holds data about generated path from matching with selected start_vertex and end_vertex*/
	struct path {
		/* Default constructor */
		path() = default;

		/* Generates path structure from perfect matching and endpoints. */
		path(perfect_matching matching, sfi start, sfi end) : base_matching(matching), start_vertex(start), end_vertex(end), has_solution(false) {

		}

		//Original perfect matching
		perfect_matching base_matching;

		//Selected start vertex
		sfi start_vertex;

		//Selected end vertex
		sfi end_vertex;

		//false until complement matching to create a Hamiltonian path (actually a Hamiltonian cycle, because edge start_vertex->end_vertex is also added) from base_matching is found.
		bool has_solution; 

		//Complement matching to base_matching creating together a Hamiltonian cycle. Undefined content until has_solution is set to true. 
		perfect_matching found_path;

	};

	
}
#endif //PATH_
