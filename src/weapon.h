#ifndef WEAPON_H
#define WEAPON_H

#include "weaponinterface.h"
#include "state.h"
#include "gameinterface.h"
#include "gameshader.h"

#include <memory>

namespace zombie {

	class Weapon;
	typedef std::shared_ptr<Weapon> WeaponPtr;

	// Describes a Weapon and is responsible of shooting.
	class Weapon {
	public:
		Weapon() : weaponInterface_(nullptr), laserSight_(false) {
		}

		Weapon(bool laserSight) : weaponInterface_(nullptr), laserSight_(laserSight) {
		}

		Weapon(const Weapon& weapon) {
			weaponInterface_ = weapon.weaponInterface_->clone();
		}

		Weapon& operator=(const Weapon& weapon) {
			weaponInterface_ = weapon.weaponInterface_->clone();
			return *this;
		}

		Weapon(const WeaponInterfacePtr& weaponInterface) : weaponInterface_(weaponInterface) {
		}

		inline void pullTrigger(Unit& shooter, float time) {
			weaponInterface_->pullTrigger(shooter, time);
		}

		inline void releaseTrigger(Unit& shooter, float time) {
			weaponInterface_->releaseTrigger(shooter, time);
		}

		// Tries to reload the weapon. If it reloads return true, else false.
		inline void reload(float time) {
			weaponInterface_->reload(time);
		}

		inline int getClipSize() const {
			return weaponInterface_->getClipSize();
		}

		inline int getBulletsInWeapon() const {
			return weaponInterface_->getBulletsInWeapon();
		}

		// Draw the weapon symbol.
		virtual void drawSymbol(float timeStep) = 0;

		// Draw the weapon. The function will be called when the physical object is drawn.
		virtual void draw(float timeStep, float x, float y, const GameShader& shader) = 0;

		inline float getRange() const {
			return weaponInterface_->getRange();
		}

		virtual WeaponPtr clone() const = 0;

		// Should be called by the zombieEngine.
		void init(b2World* world, GameInterface* gameInterface) {
			weaponInterface_->initEngine(world, gameInterface);
		}

		inline bool hasLaserSight() const {
			return laserSight_;
		}

		virtual void updateLaserSight(b2World* world, float time, Position position, float angle) = 0;

	private:
		WeaponInterfacePtr weaponInterface_;
		bool laserSight_;
	};

} // Namespace zombie.

#endif // WEAPON_H
