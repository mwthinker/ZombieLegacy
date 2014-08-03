#ifndef BOX2DDEF_H
#define BOX2DDEF_H

#include <Box2D/Box2D.h>

#include <vector>

namespace zombie {

	const float PI = b2_pi;

	const b2Vec2 ORIGO(0, 0);
	const b2Vec2 ZERO(0, 0);

	// Math vectors.
	typedef b2Vec2 Velocity;
	typedef b2Vec2 Position;
	typedef b2Vec2 Force;
	typedef b2Vec2 Vec3;
	typedef b2Vec2 Point;
	typedef std::vector<Position> Points;

} // Namespace zombie.

#endif // BOX2DDEF_H
