#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "gameinterface.h"
#include "device.h"
#include "gamedata.h"
#include "graphic.h"
#include "terrain2d.h"
#include "explosion.h"
#include "fog.h"
#include "weaponeffect.h"
#include "weaponinterface.h"
#include "unit2d.h"
#include "car2d.h"
#include "water.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <gui/component.h>

#include <map>
#include <memory>
#include <list>
#include <vector>

namespace zombie {
		
	class Weapon2D;

	// Responsible of loading map, units and initiate all
	// game related things and to start the game engine.
	// It also handle all game events triggered by the game engine,
	// e.g. what happens when a unit dies. All graphics and sound is
	// started here.
	class ZombieGame : public GameInterface, public gui::Component {
	public:
		ZombieGame(const GameData& gameData);
		~ZombieGame();

		// Start the game.
		void startGame();

		// Draw the graphic and (deltaTime) should be the time past
		// from the previous call to this funtion.
		// Override, gui::Component.
		void draw(Uint32 deltaTime) override;

		void zoom(float scale);

		int getZombiesKilled() const {
			return zombiesKilled_;
		}

		float getHealth() {
			return health_;
		}

		int getClipSize() {
			return clipsize_;
		}

		int getBulletsInWeapon() {
			return bulletsInWeapon_;
		}

		int getNbrUnits() {
			return engine_.getNbrUnits();
		}		

	private:
		void init();

		// Implements the GameInterface.
		void updateEachCycle(Unit& unit, Unit& human) override;

		void updateEachCycle(Unit& human) override;

		void humanDied(Unit& unit) override;

		void unitDied(Unit& unit) override;

		void collision(float impulse, Unit& unit) override;

		void collision(float impulse, Car& car) override;

		void collision(float impulse, Building& building) override;

		void shotMissed(Position startPosition, Position hitPosition) override;

		void shotHit(Position startPosition, Position hitPosition, Unit& unit) override;

		void explosion(Position position, float explosionRadius) override;

		void removedFromWorld(Unit& unit) override;

		// End of the GameInterface.

		Unit2D* createUnit(Unit2D& unit);
		Car2D* createCar(Car2D& car);

		void loadTerrain();
		
		float innerSpawnRadius_;
		float outerSpawnRadius_;
		float scale_;
		float lastSpawnTime_;
		float spawnPeriod_;

		ExplosionProperties explosionProperties_;
		//std::shared_ptr<Fog> fog_;
		
		Animation humanInjured_;
		Animation humanDie_;
		Animation zombieInjured_;
		Animation zombieDie_;
		mw::Sprite tree_;
		mw::Sprite wall_;
		mw::Music music_;

		Position viewPosition_;
		Position refViewPosition_;

		DevicePtr keyboard_;
		const GameData& gameData_;

		Unit2D human_;
		Unit2D zombie_;
		Car2D car_;

		std::list<std::shared_ptr<Graphic>> graphicGround_;
		std::list<std::shared_ptr<Graphic>> graphicMiddle_;
		std::list<std::shared_ptr<Graphic>> graphicSky_;
		std::vector<Position> spawningPoints_;
		std::map<std::string, Weapon2D> weapons_;

		Terrain2D terrain_;
		Water water_;

		int zombiesKilled_;
		int unitMaxLimit_;
		int nbrUnits_;
		float health_;
		int bulletsInWeapon_;
		int clipsize_;

		std::vector<Unit2D> units_;
		std::vector<Car2D> cars_;

		ZombieEngine engine_; // Must be destroyed first in order to remove box2d stuff.
	};

} // Namespace zombie.

#endif // ZOMBIEGAME_H
