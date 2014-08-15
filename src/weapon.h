#ifndef WEAPON_H
#define WEAPON_H

#include "weaponinterface.h"
#include "state.h"
#include "gameinterface.h"

#include <memory>

namespace zombie {

	class Weapon;
	typedef std::shared_ptr<Weapon> WeaponPtr;

	// Describes a Weapon and is responsible of shooting.
	class Weapon {
	public:
		Weapon() : weaponInterface_(nullptr) {
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

		void pullTrigger(Unit& shooter, float time) {
			weaponInterface_->pullTrigger(shooter, time);
		}

		void releaseTrigger(Unit& shooter, float time) {
			weaponInterface_->releaseTrigger(shooter, time);
		}

		// Tries to reload the weapon. If it reloads return true, else false.
		void reload(float time) {
			weaponInterface_->reload(time);
		}

		int getClipSize() const {
			return weaponInterface_->getClipSize();
		}

		int getBulletsInWeapon() const {
			return weaponInterface_->getBulletsInWeapon();
		}

		// Draw the weapon symbol.
		virtual void drawSymbol() {
		}

		// Draw the weapon. The function will be called when the physical object is drawn.
		virtual void draw() {
		}

		float getRange() const {
			return weaponInterface_->getRange();
		}

		virtual WeaponPtr clone() const {
			return std::make_shared<Weapon>(*this);
		}

		// Should be called by the zombieEngine.
		virtual void init(b2World* world_, GameInterface* gameInterface) {
			weaponInterface_->initEngine(world_, gameInterface);
		}

	private:
		WeaponInterfacePtr weaponInterface_;
	};

} // Namespace zombie.

#endif // WEAPON_H
