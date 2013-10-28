#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

namespace zombie {

	class Settings {
	public:
		int width_;
		int height_;
		int unitLevel_;
		int zoomLevel_;
		std::string mapFile_;
	};

} // Namespace zombie.

#endif // SETTINGS_H
