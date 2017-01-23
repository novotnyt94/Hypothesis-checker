#include "pathfinding.hpp"

namespace cube {
	std::vector<path> pathfinding::find_paths(const result_set & matchings) {
		std::vector<path> results;

#ifdef PROGRESS_INFO
		llfi matchings_solved = 0; 
#endif //PROGRESS_INFO

		for (llfi block_id = 0; block_id < PRIME_MOD; block_id++) {
			for (auto it = matchings[block_id].begin(); it != matchings[block_id].end(); it++) {
				perfect_matching act_matching = decompress(*it);
				check_one_dimension(act_matching);
				bad_num = 0;
				FOR_VERTICES(start_vertex) {
					for (sfi end_vertex = start_vertex + 1; end_vertex < VERTICES; end_vertex++) {
						//If start_vertex and end_vertex are in opposite partities and not connected, try to find a path
						if (((one_cnt[start_vertex] - one_cnt[end_vertex]) & 1) == 1 && act_matching[start_vertex] != end_vertex) {
							actual_path = path(act_matching, start_vertex, end_vertex);
							solve();
							//Check hypothesis for the failure
							if (!actual_path.has_solution) {
								check_hypothesis(actual_path);
							}
							results.push_back(std::move(actual_path));
						}
					}
				}
				number_check();

#ifdef PROGRESS_INFO
				matchings_solved++; 
				if (matchings_solved % GENERATED_PATHS_INFO == 0)
					std::cout << matchings_solved << " matchings solved" << std::endl;
#endif //PROGRESS_INFO
			}
		}

		// Analysing the result
		if (counterexamples == 0) {
			std::cout << "No counterexample was found!"  << std::endl;
		}
		else {
			std::cout << "There were " << counterexamples << " counterexamples found, the hypothesis is wrong for dimension " << DIMENSION << std::endl;
		}

		return std::move(results);
	}

	std::vector<path> pathfinding::find_unsolved_paths(const result_set & matchings) {
		std::vector<path> results;
#ifdef PROGRESS_INFO
		int matchings_solved = 0;
#endif //PROGRESS_INFO
		for (llfi block_id = 0; block_id < PRIME_MOD; block_id++) {
			for (auto it = matchings[block_id].begin(); it != matchings[block_id].end(); it++) {
				perfect_matching act_matching = decompress(*it);
				bool OK = true;
				check_one_dimension(act_matching);
				bad_num = 0;
				for (sfi start_vertex = 0; start_vertex < VERTICES; start_vertex++) {
					for (sfi end_vertex = start_vertex + 1; end_vertex < VERTICES; end_vertex++) {
						//If start_vertex and end_vertex are in opposite partities and not connected, try to find a path
						if (((one_cnt[start_vertex] - one_cnt[end_vertex]) & 1) == 1 && act_matching[start_vertex] != end_vertex) {
							actual_path = path(act_matching, start_vertex, end_vertex);
							solve();
							//Check hypothesis for the failure
							if (!actual_path.has_solution) {
								OK = false;
								check_hypothesis(actual_path);
							}
							results.push_back(std::move(actual_path));
						}
					}
				}
				number_check();
				//If none failure was found, remove this matching from result
				if (OK) {
					for (int i = 0; i < BIPARTITE_PAIRS_CNT; i++) {
						results.pop_back();
					}
				}
#ifdef PROGRESS_INFO
				matchings_solved++;
				if (matchings_solved % GENERATED_PATHS_INFO == 0)
					std::cout << matchings_solved << " matchings solved" << std::endl;
#endif //PROGRESS_INFO
			}
		}

		// Analysing the result
		if (counterexamples == 0) {
			std::cout << "No counterexample was found!" << std::endl;
		}
		else {
			std::cout << "There were " << counterexamples << " counterexamples found, the hypothesis is wrong for dimension " << (size_t)DIMENSION << std::endl;
		}

		return std::move(results);
	}

	void pathfinding::check_one_dimension(const perfect_matching & matching) {
		FOR_VERTICES(vertex_id) {
			if (!((matching[vertex_id] ^ vertex_id) & 1)) {
				all_parallel = false;
				return;
			}
		}
		all_parallel = true;
	}

