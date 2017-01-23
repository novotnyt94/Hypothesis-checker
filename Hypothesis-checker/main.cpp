#include "settings.hpp"
#include "argument_handler.hpp"
#include "serializer.hpp"
#include "matchings.hpp"
#include "pathfinding.hpp"
#include "path.hpp"
#include <iostream>


using namespace cube;

int main(int argc, char ** argv) {
	//By switching off sync the speed of I/O operations will greatly increase.
	std::ios::sync_with_stdio(false); 
	
	argument_handler::parse_args(argc, argv);
	result_set & found_matchings = matchings::results;

	//Obtaining perfect matchings, either from file or from generator
	if (argument_handler::is_input) {
		std::cout << "Loading matchings... ";
		found_matchings = serializer::load_matchings(argument_handler::input_file);
		std::cout << "Done" << std::endl;
	}
	else {
		std::cout << "Generating matchings..." << std::endl;
		matchings::find_matchings();
		std::cout << "Done" << std::endl;
	}
	std::cout << "Matchings found: " << found_matchings.size() << std::endl;

	//Save them, if it is required
	if (argument_handler::is_output1) {
		std::cout << "Saving matchings... ";
		serializer::save_matchings(argument_handler::output1_file, found_matchings);
		std::cout << "Done" << std::endl;
	}

	//Solve obtained paths
	std::cout << "Finding paths..." << std::endl;	
	std::vector<path> found_paths = pathfinding::find_unsolved_paths(found_matchings);
	std::cout << "Done" << std::endl;

	//Again, save them, if it is required
	if (argument_handler::is_output2) {
		std::cout << "Saving paths... ";
		serializer::save_paths(argument_handler::output2_file, found_paths);
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
	std::cout << "Total number of unsolved matchings: " << total << std::endl;
	return 0;
}

/*CHANGES MADE AFTER TESTING:
- Fully translated to english. Polished the code.
- Modified the type 1 number of failures to work for dimensions != 5.
- Completed the path checking in DEBUG mode.
- Added automatic check whether the hypothesis is satisfied.
- Fixed CHECK_INPUT for the new version.
- Cosmetic changes in the code of class 'matchings'.
- Removed the 'number of failures' testing for dimension 3, which results are not regular.
- Changed the half-layer search into more straightforward version
*/
