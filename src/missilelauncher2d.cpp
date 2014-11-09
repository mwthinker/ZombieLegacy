#include "missilelauncher2d.h"
#include "gameinterface.h"
#include "gamedataentry.h"

#include <cassert>

namespace zombie {

	Weapon2D loadMissileLauncher2D(GameInterface* gameInterface, GameDataEntry& entry) {
		mw::Sprite symbolImage = entry.getChildEntry("symbolImage").getSprite();

		float timeBetweenShots = entry.getChildEntry("timeBetweenShots").getFloat();
		int clipSize = entry.getChildEntry("clipSize").getInt();

		mw::Sound shotSound = entry.getChildEntry("shootSound").getSound();
		mw::Sound reloadSound = entry.getChildEntry("reloadSound").getSound();
		Animation animation = entry.getChildEntry("moveAnimation").getAnimation();
		float size = entry.getChildEntry("size").getFloat();
		Position grip;
		grip.x = entry.getChildEntry("moveImageGripX").getFloat();
		grip.y = entry.getChildEntry("moveImageGripY").getFloat();

		GameDataEntry projectile = entry.getChildEntry("projectile");
		float damageRadius = projectile.getChildEntry("damageRadius").getFloat();
		float damage = projectile.getChildEntry("damage").getFloat();
		float range = projectile.getChildEntry("range").getFloat();
		float deathTime = projectile.getChildEntry("deathTime").getFloat();
		float speed = projectile.getChildEntry("speed").getFloat();
		float force = projectile.getChildEntry("force").getFloat();

		//if (true && projectile.isAttributeEqual("type", "missile")) {
		auto missileLauncher = std::make_shared<MissileLauncher2D>(clipSize, timeBetweenShots,
			range, damageRadius, damage, deathTime, speed, force, shotSound, reloadSound);
		return Weapon2D(missileLauncher, symbolImage, animation, size, grip);
	}

} // Namespace zombie.
