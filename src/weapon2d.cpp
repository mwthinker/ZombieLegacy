#include "weapon2d.h"
#include "animation.h"
#include "auxiliary.h"
#include "gamedataentry.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {

	Weapon2D::Weapon2D() {
	}

	Weapon2D::Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
		Animation animation, float size, Position grip) :
		Weapon(weaponInterface), symbol_(symbol), animation_(animation), grip_(grip), size_(size) {
	}

	void Weapon2D::drawSymbol(float timeStep) {
		symbol_.draw();
	}

	void Weapon2D::draw(float timeStep, float x, float y, const GameShader& shader) {
		animation_.draw(timeStep, x + grip_.x, y + grip_.y, size_, size_, shader);
	}

	WeaponPtr Weapon2D::clone() const {
		return std::make_shared<Weapon2D>(*this);
	}
	
} // Namespace zombie.
