#include "matchings.hpp"

namespace cube {
	

	llfi compress(perfect_matching & source_matching) {
		llfi res = 0;

		/* The result structure:
		   Take the first unused edge, remove the last bit (the perfect matching is bipartite, thus this bit is reobtainable) and put bits 2-5 into end of result.
		   -> we get 64 bit number:

		    /-- 64 - 2 * # of vertices --\ /---------------------- 2 * # of vertices -------------------------\
		   (empty space for dimension < 5 | 4 bits of 1st edge | 4 bits of 2nd edge | ... | 4 bits of last edge). */

		FOR_VERTICES(vertex_id) {
			if (vertex_id < source_matching[vertex_id]) {
				res <<= 4;
				res += (source_matching[vertex_id] >> 1);
			}
		}
		return res;
	}

	perfect_matching decompress(llfi compressed_matching) {
		perfect_matching res;
		/* Just reversed process for compress(), starting from first edge. */
		FOR_VERTICES(vertex_id) {
			res[vertex_id] = INVALID;
		}
		sfi first_empty = 0;
		for(sfi edge_id = MATCH_SIZE - 1; edge_id != INVALID; edge_id--)	{
			while (res[first_empty] != INVALID)
				first_empty++;
			sfi other_vertex = (sfi)(((compressed_matching >> (4*edge_id)) & (DIMENSION_BITS >> 1)) << 1);
			//If thera is even number of non-zero bits, last bit was 1, otherwise 0. 
			if (!(hamming[other_vertex ^ first_empty] & 1))
				other_vertex ^= 1;

			res[first_empty] = other_vertex;
			res[other_vertex] = first_empty;

		}
		return res;
	}

	llfi compress_partial(perfect_matching & source_matching) {
		sfi edge_cnt = 0;
		llfi bitmap = 0;
		llfi result = 0;

		/* The resolvent structure:
		At first, we suppose that there is an edge from vertex 0.
		The result is composed of two parts:

		First part determines the "bitmap mask" telling which vertices are used/unused, second is equivalent to the compress() structure. We take vertices one-by-one starting at 0 and do the following:
		- If there is an edge going from this vertex to higher vertex, put '1' to the end of bitmap and 4 bits of edge to the end of result.
		- If the vertex is not yet used (=INVALID), put '0' to the end of bitmap.
		- If there is an edge going from this vertex to lower vertex, skip it. 
		
		The final result will be:

		 /-- # of vertices - # of edges - 1 ---\ /3 * (11 - # of edges)\ /------------ 4 * # of edges -----------------\
		(bitmap with ommited bit for 0th vertex | possibly empty space  | 4 bits of 1st edge | ... | 4 bits of last edge)   */

		FOR_VERTICES(vertex_id) {
			if (source_matching[vertex_id] == INVALID) {
				bitmap <<= 1;
				continue;
			}
			else if (source_matching[vertex_id] > vertex_id) {
				bitmap <<= 1;
				bitmap ^= 1;
				edge_cnt++;
				result <<= 4;
				result += (source_matching[vertex_id] >> 1);
			}
		}
#ifdef _DEBUG
		//check that number of edges is <= 11)
		if (edge_cnt > 11)
			errors::assert_error("Too large partial matching to be comprimed!");
		// There must be an edge from 0th vertex
		if (source_matching[0] == INVALID)
			errors::assert_error("First vertex should be always used while compressing!");
#endif //_DEBUG

		result ^= (bitmap << (33 + edge_cnt));
		return result;
	}
	
	void matchings::find_matchings() {
		clear_matching();
		
		//if there is at least 1 edge from Q_n, there exists an isomorphic matching with edge 0->1.
		if (HYPER_EDGE_CNT > 0) {
			hyper_edges = 1;
			add_edge(0, 1);
			find_remaining_edges(1, 2);
			remove_edge(0, 1);
		}

		//Otherwise, there exist an edge with length 3, alternatively of length 5 for d=5.
		else {
			hyper_edges = 0;
			add_edge(0, 7);
			find_remaining_edges(1, 1);
			remove_edge(0, 7);

			clear_matching();

			if (DIMENSION == 5) {
				FOR_EDGES(edge_id) {
					add_edge(edge_id, DIMENSION_BITS - edge_id);
				}
				results.insert(compress(matching));
			}
			clear_matching();
		}
		partial_matchings.clear();
	}
	
