#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "gameinterface.h"
#include "device.h"
#include "gamedata.h"
#include "graphic.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <gui/component.h>

#include <map>
#include <memory>
#include <list>

namespace zombie {

	// Responsible of loading map, units and initiate all
	// game related things and to start the game engine.
	// It also handle all game events triggered by the game engine,
	// e.g. what happens when a unit dies. All graphics and sound is
	// started here.
	class ZombieGame : public GameInterface, public gui::Component {
	public:
		ZombieGame(const GameData& gameData);
		~ZombieGame();

		// Starts the game.
		void startGame();

		// Draws the graphic and (deltaTime) should be the time past
		// from the previous call to this funtion.
		void draw(Uint32 deltaTime);

		void zoom(float scale);

	private:
		void updateUnit(Unit& unit, Unit& human) override;

		void updateSpawning(Unit& human) override;

		void currentHuman(Unit& unit) override;

		void humanDied(Unit& unit) override;

		void unitDied(Unit& unit) override;

		void collision(float impulse, Car& car, Unit& unit) override;

		void collision(float impulse, Car& car1, Car& car2) override;

		void collision(float impulse, Car& car, Building& building) override;

		void shotMissed(const Bullet& bullet) override;

		void shotHit(const Bullet& bullet, Unit& unit) override;

		float innerSpawnRadius_;
		float outerSpawnRadius_;
		Position viewPosition_;
		float scale_;
		float lastSpawnTime_;
		float spawnPeriod_;

		ZombieEngine engine_;
		Terrain2D terrain2D_;

		DevicePtr keyboard1_, keyboard2_;
		const GameData& gameData_;

		std::list<std::shared_ptr<Graphic>> graphicGround_;
		std::list<std::shared_ptr<Graphic>> graphicMiddle_;
		std::list<std::shared_ptr<Graphic>> graphicHeaven_;
		std::vector<Position> spawningPoints_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
