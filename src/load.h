#ifndef LOAD_H
#define LOAD_H

#include "terrain2d.h"
#include "animation.h"

#include <tinyxml2.h>
#include <mw/exception.h>
#include <mw/color.h>

#include <cmath>
#include <sstream>

namespace zombie {

	std::stringstream& operator>>(std::stringstream& stream, Point& point);
	std::stringstream& operator>>(std::stringstream& stream, mw::Color& color);

	// Takes c-string as input and returns the correct conversion.
	// Throws mw::Exception if the input is null or if the conversion
	// fails.
	// E.g. float a = convertFromText<float>("1.2");
	// Gives output, a = 1.2;
	template <class Output>
	Output convertFromText(const char* txt) {
		if (txt == nullptr) {
			throw mw::Exception("convertFromText, input null");
		}

		std::stringstream stream(txt);
		Output output;
		stream >> output;
		return output;
	}

	// Returns the input txt. If the c-string is empty a runtime exception is thrown.
	template <>
	const char* convertFromText<const char*>(const char* txt);

	// Takes a string as input and returns the points.
	// The string "POLYGON ((x1 y1, x2 y2, ...))" the input should be defined
	// as ((...). The last point is assumed to be the same as the first, therefore
	// the last point will not be returned.
	std::vector<Point> loadPolygon(std::string line);

	const char* toText(tinyxml2::XMLHandle handle);

	// Returns the tag's element. If the element don't exist a runtime exception is thrown.
	// The return value is never null.
	tinyxml2::XMLElement* toElement(tinyxml2::XMLHandle handle);

} // Namespace zombie.

#endif // LOAD_H
