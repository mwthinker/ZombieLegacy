#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "gameinterface.h"
#include "device.h"
#include "gamedata.h"
#include "map2d.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <gui/component.h>

#include <map>

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
		void humanPosition(float x, float y) override;

		float innerSpawnRadius_;
		float outerSpawnRadius_;
		Position viewPosition_;
		float scale_;
		
		ZombieEngine engine_;

		DevicePtr keyboard1_, keyboard2_;
		GameData gameData_;
		Map2D map2d_;
	};

} // Namespace zombie_;

#endif // ZOMBIEGAME_H
