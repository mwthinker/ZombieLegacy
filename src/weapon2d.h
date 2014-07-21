#ifndef WEAPON2D_H
#define WEAPON2D_H

#include "weapon.h"
#include "animation.h"

#include <mw/sprite.h>

namespace zombie {
	
	// Describes a Weapon and is responsible of shooting.
	class Weapon2D : public Weapon {
	public:
		Weapon2D() {
		}

		Weapon2D(float damage, float timeBetweenShots, float range, int clipSize, mw::Sprite symbol, Animation animation) : Weapon(damage, timeBetweenShots, range, clipSize), 
			symbol_(symbol), animation_(animation) {
		}

		void drawSymbol() override {
			symbol_.draw();
		}

		void draw() override {
			animation_.draw(0);
		}

		WeaponPtr clone() const override {
			return std::make_shared<Weapon2D>(*this);
		}

	private:
		mw::Sprite symbol_;
		Animation animation_;
	};

} // Namespace zombie.

#endif // WEAPON2D_H
