#include "serializer.hpp"

namespace cube {
	result_set serializer::load_matchings(const std::string file_name) {
		//tries to open selected file
		input_file.open(file_name); 
		if (input_file.fail())
			errors::input_error("An error occured when opening " + file_name);

		result_set found_matchings;
		while (true) {
			std::string line;
			next_line(line);
			
			if (input_file.fail()) {
				//file read failed - if it was not due to EOF, there was some error
				if (input_file.eof())
					break;
				else
					errors::input_error("Unexpected error while reading input file!");
			}
		
			//Deserialize the matching
			perfect_matching match = parse_matching(line);
			found_matchings.insert(compress(match));
		}
		input_file.close();
		return std::move(found_matchings);
	}

	result_set serializer::load_comp_matchings(const std::string file_name) { //TODO
		//tries to open selected file
		input_file.open(file_name, std::ios::in | std::ios::binary);
		if (input_file.fail())
			errors::input_error("An error occured when opening " + file_name);

		result_set found_matchings;
		while (true) {
			llfi actual_matching;
			input_file.read((char*)& actual_matching, sizeof(llfi));

			if (input_file.fail()) {
				//file read failed - if it was not due to EOF, there was some error
				if (input_file.eof())
					break;
				else
					errors::input_error("Unexpected error while reading input file!");
			}
#ifdef CHECK_INPUT
			perfect_matching decompressed_matching = decompress(actual_matching);
			FOR_VERTICES(vertex_id) {
				if (decompressed_matching[vertex_id] == INVALID)
					errors::input_error("Some matching is corrupted!");
			}
#endif //CHECK_INPUT

			found_matchings.insert(actual_matching);
		}
		input_file.close();
		return std::move(found_matchings);
	}

	void serializer::save_matchings(const std::string file_name, const result_set & matchings) {
		//tries to open selected file
		output_file.open(file_name, std::ios::out);
		if (output_file.fail())
			errors::output_error("An error occured when opening " + file_name);
		//save the matching in non-comprimed state (may create HUGE files for DIMENSION=5)
		for (llfi block_id = 0; block_id < PRIME_MOD; block_id++) {
			for (auto it = matchings[block_id].begin(); it != matchings[block_id].end(); it++) {
				output_file << get_serialized_matching(decompress(*it)) << std::endl;
			}
		}
		if (output_file.fail())
			errors::output_error("An error occured while saving matchings!");
		output_file.close();
	}

	void serializer::save_comp_matchings(const std::string file_name, const result_set & matchings) {
		//tries to open selected file
		output_comp_file.open(file_name, std::ios::out | std::ios::binary);
		if (output_comp_file.fail())
			errors::output_error("An error occured when opening " + file_name);
		//save the matching in comprimed state (still creates almost 10GB file for d=5)
		for (llfi block_id = 0; block_id < PRIME_MOD; block_id++) {
			for (auto it = matchings[block_id].begin(); it != matchings[block_id].end(); it++) {
				output_comp_file.write((char*)&(*it), sizeof(*it));
			}
		}
		if (output_comp_file.fail())
			errors::output_error("An error occured while saving compressed matchings!");
		output_comp_file.close();
	}


	void serializer::save_paths(const std::string file_name, const std::vector<path> & paths) {
		//tries to open selected file
		output_path_file.open(file_name);
		if (output_path_file.fail())
			errors::output_error("An error occured when opening " + file_name);
		for (size_t path_id = 0; path_id != paths.size(); path_id++) {
			if (path_id % ((VERTICES >> 1) * ((VERTICES >> 1) - 1)) == 0)
				output_path_file << "Matching " << get_serialized_matching(paths[path_id].base_matching) << std::endl;
			output_path_file << "\t" << get_serialized_path(paths[path_id]) << std::endl;
		}
		if (output_path_file.fail())
			errors::output_error("An error occured while saving matchings!");
		output_path_file.close();
	}

