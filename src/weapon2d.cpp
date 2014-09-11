#include "weapon2d.h"
#include "animation.h"
#include "auxiliary.h"

#include "missilelauncher2d.h"
#include "missile2d.h"
#include "gun.h"

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

	void Weapon2D::draw(float timeStep, const gui::WindowMatrixPtr& wPtr) {
		wPtr->useShader();
		wPtr->setColor(1, 1, 1);
		mw::Matrix44 old = wPtr->getModel();
		wPtr->setModel(old * mw::getTranslateMatrix(-grip_.x, -grip_.y) * mw::getScaleMatrix(size_, size_));
		animation_.draw(timeStep, wPtr);
		wPtr->setModel(old);
	}

	WeaponPtr Weapon2D::clone() const {
		return std::make_shared<Weapon2D>(*this);
	}

	Weapon2D loadWeapon2D(GameInterface* gameInterface, GameDataEntry& entry) {
		mw::Sprite symbolImage = entry.getSprite("symbolImage");

		float timeBetweenShots = entry.getFloat("timeBetweenShots");
		int clipSize = entry.getInt("clipSize");

		mw::Sound shoot = entry.getSound("shootSound");
		mw::Sound reload = entry.getSound("reloadSound");
		Animation animation = entry.getAnimation("moveAnimation");
		float size = entry.getFloat("size");
		Position grip;
		grip.x = entry.getFloat("moveImageGripX");
		grip.y = entry.getFloat("moveImageGripY");

		GameDataEntry projectile = entry.getChildEntry("projectile");
		float damage = projectile.getFloat("damage");
		float range = projectile.getFloat("range");

		if (projectile.isAttributeEqual("type", "missile")) {
			auto missile = loadMissile2D(gameInterface, projectile, damage, range);
			auto missileLauncher = std::make_shared<MissileLauncher2D>(missile, clipSize, timeBetweenShots, range, shoot, reload);
			return Weapon2D(missileLauncher, symbolImage, animation, size, grip);
		} else {
			return Weapon2D(std::make_shared<Gun>(damage, timeBetweenShots, range, clipSize, shoot, reload), symbolImage, animation, size, grip);
		}
	}
	
} // Namespace zombie.
