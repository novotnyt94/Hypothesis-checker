#ifndef SERIALIZER_
#define SERIALIZER_

#include "settings.hpp"
#include "errors.hpp"
#include "path.hpp"
#include "matchings.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace cube {
	/* Serializes/deserializes matchings and paths to/from files */
	class serializer {
	public:
		//Loads matchings from selected file
		static result_set load_matchings(const std::string file_name);

		//Loads matchings from selected file
		static result_set load_comp_matchings(const std::string file_name);

		//Saves matchings to selected file
		static void save_matchings(const std::string file_name, const result_set & matchings);

		//Saves matchings to selected file
		static void save_comp_matchings(const std::string file_name, const result_set & matchings);

		//Saves paths to selected file
		static void save_paths(const std::string file_name, const std::vector<path> & paths);
		
	private:		
		//Deserializes perfect matchings from format v(1)->v(2) v(3)->v(4) ... v(n-1)->v(n)
		static perfect_matching parse_matching(std::string & line);

		//Transforms text-based edge (v(x)->v(y)) into structure edge.
		static edge parse_edge(std::string && token);

		//Transforms perfect matching into u_1->v_1 u_2->v_2 ... u_k->v_k
		static std::string get_serialized_matching(const perfect_matching & matching);

		//Serializes given path into p(1)->p(2)->...->p(n)
		static std::string get_serialized_path(const path & path_);
		
		//Loads next non-empty line into buffer
		static void next_line(std::string & line);


		//input file stream
		static std::ifstream input_file;

		//Matching output file stream
		static std::ofstream output_file;

		//Compressed matching output file stream
		static std::ofstream output_comp_file;

		//Path output file stream
		static std::ofstream output_path_file;
	};
}

#endif //SERIALIZER_
