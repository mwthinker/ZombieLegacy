#include "load.h"

#include <tinyxml2.h>

#include <vector>
#include <string>

namespace zombie {

	std::stringstream& operator>>(std::stringstream& stream, Point& point) {
		stream >> point.x;
		stream >> point.y;
		return stream;
	}

	// Takes a string as input and returns the points.
	// The string "POLYGON ((x1 y1, x2 y2, ...))" the input should be defined
	// as ((...). The last point is assumed to be the same as the first, therefore
	// the last point will not be returned.
	std::vector<Point> loadPolygon(std::string line) {
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
		std::stringstream stream(line);
		std::vector<Point> points;
		while (stream >> point) {
			points.push_back(point);
		}
		points.pop_back();
		return points;
	}

	std::vector<TupleImageScaleTime> loadAnimation(tinyxml2::XMLHandle xml) {
		tinyxml2::XMLHandle handle = xml.FirstChildElement("scale");
		if (handle.ToElement() == nullptr) {
			throw  std::exception();
		}
		float scale = convertFromText<float>(handle.ToElement()->GetText());
		std::vector<TupleImageScaleTime> animation;

		while (handle.NextSiblingElement("image").ToElement() != nullptr) {
			handle = handle.NextSiblingElement("image");
			std::string image = handle.ToElement()->GetText();
			handle = handle.NextSiblingElement("time");
			float time = convertFromText<float>(handle.ToElement()->GetText());
			animation.push_back(TupleImageScaleTime(image, scale, time));
		}

		return move(animation);
	}

	template <>
	const char* convertFromText<const char*>(const char* txt) {
		if (txt == nullptr) {
			throw mw::Exception("convertFromText, input null");
		}
		if (txt[0] == '\n') {
			throw mw::Exception("convertFromText, string is empty");
		}
		return txt;
	}

	// Returns the value of the tag's element. If the value is empty, a empty
	// string is returned. If the element don't exist a runtime exception is thrown.
	const char* toText(tinyxml2::XMLHandle handle) {
		tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw mw::Exception("convertFromText failed");
		}
		return element->GetText();
	}

	// Returns the tag's element. If the element don't exist a runtime exception is thrown.
	// The return value is never null.
	tinyxml2::XMLElement* toElement(tinyxml2::XMLHandle handle) {
		tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw mw::Exception("convertFromText failed");
		}
		return element;
	}

} // Namespace zombie.
