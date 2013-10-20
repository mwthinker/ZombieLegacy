#ifndef CARPROPERTIES_H
#define CARPROPERTIES_H

#include <string>

namespace zombie {

	class CarProperties {
	public:
		std::string name_;
		float mass_;
		float life_;		
		float width_;
		float length_;
		std::string image_;
	};

} // Namespace zombie.

#endif // CARPROPERTIES_H
