#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "building.h"
#include "buildingshader.h"
#include "array.h"

#include <mw/sprite.h>
#include <mw/vertexbufferobject.h>

#include <vector>

namespace zombie {	

	class Building2D : public Building {
	public:
		friend class DrawBuildings;

		Building2D();

		Building2D(Position p1, Position p2, Position p3, Position p4,
			const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof);

	private:
		float height_;
		mw::Sprite leftWall_, rightWall_, roof_;
	};

} // Namespace zombie.

#endif // BUILDING2D_H
