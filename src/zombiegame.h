#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "zombieengine.h"
#include "gameinterface.h"
#include "device.h"
#include "gamedataentry.h"
#include "terrain2d.h"
#include "explosion.h"
#include "unit2d.h"
#include "car2d.h"
#include "building2d.h"
#include "water.h"
#include "gameshader.h"
#include "player.h"

#include <mw/texture.h>
#include <mw/sprite.h>
#include <mw/sound.h>

#include <gui/component.h>

#include <map>
#include <memory>
#include <list>
#include <vector>

namespace zombie {

	template <class E, int _size>
	class Array {
	public:
		Array() : size_(0) {
		}

		inline E* create(const E& e) {
			return &(data_[size_++] = e);
		}

		inline typename std::array<E, _size>::iterator begin() {
			return data_.begin();
		}

		inline typename std::array<E, _size>::iterator end() {
			return data_.end();
		}

		inline typename std::array<E, _size>::const_iterator begin() const {
			return data_.cbegin();
		}

		inline typename std::array<E, _size>::const_iterator end() const {
			return data_.cend();
		}

		inline int size() const {
			return size_;
		}

		inline int getMaxSize() const {
			return data_.size();
		}

		E& operator[](int index) {
			return data_[index];
		}

		std::array<E, _size> data_;
		int size_;
	};

	// Responsible of loading map, units and initiate all
	// game related things and to start the game engine.
	// It also handle all game events triggered by the game engine,
	// e.g. what happens when a unit dies. All graphics and sound is
	// started here.
	class ZombieGame : public GameInterface, public gui::Component {
	public:
		ZombieGame(GameDataEntry zombieEntry);
		~ZombieGame();

		// Start the game.
		void startGame();

		// Draw the graphic and (deltaTime) should be the time past
		// from the previous call to this funtion.
		// Override, gui::Component.
		void draw(Uint32 deltaTime) override;

		void zoom(float scale);

	private:
		void updateGame(float deltaTime);

		void makeGameStep();

		void drawGame(float deltaTime);

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

		float getFps() const;

		void init();

		// Implements the GameInterface.
		void unitDied(Unit& unit) override;

		void collision(float impulse, Unit& unit) override;

		void collision(float impulse, Car& car) override;

		void collision(float impulse, Building& building) override;

		void shotMissed(Position startPosition, Position hitPosition) override;

		void shotHit(Position startPosition, Position hitPosition, Unit& unit) override;

		void explosion(Position position, float explosionRadius) override;

		void removedFromWorld(Unit& unit) override;
		// End of the GameInterface.

		void loadTerrain();
		
		float innerSpawnRadius_;
		float outerSpawnRadius_;
		float scale_;
		float lastSpawnTime_;
		float spawnPeriod_;

		ExplosionProperties explosionProperties_;
		
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
		GameDataEntry zombieEntry_;		

		std::vector<Position> spawningPoints_;

		Terrain2D terrain_;
		Water water_;

		int zombiesKilled_;
		int unitMaxLimit_;
		int nbrUnits_;
		float health_;
		int bulletsInWeapon_;
		int clipsize_;

		Array<Unit2D, 100> units_;
		Array<Car2D, 20> cars_;
		Array<Building2D, 100> buildings_;
		std::vector<std::unique_ptr<Player>> players_;
		Array<Explosion, 10> explosions_;

		int frame_;
		float fps_;
		float lastFramTime_;

		ZombieEngine engine_;
		GameShader gameShader_;
		bool started_;
		
		State humanState_;

		// Fix timestep.
		float timeStep_;
		float accumulator_;
	};

} // Namespace zombie.

#endif // ZOMBIEGAME_H
