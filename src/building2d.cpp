#include "building2d.h"
#include "auxiliary.h"

#include <mw/opengl.h>

namespace zombie {

	Building2D::Building2D() : Building(ZERO, ZERO, ZERO, ZERO) {
	}

	Building2D::Building2D(Position p1, Position p2, Position p3, Position p4,
		const mw::Sprite& rightWall, const mw::Sprite& leftWall, const mw::Sprite& roof)
		: Building(p1, p2, p3, p4), leftWall_(leftWall), rightWall_(rightWall), roof_(roof),
		height_(0.2f) {

	}

} // Namespace zombie.
