#include "missilelauncher2d.h"
#include "gameinterface.h"
#include "zombieentry.h"
#include "laser.h"

#include <cassert>

namespace zombie {

	Weapon2D loadMissileLauncher2D(GameInterface* gameInterface, ZombieEntry& entry) {
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

		ZombieEntry projectile = entry.getChildEntry("projectile");
		float damageRadius = projectile.getChildEntry("damageRadius").getFloat();
		float damage = projectile.getChildEntry("damage").getFloat();
		float range = projectile.getChildEntry("range").getFloat();
		float deathTime = projectile.getChildEntry("deathTime").getFloat();
		float speed = projectile.getChildEntry("speed").getFloat();
		float force = projectile.getChildEntry("force").getFloat();


		bool hasLaser = entry.getChildEntry("laserSight").getBool();
		std::shared_ptr<Laser> laser;
		if (hasLaser) {
			laser = loadLaser(entry.getParent().getParent().getChildEntry("laserSight"));
		}
		//if (true && projectile.isAttributeEqual("type", "missile")) {
		auto missileLauncher = std::make_shared<MissileLauncher2D>(clipSize, timeBetweenShots,
			range, damageRadius, damage, deathTime, speed, force, shotSound, reloadSound);

		if (hasLaser) {
			return Weapon2D(missileLauncher, symbolImage, animation, size, grip, *laser);
		} else {
			return Weapon2D(missileLauncher, symbolImage, animation, size, grip);
		}
	}

} // Namespace zombie.