	perfect_matching serializer::parse_matching(std::string & line) {
		perfect_matching matching;

#ifdef CHECK_INPUT 
		sfi edge_count = 0;
		FOR_VERTICES(vertex_id) {
			matching[vertex_id] = INVALID;
		}
#endif //CHECK_INPUT

		std::stringstream str(std::move(line));
		std::string token;
		while (true) {
			str >> token;
			if (token == "" || str.fail()) //end of line
				break;

			edge next_edge = parse_edge(std::move(token));
			matching[next_edge.first] = next_edge.second;
			matching[next_edge.second] = next_edge.first;

#ifdef CHECK_INPUT 
			if (edge_count == MATCH_SIZE)
				errors::input_error("Some line has too many edges!");
			edge_count++;
#endif //CHECK_INPUT
		}		
#ifdef CHECK_INPUT 
		//Check, if the result is really a matching
		if (edge_count != MATCH_SIZE)
			errors::input_error("Some line does not have enough edges!");
		FOR_VERTICES(vertex_id) {
			if (matching[vertex_id] == INVALID)
				errors::input_error("Some vertex was not set up!");
		}
#endif //CHECK_INPUT_
		return std::move(matching);
	}

	void serializer::next_line(std::string & line) {
		do {
			std::getline(input_file, line);
		} while (line.empty() && !input_file.fail());	//skipping empty lines
	}

	edge serializer::parse_edge(std::string && token) {
		sfi vert1, vert2;
		try {
			vert1 = std::stoi(token);
			size_t vert2_start = std::find(token.cbegin(), token.cend(), '>') - token.cbegin() + 1;
			vert2 = std::stoi(token.substr(vert2_start, token.length() - vert2_start));
		}
		catch (...) {
			errors::input_error("An error occured while parsing edges!");
		}
		//syntax check
#ifdef CHECK_INPUT 
		if (vert1 >= VERTICES || vert2 >= VERTICES)
			errors::input_error("Nonexisting vertex selected: " + token);
		sfi position = 0;
		sfi mode = 0;
		bool was_number = false;
		while (position < token.length()) {
			switch (mode) {
			case 0:
				if (token[position] < '0' || token[position] > '9') {
					if (!was_number || token[position] != '-')
						errors::input_error("Wrong edge definition: " + token);
					else {
						mode++;
						was_number = false;
					}
				}
				else
					was_number = true;
				break;
			case 1:
				if (token[position] != '>')
					errors::input_error("Wrong edge definition: " + token);
				else
					mode++;
				break;
			case 2:
				if (token[position] < '0' || token[position] > '9')
					errors::input_error("Wrong edge definition: " + token);
				was_number = true;
				break;
			default:
				errors::assert_error("Reached default case in a switch!");
				break;
			}
			position++;
		}
		if (mode != 2 || !was_number)
			errors::input_error("Wrong edge definition: " + token);
#endif //CHECK_INPUT_			
		return std::make_pair(vert1, vert2);
	}

	std::string serializer::get_serialized_matching(const perfect_matching & matching) {
		std::stringstream ret;
		FOR_VERTICES(vertex_id) {
			if (matching[vertex_id] > vertex_id) {
				ret << (size_t)vertex_id << "->" << (size_t)matching[vertex_id] << " ";
			}
		}
		return ret.str();
	}
	

	std::string serializer::get_serialized_path(const path & this_path) {
		std::stringstream ret;
		ret << (size_t)this_path.start_vertex << "->" << (size_t)this_path.end_vertex << ": ";
		if (!this_path.has_solution)
			ret << "Path does not exist.";
		else {
			ret << (size_t)this_path.start_vertex;
			sfi act_vertex = this_path.start_vertex;
			for (sfi i = 0; i < (VERTICES >> 1) - 1; i++)
			{
				act_vertex = this_path.base_matching[act_vertex];
				ret << "->" << (size_t)act_vertex;
				act_vertex = this_path.found_path[act_vertex];
				ret << "->" << (size_t)act_vertex;
			}
			ret << "->" << (size_t)this_path.end_vertex;
		}
		return ret.str();
	}

	std::ifstream serializer::input_file;
	std::ofstream serializer::output_file;
	std::ofstream serializer::output_comp_file;
	std::ofstream serializer::output_path_file;
}