#ifndef ERRORS_
#define ERRORS_

#include <iostream>
#include <string>

namespace cube {
	/* Class for reporting various errors. */
	class errors {
	public:

		inline static void assert_error(std::string msg, int ret_val = 1) {
			std::cerr << "Runtime error: " << msg << std::endl;
			if (ret_val != 0) //If ret_val=0, the error is not critical
				std::exit(ret_val);
		}

		inline static void unimplemented_feature(std::string msg, int ret_val = 1) {
			std::cerr << "Not implemented: " << msg << std::endl;
			if (ret_val != 0) //If ret_val=0, the error is not critical
				std::exit(ret_val);
		}

		inline static void argument_error(std::string msg, int ret_val = 1) {
			std::cerr << "Argument error: " << msg << std::endl;
			if (ret_val != 0) //If ret_val=0, the error is not critical
				std::exit(ret_val);
		}

		inline static void input_error(std::string msg, int ret_val = 1) {
			std::cerr << "Input error: " << msg << std::endl;
			if (ret_val != 0) //If ret_val=0, the error is not critical
				std::exit(ret_val);
		}

		inline static void output_error(std::string msg, int ret_val = 1) {
			std::cerr << "Output error: " << msg << std::endl;
			if (ret_val != 0) //If ret_val=0, the error is not critical
				std::exit(ret_val);
		}
	};
}

#endif //ERRORS_
