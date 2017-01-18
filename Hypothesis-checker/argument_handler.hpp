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

		//Whether output file for matchings was selected
		static bool is_output1;

		//Whether output file for paths was selected
		static bool is_output2;

		//Input file name 
		static std::string input_file;

		//Matching output file name
		static std::string output1_file;

		//Path output file name
		static std::string output2_file;
	};
}

#endif //ARGUMENT_HANDLER
