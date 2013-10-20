#ifndef WEAPONPROPERTIES_H
#define WEAPONPROPERTIES_H

#include <string>

namespace zombie {

	class WeaponProperties {
	public:
		std::string name_;
		float mass_;
		std::string image_;
		float damage_;
		float timeBetweenShots_;
		float range_;
		int clipSize_;
		std::string shootSound_;
		std::string reloadSound_;
	};

} // Namespace zombie.

#endif // WEAPONPROPERTIES_H