	void pathfinding::number_check() {
		if (bad_num > 0 && DIMENSION > 3) {
			if (all_parallel) {
				if (bad_num == TYPE1_CNT || bad_num == (TYPE1_CNT >> 1)) {
					std::cout << "Number OK (type 1): " << bad_num << std::endl;
				}
				else
					std::cout << "Matching type 1 failed: Unexpected number " << bad_num << std::endl;
			}
			else {
				if (bad_num == 1 || bad_num == 2) {
					std::cout << "Number OK (type 2): " << bad_num << std::endl;
				}
				else
					std::cout << "Matching type 2 failed: Unexpected number " << bad_num << std::endl;
			}
		}
		//for dimension 3, numbers of failures are not regular, therefore we will not test it
		else if (bad_num > 0) {
			std::cout << "Number OK: " << bad_num << std::endl;
		}
	}
	void pathfinding::check_hypothesis(const path & this_path) {
		bad_num++;
		//If all edges are parallel, ending vertices should be in opposite partities 
		if (all_parallel) {
			if (!(this_path.start_vertex ^ this_path.end_vertex) & 1) { 
				std::cout << "Matching type 1 failed: " << (size_t)this_path.start_vertex << ", " << (size_t)this_path.end_vertex << " are not in opposite dimensions." << std::endl;
				counterexamples++;
				return;
			}

		}
		//Else the matching created by removing edges from start_vertex and end_vertex and adding edge of their neighbours should be a half-layer.
		//Aditionally, there is a test if the added edge lies in Q(d). This is not required for the hypothesis, but it should hold and was confirmed for d=5.
		else {
			sfi start_neighbour = this_path.base_matching[this_path.start_vertex];
			sfi end_neighbour = this_path.base_matching[this_path.end_vertex];
			if (one_cnt[start_neighbour ^ end_neighbour] > 1) {
				std::cout << "Matching type 2 failed: Neighbours of " << (size_t)this_path.start_vertex << ", " << (size_t)this_path.end_vertex << " are not neighbours."<< std::endl;
				counterexamples++;
				return;
			}

			//test with switching edges
			sfi first_ignored = std::min(start_neighbour, this_path.start_vertex);
			sfi second_ignored = std::min(end_neighbour, this_path.end_vertex);
			sfi required_dim = start_neighbour ^ end_neighbour;
			FOR_VERTICES(vertex_id) {
				sfi other_vertex = this_path.base_matching[vertex_id];
				if (other_vertex > vertex_id && !((other_vertex ^ vertex_id) & required_dim)) {
					if (vertex_id != first_ignored && vertex_id != second_ignored) {
						std::cout << "Matching type 2 failed: Not a half-layer " << (size_t)this_path.start_vertex << ", " << (size_t)this_path.end_vertex << std::endl;
						counterexamples++;
						return;
					}
				}
			}
		}
	}

	void pathfinding::solve() {
		prepare_data();
		choose_next(0);

#ifdef _DEBUG 
		//Check that the result is really a path. 
		if (actual_path.has_solution) {
			if (actual_path.found_path[actual_path.start_vertex] != actual_path.end_vertex || actual_path.found_path[actual_path.end_vertex] != actual_path.start_vertex)
				errors::assert_error("Ending vertices are not start_vertex & end_vertex!");
			
			//Check that we got a Hamiltonian path with all edges used.
			sfi act_vertex = actual_path.start_vertex;
			for (sfi i = 0; i < (VERTICES >> 1) - 1; i++) {			
				act_vertex = actual_path.base_matching[act_vertex];
				if (act_vertex == actual_path.end_vertex)
					errors::assert_error("Some vertex is not on the path!");
				if (one_cnt[act_vertex ^ actual_path.found_path[act_vertex]] != 1)
					errors::assert_error("Vertices marked as neighbours on the path are not neighbours!");
				
				act_vertex = actual_path.found_path[act_vertex];
				if (act_vertex == actual_path.end_vertex)
					errors::assert_error("Some vertex is not on the path!");
			}

			if (actual_path.base_matching[act_vertex] != actual_path.end_vertex)
				errors::assert_error("The \"path\" has a cycle!");
		}
#endif // _DEBUG
	}

