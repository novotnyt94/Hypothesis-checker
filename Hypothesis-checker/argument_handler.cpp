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
				if (is_input || is_comp_input) {
					errors::argument_error("Only one input file may be specified!");
				}
				is_input = true;
				input_file = argv[arg_id];
				arg_id++;
				break;

			case 'c': //Compressed input file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Input file name expected after -c!");
				}
				if (is_input || is_comp_input) {
					errors::argument_error("Only one input file may be specified!");
				}
				is_comp_input = true;
				comp_input_file = argv[arg_id];
				arg_id++;
				break;

			case 'p': //Matching output file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Output file name expected after -p!");
				}
				is_output = true;
				output_file = argv[arg_id];
				arg_id++;
				break;

			case 'm': //Compressed matching output file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Output file name expected after -m!");
				}
				is_comp_output = true;
				comp_output_file = argv[arg_id];
				arg_id++;
				break;

			case 'o': //Path output file
				if (arg_id == argc || argv[arg_id][0] == '-') {
					errors::argument_error("Output file name expected after -o!");
				}
				is_path_output = true;
				path_output_file = argv[arg_id];
				arg_id++;
				break;

			default:
				errors::argument_error("Unknown argument skipped:" + (std::string)argv[arg_id], 0);
			}
		}
	}

	bool argument_handler::is_input = false;
	bool argument_handler::is_comp_input = false;
	bool argument_handler::is_output = false;
	bool argument_handler::is_comp_output = false;
	bool argument_handler::is_path_output = false;
	std::string argument_handler::input_file;
	std::string argument_handler::comp_input_file;
	std::string argument_handler::output_file;
	std::string argument_handler::comp_output_file;
	std::string argument_handler::path_output_file;
}