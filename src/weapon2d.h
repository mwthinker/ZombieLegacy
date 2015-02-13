#ifndef WEAPON2D_H
#define WEAPON2D_H

#include "weapon.h"
#include "animation.h"
#include "weaponinterface.h"
#include "gameshader.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {

	class GameInterface;
	class GameDataEntry;
	class Laser;

	// Describes a Weapon and is responsible of shooting.
	class Weapon2D : public Weapon {
	public:
		Weapon2D();

		Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
			Animation animation, float size, Position grip);
		
		Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
			Animation animation, float size, Position grip, const Laser& laser);

		Weapon2D(const Weapon2D& weapon);
		
		inline const mw::Sprite& getSymbol() const override {
			return symbol_;
		}

		void draw(float timeStep, float x, float y, const GameShader& shader) override;

		void updateLaserSight(b2World* world, float time, Position position, float angle) override;

		WeaponPtr clone() const override;

	private:
		std::unique_ptr<Laser> laser_;
		mw::Sprite symbol_;
		Animation animation_;
		float size_;
		Position grip_;
	};

} // Namespace zombie.

#endif // WEAPON2D_H
