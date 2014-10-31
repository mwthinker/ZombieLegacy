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

	void Weapon2D::draw(float timeStep, float x, float y, const GameShader& shader) {
		animation_.draw(timeStep, x + grip_.x, y + grip_.y, size_, size_, shader);
	}

	WeaponPtr Weapon2D::clone() const {
		return std::make_shared<Weapon2D>(*this);
	}

	Weapon2D loadWeapon2D(GameInterface* gameInterface, GameDataEntry& entry) {
		mw::Sprite symbolImage = entry.getChildEntry("symbolImage").getSprite();

		float timeBetweenShots = entry.getChildEntry("timeBetweenShots").getFloat();
		int clipSize = entry.getChildEntry("clipSize").getInt();

		mw::Sound shoot = entry.getChildEntry("shootSound").getSound();
		mw::Sound reload = entry.getChildEntry("reloadSound").getSound();
		Animation animation = entry.getChildEntry("moveAnimation").getAnimation();
		float size = entry.getChildEntry("size").getFloat();
		Position grip;
		grip.x = entry.getChildEntry("moveImageGripX").getFloat();
		grip.y = entry.getChildEntry("moveImageGripY").getFloat();

		GameDataEntry projectile = entry.getChildEntry("projectile");
		float damage = projectile.getChildEntry("damage").getFloat();
		float range = projectile.getChildEntry("range").getFloat();

		if (projectile.isAttributeEqual("type", "missile")) {
			auto missileLauncher = std::make_shared<MissileLauncher2D>(clipSize, timeBetweenShots, range, shoot, reload);
			return Weapon2D(missileLauncher, symbolImage, animation, size, grip);
		} else {
			auto gun = std::make_shared<Gun>(damage, timeBetweenShots, range, clipSize, shoot, reload);
			return Weapon2D(gun, symbolImage, animation, size, grip);
		}
	}
	
} // Namespace zombie.
