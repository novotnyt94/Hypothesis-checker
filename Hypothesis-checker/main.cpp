/**
induction-assumption-checker, main.cpp
File with entry point of the program.

@author Tomas Novotny
@version 1.0 18/08/17
*/

#include "settings.hpp"
#include "argument_handler.hpp"
#include "serializer.hpp"
#include "matchings.hpp"
#include "pathfinding.hpp"
#include "path.hpp"
#include <iostream>


using namespace cube;


/* Entry point of the program, handles calls of correct subrutines. */
int main(int argc, char ** argv) {
	//By switching off sync the speed of I/O operations will greatly increase.
	std::ios::sync_with_stdio(false); 
	
	argument_handler::parse_args(argc, argv);
	result_set & found_matchings = matchings::results;

	//Obtaining perfect matchings, either from file or from generator
	if (argument_handler::is_input || argument_handler::is_comp_input) {
		std::cout << "Loading matchings... ";
		if (argument_handler::is_input)
			found_matchings = serializer::load_matchings(argument_handler::input_file);
		else 
			found_matchings = serializer::load_comp_matchings(argument_handler::comp_input_file);
		std::cout << "Done" << std::endl;
	}
	else {
		std::cout << "Generating matchings..." << std::endl;
		matchings::find_matchings();
		std::cout << "Done" << std::endl;
	}
	std::cout << "Matchings found: " << found_matchings.size() << std::endl;

	//Save them, if it is required
	if (argument_handler::is_output) {
		std::cout << "Saving matchings... ";
		serializer::save_matchings(argument_handler::output_file, found_matchings);
		std::cout << "Done" << std::endl;
	}

	//Save them compressed, if it is required
	if (argument_handler::is_comp_output) {
		std::cout << "Saving compressed matchings... ";
		serializer::save_comp_matchings(argument_handler::comp_output_file, found_matchings);
		std::cout << "Done" << std::endl;
	}

	//Solve obtained paths
	std::cout << "Finding paths..." << std::endl;	
	std::vector<path> found_paths = pathfinding::find_unsolved_paths(found_matchings);
	std::cout << "Done" << std::endl;

	//Again, save them, if it is required
	if (argument_handler::is_path_output) {
		std::cout << "Saving paths... ";
		serializer::save_paths(argument_handler::path_output_file, found_paths);
		std::cout << "Done" << std::endl;
	}

	//Show results
	std::cout << "Matchings with impossible configurations:" << std::endl;
	size_t total = 0;
	size_t solved = found_paths.size() / BIPARTITE_PAIRS_CNT;
	for (size_t i = 0; i < solved; i++) {
		size_t failed = 0;
		for (size_t j = 0; j < BIPARTITE_PAIRS_CNT; j++) {
			if (!found_paths[i * BIPARTITE_PAIRS_CNT + j].has_solution)
				failed++;
		}
		//Output only matchings with at least one failed path (for clarity)
		if (failed > 0) {
			total++;
			std::cout << "Matching ";
			std::array<sfi, VERTICES> dimcnts;
			FOR_VERTICES(vertex_id) {
				dimcnts[vertex_id] = 0;
			}
			FOR_VERTICES(vertex_id) {
				if (found_paths[i * BIPARTITE_PAIRS_CNT].base_matching[vertex_id] > vertex_id) {
					dimcnts[vertex_id ^ found_paths[i * BIPARTITE_PAIRS_CNT].base_matching[vertex_id]]++;
					std::cout << (size_t)vertex_id << "->" << (size_t)found_paths[i * BIPARTITE_PAIRS_CNT].base_matching[vertex_id] << ", ";
				}
			}
			std::cout << "impossible: " << failed << std::endl;
		}
	}
	std::cout << "Total number of matchings with unsolved configurations: " << total << std::endl;
	return 0;
}
