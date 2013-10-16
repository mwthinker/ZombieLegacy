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
	// Gives output a = 1.2;
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

	template <>
	BuildingProperties convertFromText<BuildingProperties>(const char* txt) {
		BuildingProperties properties_;
		return properties_;
	}

} // Namespace zombie.

#endif // LOAD_H
