#include "gun.h"
#include "weaponinterface.h"
#include "state.h"
#include "unit.h"
#include "box2ddef.h"
#include "closestraycastcallback.h"
#include "laser.h"

namespace zombie {

	Weapon2D loadGun(GameInterface* gameInterface, GameDataEntry& entry) {
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
		
		bool hasLaser = entry.getChildEntry("laserSight").getBool();
		std::shared_ptr<Laser> laser;
		if (hasLaser) {
			laser = loadLaser(entry.getParent().getParent().getChildEntry("laserSight"));
		}
		auto gun = std::make_shared<Gun>(damage, timeBetweenShots, range, clipSize, shoot, reload);

		if (hasLaser) {
			return Weapon2D(gun, symbolImage, animation, size, grip, *laser);
		} else {
			return Weapon2D(gun, symbolImage, animation, size, grip);
		}
		
	}

	namespace {

		void doShotDamage(b2World& world, GameInterface& gameInterface, float damage, float angle, float range, Unit& shooter) {
			b2Vec2 dir(std::cos(angle), std::sin(angle));
			b2Vec2 hitPosition = shooter.getPosition() + range * dir;

			// Return the closest physcal object.
			ClosestRayCastCallback callback([](b2Fixture* fixture) {
				return !fixture->IsSensor();
			});

			world.RayCast(&callback, shooter.getPosition(), hitPosition);
			b2Fixture* fixture = callback.getFixture();

			hitPosition = shooter.getPosition() + range * callback.getFraction() * dir;

			// Did bullet hit something?
			if (fixture != nullptr) {
				Object* ob = static_cast<Object*>(fixture->GetUserData());

				if (Unit* target = dynamic_cast<Unit*>(ob)) {
					gameInterface.shotHit(shooter.getPosition(), hitPosition, *target);
					// Target alive?
					if (!target->isDead()) {
						target->updateHealthPoint(-damage);
					}
				} else {
					// Calculate the hit position on the unknown object.
					gameInterface.shotMissed(shooter.getPosition(), hitPosition);
				}
			} else {
				gameInterface.shotMissed(shooter.getPosition(), hitPosition);
			}
		}

	}

	Gun::Gun(float damage, float timeBetweenShots, float range, int clipSize, mw::Sound shot, mw::Sound reload) : shot_(shot), reload_(reload) {
		damage_ = damage;
		timeBetweenShots_ = timeBetweenShots;
		range_ = range;
		clipSize_ = clipSize;
		bulletsInWeapon_ = clipSize;
		lastShotTime_ = 0.0f;
	}

	void Gun::pullTrigger(Unit& unit, float time) {
		if (lastShotTime_ + timeBetweenShots_ < time) {
			if (bulletsInWeapon_ > 0) {
				lastShotTime_ = time;
				--bulletsInWeapon_;
				mw::Sound sound = shot_;
				sound.play();
				doShotDamage(*world_, *gameInterface_, damage_, unit.getDirection(), range_, unit);
			}
		}
	}

	void Gun::releaseTrigger(Unit& unit, float time) {
	}

	int Gun::getClipSize() const {
		return clipSize_;
	}

	int Gun::getBulletsInWeapon() const {
		return bulletsInWeapon_;
	}

	// Tries to reload the weapon. If it reloads return true, else false.
	void Gun::reload(float time) {
		if (bulletsInWeapon_ == clipSize_) {
			// No need to reload.
		} else {
			mw::Sound sound = reload_;
			sound.play();
			bulletsInWeapon_ = clipSize_;
		}
	}

	float Gun::getRange() const {
		return range_;
	}

	void Gun::initEngine(b2World* world, GameInterface* gameInterface) {
		gameInterface_ = gameInterface;
		world_ = world;
	}

	WeaponInterfacePtr Gun::clone() const {
		return std::make_shared<Gun>(*this);
	}

} // Namespace zombie.
