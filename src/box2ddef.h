#ifndef BOX2DDEF_H
#define BOX2DDEF_H

#include <mw/matrix44.h>

#include <Box2D/Box2D.h>

#include <vector>

namespace zombie {

	const float PI = b2_pi;

	const b2Vec2 ORIGO(0, 0);
	const b2Vec2 ZERO(0, 0);

	// Math vectors.
	using Velocity = b2Vec2;
	using Position = b2Vec2;
	using Force = b2Vec2;
	using Vec3 = b2Vec2;
	using Point = b2Vec2;
	using Points = std::vector<Position>;

	using Matrix44 = mw::Matrix44<float>;

} // Namespace zombie.

#endif // BOX2DDEF_H
