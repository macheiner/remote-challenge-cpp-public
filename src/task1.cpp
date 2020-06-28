#include <iostream>
#include <string>
#include <vector>

#include "input_output.h"
#include "challenge_task1.h"


int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "USAGE: " << argv[0] << " [INPUT_FILE] [OUTPUT_FILE]" << std::endl;
		return 0;
	}

	auto input_file = std::string{argv[1]};
	auto output_file = std::string{argv[2]};

	chal::Points input_polygon = chal::read_polygon_from_json(input_file);

	chal::Points result_polygon;
	result_polygon = chal::challenge_task_1(input_polygon);

	chal::write_point_vector_to_json(result_polygon, output_file);
}