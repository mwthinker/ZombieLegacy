#ifndef LOAD_H
#define LOAD_H

#include <mw/exception.h>

#include <cmath>
#include <sstream>

namespace zombie {

	// Takes c-string as input and returns the correct conversion.
	// Throws mw::Exception if the input is null or if the conversion
	// fails.
	// E.g. float a = convertFromText<float>("1.2");
	// Gives output, a = 1.2;
	template <class Output>
	Output convertFromText(const char* txt) {
		if (txt == nullptr) {
			throw mw::Exception("convertFromText failed");
		}
		std::stringstream stream(txt);
		Output output;
		// Extract formatted input succeeds?
		if (stream >> output) {
			return output;
		} else {
			throw mw::Exception("convertFromText failed");
		}
	}

	std::stringstream& operator>>(std::stringstream& stream, Point& point) {
		stream >> point.x;
		stream >> point.y;
		return stream;
	}

	// Takes c-string as input and returns the corresponding buildingProperties.
	// Throws mw::Exception if the input is null or if the conversion
	// fails.
	// E.g. BuildingProperties p = convertFromText<BuildingProperties>("POLYGON ((1.2 2,41.2 0.123))");
	// Gives BuildingProperties: p.points_={Point(1.2,2),Point(41.2,0.123)}; which is vector with the two points.
	template <>
	BuildingProperties convertFromText<BuildingProperties>(const char* txt) {
		std::stringstream stream(txt);
		std::string word;
		
		if (stream >> word) {
			if (word == "POLYGON") {
				BuildingProperties properties;
				std::string line = stream.str();
				int index = line.find("((");
				int index2 = line.rfind("))");
				
				// New line is the string between "((" and "))".
				line = line.substr(index + 2, index2 - index - 2);
				
				// Replace all ',' (comma) with whitespace.
				for (char& chr : line) {
					if (chr == ',') {
						chr = ' ';
					}
				}
				Point point;
				stream.str(line);
				while (stream >> point) {
					properties.points_.push_back(point);
				}
				return properties;
			} else {
				throw mw::Exception("convertFromText failed, unknown geometry: " + word);
			}
		} else {
			throw mw::Exception("convertFromText failed");
		}
		throw mw::Exception("convertFromText failed: BuildingProperties");
	}

} // Namespace zombie.

#endif // LOAD_H
