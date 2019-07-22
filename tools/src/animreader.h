#pragma once

#include <string>

struct MDLPKG;

struct AnimParserLine {
	std::string id;
	std::string value;

	int loc_colon;

	float float_vals[16];

	static AnimParserLine parse_line(std::string line) {
		AnimParserLine parsed_line;
		parsed_line.loc_colon = line.find(":");
		parsed_line.id = line.substr(0, parsed_line.loc_colon);
		parsed_line.value = line.substr(parsed_line.loc_colon + 1, line.length());

		return parsed_line;
	}

	void parse_floats() {
		int offset = 0;
		int loc = 0;
		int count = 0;

		while ((loc = value.find(",", offset)) != std::string::npos) {
			std::string tmp = value.substr(offset, loc - (offset != 0 ? offset : 0));

			float_vals[count++] = stof(tmp);

			offset = loc + 1;
		}

		float_vals[count++] = stof(value.substr(offset, value.length()));
	}
};

struct AnimReader {

	bool read_animations(const std::string& filename, MDLPKG* pkg);
};


