#include <memory>
#include <vector>

#include <mw/color.h>
#include <Box2D/Box2D.h>

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

namespace zombie {

	const float PI = 3.14159265f;

	const b2Vec2 ORIGO(0, 0);

	// Math vectors.
	typedef b2Vec2 Velocity;
	typedef b2Vec2 Position;
	typedef b2Vec2 Force;
	typedef b2Vec2 Vec3;
	typedef b2Vec2 Point;
	typedef std::vector<Position> Points;
	
	// Smart pointers.
	class AiBehavior;
	typedef std::shared_ptr<AiBehavior> AiBehaviorPtr;

	typedef mw::Color Color;
	const Color BLACK(0,0,0);
	const Color WHITE(1,1,1);

	const Color RED(1,0,0);
	const Color GREEN(0,1,0);
	const Color BLUE(0,0,1);

	const Color GRAY1(0.1,0.1,0.1);
	const Color GRAY2(0.2,0.2,0.2);
	const Color GRAY3(0.3,0.3,0.3);
	const Color GRAY4(0.4,0.4,0.4);

} // Namespace zombie.

#endif // TYPEDEFS_H
