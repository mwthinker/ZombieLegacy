#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "gameinterface.h"
#include "device.h"
#include "taskmanager.h"
#include "gamedata.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <map>

namespace zombie {

	// Responsible of loading map, units and initiate all
	// game related things and to start the game engine.
	// It also handle all game events triggered by the game engine,
	// e.g. what happens when a unit dies. All graphics and sound is
	// started here.
	class ZombieGame : public GameInterface {
	public:
		ZombieGame(const GameData& gameData);
		~ZombieGame();

		// Starts the game.
		void startGame();

		// Draws the graphic and (deltaTime) should be the time past 
		// from the previous call to this funtion.
		void update(float deltaTime);

		// Makes the game reacting on an event (windowEvent).
		void eventUpdate(const SDL_Event& windowEvent);

		void zoom(float scale);

		void updateSize(int width, int height);

	private:
		void humanPosition(float x, float y) override;

		float innerSpawnRadius_;
		float outerSpawnRadius_;
		Position viewPosition_;
		float scale_;

		// TaskManager must be defined before ZombieEngine if there 
		// are tasks that are binded as callbacks in the ZombieEngine.
		// I.e. the destructor of TaskManager must be called before TaskManager.
		TaskManager taskManager_;
		ZombieEngine engine_;

		DevicePtr keyboard1_, keyboard2_;
		GameData gameData_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
