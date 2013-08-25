#ifndef DRAWWEAPONOBJECT_H
#define DRAWWEAPONOBJECT_H

#include "graphictask.h"

namespace zombie {

	class WeaponObject;

	class DrawWeaponObject : public GraphicTask {
	public:
		DrawWeaponObject(WeaponObject* wOb);

		void draw(double time) override;

		bool isRunning() const override;

	private:
		InMemory inMemory_;
		WeaponObject* wOb_;
	};

} // Namespace zombie.

#endif // DRAWWEAPONOBJECT_H
