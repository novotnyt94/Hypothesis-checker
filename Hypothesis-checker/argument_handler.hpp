#ifndef ARGUMENT_HANDLER_
#define ARGUMENT_HANDLER_

#include "errors.hpp"
#include <string>

namespace cube {
	/* Parses arguments passed to the program. */
	class argument_handler {
	public:
		/* Parses the arguments. */
		static void parse_args(int argc, char ** argv);

		//Whether input file was selected
		static bool is_input;

		//Whether compressed input file was selected
		static bool is_comp_input;

		//Whether output file for matchings was selected
		static bool is_output;

		//Whether output file for compressed matchings was selected
		static bool is_comp_output;

		//Whether output file for paths was selected
		static bool is_path_output;

		//Input file name 
		static std::string input_file;

		//Compressed input file name
		static std::string comp_input_file;

		//Matching output file name
		static std::string output_file;

		//Compressed matching output file name
		static std::string comp_output_file;

		//Path output file name
		static std::string path_output_file;
	};
}

#endif //ARGUMENT_HANDLER
