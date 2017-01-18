#include "argument_handler.hpp"

namespace cube {
	void argument_handler::parse_args(int argc, char ** argv) {
		int arg_id = 1;
		while (arg_id < argc) {
			//Expected a switch (of length 2)
			if (argv[arg_id][0] != '-' || argv[arg_id][1] == 0 || argv[arg_id][2] != 0)
				errors::argument_error("Argument switch expected!");

			switch (argv[arg_id++][1]) {
			case 'i': //Input file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Input file name expected after -i!");
				}
				is_input = true;
				input_file = argv[arg_id];
				arg_id++;
				break;

			case 'm': //Matching output file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Output file name expected after -m!");
				}
				is_output1 = true;
				output1_file = argv[arg_id];
				arg_id++;
				break;

			case 'o': //Path output file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Output file name expected after -o!");
				}
				is_output2 = true;
				output2_file = argv[arg_id];
				arg_id++;
				break;

			default:
				errors::argument_error("Unknown argument skipped:" + (std::string)argv[arg_id], 0);
			}
		}
	}

	bool argument_handler::is_input = false;
	bool argument_handler::is_output1 = false;
	bool argument_handler::is_output2 = false;
	std::string argument_handler::input_file;
	std::string argument_handler::output1_file;
	std::string argument_handler::output2_file;
}