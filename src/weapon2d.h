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

	// Describes a Weapon and is responsible of shooting.
	class Weapon2D : public Weapon {
	public:
		Weapon2D();

		Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
			Animation animation, float size, Position grip);

		void drawSymbol(float timeStep) override;

		void draw(float timeStep, float x, float y, const GameShader& shader) override;

		WeaponPtr clone() const override;

	private:
		mw::Sprite symbol_;
		Animation animation_;
		float size_;
		Position grip_;
	};

} // Namespace zombie.

#endif // WEAPON2D_H
