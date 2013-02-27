#include <memory>

#include <mw/mathvector.h>

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

namespace zombie {

	// MathVector.
	typedef mw::MathVector Velocity;
	typedef Velocity Position;
	typedef Velocity Force;
	typedef Velocity Vec3;

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

	class PhysicalUnitInterface;
	typedef std::shared_ptr<PhysicalUnitInterface> PhysicalUnitPtr;
	
	class StaticPhyscalUnit;
	typedef std::shared_ptr<StaticPhyscalUnit> StaticPhyscalUnitPtr;

} // Namespace zombie.

#endif // TYPEDEFS_H