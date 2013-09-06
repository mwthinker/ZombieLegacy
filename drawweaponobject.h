#ifndef DRAWWEAPONOBJECT_H
#define DRAWWEAPONOBJECT_H

#include "graphictask.h"

namespace zombie {

	class WeaponObject;

	class DrawWeaponObject : public GraphicTask {
	public:
		DrawWeaponObject(WeaponObject* weapon);

		void draw(double time) override;

		bool isRunning() const override;

	private:
		int idWeaponObject_;
	};

} // Namespace zombie.

#endif // DRAWWEAPONOBJECT_H
