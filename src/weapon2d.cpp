#include "weapon2d.h"
#include "animation.h"
#include "auxiliary.h"
#include "zombieentry.h"
#include "laser.h"
#include "closestraycastcallback.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {

	Weapon2D::Weapon2D() : laser_(nullptr) {
	}

	Weapon2D::Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
		Animation animation, float size, Position grip) :
		Weapon(weaponInterface), symbol_(symbol), animation_(animation),
		grip_(grip), size_(size), laser_(nullptr) {
		
	}

	Weapon2D::Weapon2D(const WeaponInterfacePtr& weaponInterface, mw::Sprite symbol,
		Animation animation, float size, Position grip, const Laser& laser) :
		Weapon(weaponInterface), symbol_(symbol), animation_(animation), 
		grip_(grip), size_(size), laser_(new Laser(laser)) {

	}

	Weapon2D::Weapon2D(const Weapon2D& weapon) :
		Weapon(weapon), symbol_(weapon.symbol_), animation_(weapon.animation_),
		grip_(weapon.grip_), size_(weapon.size_), laser_(nullptr) {

		if (weapon.laser_) {
			laser_ = std::unique_ptr<Laser>(new Laser(*weapon.laser_));
		}
	}

	void Weapon2D::draw(float timeStep, float x, float y, const GameShader& shader) {
		if (laser_) {
			laser_->update(x, y);
			laser_->draw(timeStep, shader);
		}
		shader.setGlColorU(1, 1, 1);
		animation_.draw(timeStep, x + grip_.x, y + grip_.y, size_, size_, shader);
	}

	void Weapon2D::updateLaserSight(b2World* world, float timeStep, Position position, float angle) {
		if (laser_) {
			b2Vec2 dir(std::cos(angle), std::sin(angle));

			// Return the closest physical object!
			ClosestRayCastCallback callback([](b2Fixture* fixture) {
				return !fixture->IsSensor() && fixture->GetBody()->GetUserData() != nullptr;
			});
			world->RayCast(&callback, position, position + getRange() * dir);
			float f = callback.getFraction();
			laser_->update(f*getRange());
		}
	}

	WeaponPtr Weapon2D::clone() const {
		return std::make_shared<Weapon2D>(*this);
	}
	
} // Namespace zombie.