	void pathfinding::prepare_data() {

		sfi component_id = 0;
		//prepare vertex_component:
		FOR_VERTICES(vertex_id) {
			actual_path.found_path[vertex_id] = INVALID;
			if (actual_path.base_matching[vertex_id] > vertex_id) {
				sfi first_vertex = vertex_id;
				sfi second_vertex = actual_path.base_matching[vertex_id];
				//setting new component for this edge:
				vertex_component[first_vertex] = component_id;
				vertex_component[second_vertex] = component_id;
				component_id++;
				//setting neighbours_bitmap (if the edge is from Q(d), we cannot use that edge, otherwise we can use any edge):
				if (one_cnt[first_vertex ^ second_vertex] == 1) {
					neighbours_bitmap[first_vertex] = DIMENSION_BITS ^ (first_vertex ^ second_vertex);
					neighbours_bitmap[second_vertex] = DIMENSION_BITS ^ (first_vertex ^ second_vertex);
				}
				else {
					neighbours_bitmap[first_vertex] = DIMENSION_BITS;
					neighbours_bitmap[second_vertex] = DIMENSION_BITS;
				}
			}
		}
		//prepare component_sizes:
		for (sfi i = 0; i < MATCH_SIZE; i++) {
			component_sizes[i] = 2;
		}
		//adding "edge" start_vertex->end_vertex (simulates that start_vertex and end_vertex are not connectable)
		add_edge(actual_path.start_vertex, actual_path.end_vertex);
	}

	void pathfinding::add_edge(const sfi first_vertex, const sfi second_vertex) {
#ifdef _DEBUG
		//Check that adding this edge is valid
		if (vertex_component[first_vertex] == vertex_component[second_vertex])
			errors::assert_error("Chosen edge would connect same components!");
		if (actual_path.found_path[first_vertex] != INVALID || actual_path.found_path[second_vertex] != INVALID)
			errors::assert_error("Chosen vertex is not external!");
#endif //_DEBUG

		sfi new_component_id = vertex_component[first_vertex];
		sfi old_component_id = vertex_component[second_vertex];
		sfi old_component_size = component_sizes[old_component_id];

		//Component sizes actualization
		component_sizes[new_component_id] += old_component_size;
		component_sizes[old_component_id] = 0;		
		sfi actual_vertex = actual_path.base_matching[second_vertex];
		vertex_component[second_vertex] = new_component_id;
		vertex_component[actual_vertex] = new_component_id;

		//Change of component numbers for all vertices from second_vertex component
		while (actual_path.found_path[actual_vertex] != INVALID) {
			actual_vertex = actual_path.found_path[actual_vertex];
			vertex_component[actual_vertex] = new_component_id;
			actual_vertex = actual_path.base_matching[actual_vertex];
			vertex_component[actual_vertex] = new_component_id;	
		}

		for (sfi dim = 1; dim < VERTICES; dim <<= 1) {
			//Check if new endpoints of this component are neighbours
			if (vertex_component[actual_vertex ^ dim] == new_component_id && actual_path.found_path[actual_vertex ^ dim] == INVALID) {
				neighbours_bitmap[actual_vertex] &= ~dim;
				neighbours_bitmap[actual_vertex ^ dim] &= ~dim;
			}
		}
		//Removing external status from start_vertex, end_vertex 
		for (sfi dim = 1; dim < VERTICES; dim <<= 1) {
			neighbours_bitmap[first_vertex ^ dim] &= ~dim;
			neighbours_bitmap[second_vertex ^ dim] &= ~dim;
		}
		//Finally, add the edge into path
		actual_path.found_path[first_vertex] = second_vertex;
		actual_path.found_path[second_vertex] = first_vertex;

#ifdef _DEBUG
		//Check that component sizes were not corrupted
		vertices_array numbers;
		FOR_EDGES(component_id) {
			numbers[component_id] = 0;
		}
		FOR_VERTICES(vertex_id) {
			numbers[vertex_component[vertex_id]]++;
		}
		FOR_EDGES(component_id) {
			if (component_sizes[component_id] != numbers[component_id])
				errors::assert_error("numbers of vertices in components does not match the values in component_sizes!");
		}
#endif //_DEBUG
	}

