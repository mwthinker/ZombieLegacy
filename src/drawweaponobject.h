#ifndef DRAWWEAPONOBJECT_H
#define DRAWWEAPONOBJECT_H

#include "task.h"

namespace zombie {

	class WeaponObject;

	class DrawWeaponObject : public Task {
	public:
		DrawWeaponObject(WeaponObject* weapon);

		bool update(double time) override;

	private:
		int idWeaponObject_;
	};

} // Namespace zombie.

#endif // DRAWWEAPONOBJECT_H
