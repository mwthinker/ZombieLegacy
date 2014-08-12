#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "box2ddef.h"
#include "animation.h"
#include "weapon.h"

#include <vector>
#include <string>

namespace zombie {

	class DataInterface {
	public:
		// Data.
		virtual void loadZombie(float mass, float radius, float life, float walkingSpeed, float runningSpeed, float stamina, const Animation& moveA, Position grip, const Animation& injuredA, const Animation& dieA, const mw::Sound& die, const mw::Sound& hitSound, std::string weapon) = 0;
		virtual void loadHuman(float mass, float radius, float life, float walkingSpeed, float runningSpeed, float stamina, const Animation& moveA, Position grip, const Animation& injuredA, const Animation& dieA, const mw::Sound& die, const mw::Sound& hitSound, std::string weapon) = 0;
		virtual void loadCar(float mass, float width, float length, float life, const Animation& animation) = 0;
		virtual void loadWeapon(std::string name, float damage, float timeBetweenShots, float range, int clipSize, const mw::Sprite& symbol, const Animation& moveAnimation, float size, Position grip, const mw::Sound& shoot, const mw::Sound& reload) = 0;
		
		virtual void loadExplosion(const mw::Texture& particle, const mw::Sprite& shockwave, const mw::Sprite& emitter, const mw::Sound& sound) = 0;
		virtual void loadFog(const mw::Texture& fog, float radius, const mw::Color& color) = 0;
		
		// Map.
		virtual void loadTree(const Position& position) = 0;
		virtual void loadBuilding(const std::vector<Position>& corners) = 0;
		virtual void loadRoad(const std::vector<Position>& road) = 0;
		virtual void loadWater(const std::vector<Position>& positions) = 0;
		virtual void loadSpawningPoint(const Position& position) = 0;
	
	protected:
		// To avoid virtual destructor.
		~DataInterface() {
		}
	};

} // Namespace zombie.

#endif // DATAINTERFACE_H
