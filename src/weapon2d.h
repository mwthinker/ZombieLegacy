#ifndef WEAPON2D_H
#define WEAPON2D_H

#include "weapon.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {
	
	// Describes a Weapon and is responsible of shooting.
	class Weapon2D : public Weapon {
	public:
		Weapon2D() {
		}

		Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
			Animation animation, float size, Position grip) : 
			Weapon(weaponInterface), symbol_(symbol), animation_(animation), grip_(grip), size_(size) {
		}

		void drawSymbol(float timeStep) override {
			symbol_.draw();
		}

		void draw(float timeStep, const gui::WindowMatrixPtr& wPtr) override {
			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			mw::Matrix44 old = wPtr->getModel();
			wPtr->setModel(old * mw::getTranslateMatrix(-grip_.x, -grip_.y) * mw::getScaleMatrix(size_, size_));
			animation_.draw(timeStep, wPtr);
			wPtr->setModel(old);
		}

		WeaponPtr clone() const override {
			return std::make_shared<Weapon2D>(*this);
		}

	private:
		mw::Sprite symbol_;
		Animation animation_;
		float size_;
		Position grip_;
	};

} // Namespace zombie.

#endif // WEAPON2D_H
