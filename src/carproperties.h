#ifndef CARPROPERTIES_H
#define CARPROPERTIES_H

#include <string>

namespace zombie {

	class CarProperties {
	public:
		std::string name_;
		float mass_;
		std::string animation_;
		float life_;		
		float width_;
		float length_;
	};

} // Namespace zombie.

#endif // CARPROPERTIES_H