	void matchings::find_remaining_edges(const sfi count, const sfi first_empty) {
		// Try to add any possible edge  
		for (sfi end_vertex = first_empty + 1; end_vertex < VERTICES; end_vertex++) {
			// We need bipartitness
			if (!is_set(end_vertex) && (hamming[first_empty ^ end_vertex] & 1)) {
				// Actualize number of edges used from Q_n, check constrain
				bool is_hyper = (hamming[first_empty ^ end_vertex] == 1);
				if (is_hyper) {
					
					if (HYPER_EDGE_CNT <= hyper_edges)
						// Too many edges
						continue;
					// Otherwise add this edge
					hyper_edges++;
				}
				
				else {
					if (HYPER_EDGE_CNT >= hyper_edges + MATCH_SIZE - count)
						//There are not enough edges to fulfil the constraint
						continue;
				}

				add_edge(first_empty, end_vertex);
				// If this is perfect matching, add it into results
				if (count + 1 == MATCH_SIZE) {
					add_new_matching();
				}

				// Otherwise, for up to 10 edges used, we check whether the matching was yet visited (higher number is not effective).
				else if (count < 10) {
					perfect_matching min_matching = lex_min_matching();
					if (partial_matchings.insert(compress_partial(min_matching))) { 
					//new matching, continue the search
						sfi new_first_empty = first_empty + 1;
						while (is_set(new_first_empty))
							new_first_empty++;
						find_remaining_edges(count + 1, new_first_empty);
					}
				}
				// Otherwise, continue without testing.
				else {
					sfi new_first_empty = first_empty + 1;
					while (is_set(new_first_empty))
						new_first_empty++;
					find_remaining_edges(count + 1, new_first_empty);
				}

				// Return to initial state.
				remove_edge(first_empty, end_vertex);
				if (is_hyper)
					hyper_edges--;
			}
		}
	}

	sfi matchings::transform(sfi vertex_id) {
#ifdef _DEBUG
		//Test of transformation uniqueness
		if (!transform_data.is_unique())
			errors::assert_error("Transformation data are not complete!");

		//Test of its validity (the shuffle of coordinates should be a permutation).
		dimension_states used_dims;
		FOR_DIMENSION(dim_id) {
			used_dims[dim_id] = false;
		}
		FOR_DIMENSION(dim_id) {
			if (used_dims[top_one[transform_data.dim_permutation[dim_id]]])
				errors::assert_error("Transformation data are corrupted!");
			used_dims[top_one[transform_data.dim_permutation[dim_id]]] = true;
		}	
#endif // _DEBUG

		//swapping coordinates
		vertex_id ^= transform_data.dim_swap;

		//permutating coordinates values
		sfi transformed_vertex = 0;
		FOR_DIMENSION(dim_id) {
			if (vertex_id & (1 << dim_id)) 
				transformed_vertex ^= (1 << top_one[transform_data.dim_permutation[dim_id]]);
		}
		return transformed_vertex;
	}

	sfi matchings::inverse_transform(sfi vertex_id) {
#ifdef _DEBUG
		//Test of transformation uniqueness
		if (!transform_data.is_unique())
			errors::assert_error("Transformation data are not complete!");

		//Test of its validity (the shuffle of coordinates should be a permutation).
		dimension_states used_dims;
		FOR_DIMENSION(dim_id) {
			used_dims[dim_id] = false;
		}
		FOR_DIMENSION(dim_id) {
			if (used_dims[top_one[transform_data.dim_permutation[dim_id]]])
				errors::assert_error("Transformation data are corrupted!");
			used_dims[top_one[transform_data.dim_permutation[dim_id]]] = true;
		}
#endif // _DEBUG

		//permutating coordinates (we need to do inverse the actions in respect to transform())
		sfi transformed_vertex = 0;
		FOR_DIMENSION(dim_id) {
			if (vertex_id & transform_data.dim_permutation[dim_id])
				transformed_vertex ^= (1 << dim_id);
		}

		//swapping coordinates values
		transformed_vertex ^= transform_data.dim_swap;

		return transformed_vertex;
	}

	void matchings::add_new_matching() {
		perfect_matching new_match = lex_min_matching();
		//try to insert new matching
		results.insert(compress(new_match));

#ifdef PROGRESS_INFO
		cycles++;
		if (cycles % GENERATED_MATCHINGS_INFO == 0) {
			std::cout << "Matchings: " << cycles << ", distinct: " << results.size() << std::endl;
			std::cout << "Actual: ";
			FOR_VERTICES(vertex_id) {
				if (matching[vertex_id] > vertex_id)
					std::cout << (size_t)vertex_id << "->" << (size_t)matching[vertex_id] << " ";
			}
			
#ifdef __linux__
			//Memory usage information (only for linux)    
			rusage r;
			getrusage(RUSAGE_SELF, &r);
			std::cout << std::endl << "Memory: " << r.ru_maxrss;
#endif
			std::cout << std::endl;
		}
#endif // PROGRESS_INFO		
	}

	void matchings::add_edge(const sfi first_vertex, const sfi second_vertex) {
#ifdef _DEBUG
		if (is_set(first_vertex) || is_set(second_vertex))
			errors::assert_error("Used vertex marked as unused!");
#endif //_DEBUG
		matching[first_vertex] = second_vertex;
		matching[second_vertex] = first_vertex;
	}

	void matchings::remove_edge(const sfi first_vertex, const sfi second_vertex) {
#ifdef _DEBUG
		if (!is_set(first_vertex) || !is_set(second_vertex))
			errors::assert_error("Unused vertex marked as used!");
		if (matching[first_vertex] != second_vertex || matching[second_vertex] != first_vertex)
			errors::assert_error("Selected pair is not an edge!");
#endif //_DEBUG
		matching[first_vertex] = INVALID;
		matching[second_vertex] = INVALID;
	}

