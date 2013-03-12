#include <memory>

#include <Box2D\Box2D.h>
#include <mw/mathvector.h>

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

namespace zombie {

	// MathVector.
	typedef b2Vec2 Velocity;
	typedef b2Vec2 Position;
	typedef b2Vec2 Force;
	typedef b2Vec2 Vec3;
	
	// Smart pointers.
	class AiBehavior;
	typedef std::shared_ptr<AiBehavior> AiBehaviorPtr;

	class Building;
	typedef std::shared_ptr<Building> BuildingPtr;

	class Unit;
	typedef std::shared_ptr<Unit> UnitPtr;

	class HumanPlayer;
	typedef std::shared_ptr<HumanPlayer> HumanPlayerPtr;

	class AiPlayer;
	typedef std::shared_ptr<AiPlayer> AiPlayerPtr;

	class Player;
	typedef std::shared_ptr<Player> PlayerPtr;	

} // Namespace zombie.

#endif // TYPEDEFS_H