	void pathfinding::remove_edge(const sfi first_vertex, const sfi second_vertex) {
		sfi old_component_id = vertex_component[first_vertex];
		sfi new_component_id = 1;
		//Selecting some free component_id (there is always at least one)
		while (component_sizes[new_component_id] > 0) 
			new_component_id++;
		sfi actual_vertex = actual_path.base_matching[second_vertex];
		vertex_component[second_vertex] = new_component_id;
		vertex_component[actual_vertex] = new_component_id;
		sfi length = 1;
		//Revert numbers of component to disjoint
		while (actual_path.found_path[actual_vertex] != INVALID) {		
			actual_vertex = actual_path.found_path[actual_vertex];
			vertex_component[actual_vertex] = new_component_id;
			actual_vertex = actual_path.base_matching[actual_vertex];
			vertex_component[actual_vertex] = new_component_id;
			length++;
		}
		//Switch component sizes to new state
		component_sizes[old_component_id] -= (length << 1);
		component_sizes[new_component_id] += (length << 1);

		//Remove the edge
		actual_path.found_path[first_vertex] = INVALID;
		actual_path.found_path[second_vertex] = INVALID;

		//Set neighbours_bitmap to new state
		//Enable edge (if exists) between ends of initial component 
		for (sfi dim = 1; dim < VERTICES; dim <<= 1) {
			if (actual_path.found_path[actual_vertex ^ dim] == INVALID && vertex_component[actual_vertex ^ dim] == old_component_id) {
				neighbours_bitmap[actual_vertex] |= dim;
				neighbours_bitmap[actual_vertex ^ dim] |= dim;
				//This edge is only one, thus we may break the search
				break;
			}
		}

		//Allow edges from start_vertex, end_vertex (we need to check that the edge is not in the same component)
		for (sfi dim = 1; dim < VERTICES; dim <<= 1) {
			if (actual_path.found_path[first_vertex ^ dim] == INVALID && vertex_component[first_vertex ^ dim] != old_component_id) {
				neighbours_bitmap[first_vertex] |= dim;
				neighbours_bitmap[first_vertex ^ dim] |= dim;
			}
			else
				neighbours_bitmap[first_vertex] &= ~dim;

			if (actual_path.found_path[second_vertex ^ dim] == INVALID && vertex_component[second_vertex ^ dim] != new_component_id) {
				neighbours_bitmap[second_vertex] |= dim;
				neighbours_bitmap[second_vertex ^ dim] |= dim;
			}
			else
				neighbours_bitmap[second_vertex] &= ~dim;
		}


#ifdef _DEBUG
		//Check that component sizes were not corrupted
		vertices_array numbers;
		FOR_EDGES(component_id) {
			numbers[component_id] = 0;
		}
		FOR_VERTICES(vertex_id) {
			numbers[vertex_component[vertex_id]]++;
		}
		FOR_EDGES(component_id) {
			if (component_sizes[component_id] != numbers[component_id])
				errors::assert_error("Numbers of vertices in components does not match the values in component_sizes!");
		}
#endif //_DEBUG
	}

	void pathfinding::choose_next(sfi edges_added) {
		//If done except the last edge, check it and then finish the path with it.
		if (edges_added == MATCH_SIZE - 2) { 
			sfi first_external;
			sfi second_external;
			sfi actual_vertex = 0;
			while (actual_path.found_path[actual_vertex] != INVALID)
				actual_vertex++;
			first_external = actual_vertex++;
			while (actual_path.found_path[actual_vertex] != INVALID)
				actual_vertex++;
			second_external = actual_vertex;
			if (one_cnt[first_external ^ second_external] != 1) 
				//Not neighbours, different path needed. 
				return;
			else { 
				//They are neighbours, thus we are done.
				actual_path.found_path[first_external] = second_external;
				actual_path.found_path[second_external] = first_external;
				actual_path.has_solution = true;
			}
		}
		else {
			//find the most constrained vertex
			sfi posibilities_count = DIMENSION; //larger than any possible result
			sfi best_vertex = 0;
			FOR_VERTICES(vertex_id) {
				if (actual_path.found_path[vertex_id] == INVALID && one_cnt[neighbours_bitmap[vertex_id]] < posibilities_count) {
					posibilities_count = one_cnt[neighbours_bitmap[vertex_id]];
					best_vertex = vertex_id;
				}
			}
			//Try to match it with all possible neighbours
			for (sfi neighbour = 1; neighbour < VERTICES; neighbour <<= 1) {
				if (neighbours_bitmap[best_vertex] & neighbour) {
					//select the lesser component to be renumbered
					sfi first_vertex, second_vertex;
					if (component_sizes[vertex_component[best_vertex]] >= component_sizes[vertex_component[best_vertex ^ neighbour]]) {
						first_vertex = best_vertex;
						second_vertex = best_vertex ^ neighbour;
					}
					else {
						first_vertex = best_vertex ^ neighbour;
						second_vertex = best_vertex;
					}
					add_edge(first_vertex, second_vertex);
					choose_next(edges_added + 1);
					//If done, propagate result
					if (actual_path.has_solution)
						return;
					else
						remove_edge(first_vertex, second_vertex);
				}
			}
		}
	}

	path pathfinding::actual_path;
	vertices_array pathfinding::vertex_component;
	std::array<sfi, MATCH_SIZE> pathfinding::component_sizes;
	vertices_array pathfinding::neighbours_bitmap;
	bool pathfinding::all_parallel;
	llfi pathfinding::bad_num = 0;
	llfi pathfinding::counterexamples = 0;

}