	perfect_matching matchings::lex_min_matching() {

		//set actual to (temporarily) best
		best_isomorphic = matching;
		if (matching[0] == INVALID) {
			errors::unimplemented_feature("Search for matching with unused vertex 0 is not supported!");
		}
		
		//Look for better
		find_best_transformation();
		return best_isomorphic;
	}
	void matchings::find_best_transformation() {
		//Try to map all edges 
		FOR_VERTICES(vertex_id) {
			sfi second_vertex = matching[vertex_id];
			//choose every valid edges for the minimal one (both orientations)
			if (second_vertex != INVALID) {
				sfi dimension_diff = hamming[vertex_id ^ second_vertex];
				//best_isomorphic is better while it has better lowest edge
				if (dimension_diff > hamming[best_isomorphic[0]])
					continue;

				sfi end_min = 1;
				while (dimension_diff > 1) {
					end_min <<= 1;
					end_min += 1;
					dimension_diff--;
				}
				//get as much information as possible
				get_transformation(edge(vertex_id, second_vertex), edge(0, end_min));
				FOR_DIMENSION(dim_id) {
					used_dimensions[dim_id] = false;
				}
				//try all valid permutations
				generate_possible_transformations(0);
			}
		}
	}

	


	void matchings::get_transformation(const edge & base_edge, const edge & transformed_edge) {
#ifdef _DEBUG
		if (transformed_edge.first != 0)
			errors::unimplemented_feature("Transformation is working only for final edge rising from zero!");
#endif // _DEBUG

		//Get swapping data (it is simple, because we are mapping some vertex to 0th) 
		transform_data.dim_swap = base_edge.first;

		
		//Get constrains for possible permutations
		sfi dist = hamming[base_edge.first ^ base_edge.second];
		sfi dist_bits = (1 << dist) - 1;
		FOR_DIMENSION(dim_id) {
			if ((base_edge.first ^ base_edge.second) & (1 << dim_id)) 
				//They differ in this coordinate, thus we need to map it into first dist bits (looking for lexicographicaly minimal matching).
				transform_data.dim_permutation[dim_id] = dist_bits;
			else 
				//They dont differ in this coordinate, thus we need to map it into remaining bits
				transform_data.dim_permutation[dim_id] = DIMENSION_BITS ^ dist_bits;
		}
	}

	void matchings::generate_possible_transformations(const sfi count) {
		if (count == DIMENSION) {
			//Permutation selected -> try it
			try_transformation();
			return;
		}
		else {
			//Save the original data and try all possibilities to fill permutation
			sfi possibilities = transform_data.dim_permutation[count];
			FOR_DIMENSION(dim_id) {
				
				if (!used_dimensions[dim_id] && possibilities & (1 << dim_id)) {
					//The choice is possible
					used_dimensions[dim_id] = true;
					transform_data.dim_permutation[count] = (1 << dim_id);
					generate_possible_transformations(count + 1);
					used_dimensions[dim_id] = false;
				}
			}
			//Restore original value
			transform_data.dim_permutation[count] = possibilities;
		}
	}

	void matchings::try_transformation() {
		FOR_VERTICES(vertex_id) {
			actual_isomorphic[vertex_id] = INVALID;
		}
		bool found_better = false;

		//Apply the transformation on vertices one-by-one until difference is found between actual_isomorphic and best_isomorphic. Then either break or finish the better isomorphism.
		FOR_VERTICES(vertex_id) {
			if (actual_isomorphic[vertex_id] == INVALID) {
				sfi start_vertex = inverse_transform(vertex_id);
				if (!is_set(start_vertex)) { 
					if (!found_better && best_isomorphic[vertex_id] != INVALID) 
						//in best_isomorphic this vertex is set, thus actual would be worse
						return;
				}
				else {
					sfi transformed_neighbour = transform(matching[start_vertex]);
					if (!found_better) {
						//Still not known if the result is better
						if (transformed_neighbour < best_isomorphic[vertex_id]) {
							//Got better
							found_better = true;
						}
						else if (transformed_neighbour > best_isomorphic[vertex_id]) {
							//Got worse
							return;
						}
						//If it is tie, we must continue undecided
					}
					actual_isomorphic[vertex_id] = transformed_neighbour;
					actual_isomorphic[transformed_neighbour] = vertex_id;
				}
			}
		}

		if (found_better) {
			best_isomorphic = actual_isomorphic;
		}
	}

	perfect_matching matchings::matching;
	result_set matchings::results;
	result_set matchings::partial_matchings;
	sfi matchings::hyper_edges = 0;
	dimension_states matchings::used_dimensions;
	vertices_array matchings::actual_isomorphic;
	vertices_array matchings::best_isomorphic;
	transformation matchings::transform_data;

#ifdef PROGRESS_INFO	
	llfi matchings::cycles = 1;
#endif // PROGRESS_INFO	
}
