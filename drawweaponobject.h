#ifndef DRAWWEAPONOBJECT_H
#define DRAWWEAPONOBJECT_H

#include "graphictask.h"

namespace zombie {

	class WeaponObject;

	class DrawWeaponObject : public GraphicTask {
	public:
		DrawWeaponObject(WeaponObject* weapon);

		bool draw(double time) override;

	private:
		int idWeaponObject_;
	};

} // Namespace zombie.

#endif // DRAWWEAPONOBJECT_H