#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "inputkeyboard.h"
//#include "inputjoystick.h"

#include "physicalengine.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"

#include "input.h"
#include "task.h"

#include <SDL.h>
#include <memory> //std::shared_ptr
#include <vector>
#include <string>
#include <map>

namespace zombie {

typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<HumanPlayer> HumanPlayerPtr;
typedef std::shared_ptr<AiPlayer> AiPlayerPtr;
typedef std::shared_ptr<RemotePlayer> RemotePlayerPtr;

class ZombieGame {
public:
    ZombieGame();
    ~ZombieGame();
    
	// Starts the game. The connection need to be active else nothing happens.
	void startGame();

	// Updates the game time by (msDeltaTime).
    void physicUpdate(Uint32 msDeltaTime);

	// Draws the graphic and (msDeltaTime) should be the time past 
	// from the previous call to this funtion.
	void graphicUpdate(Uint32 msDeltaTime);

	// Makes the game reacting on the evennt (windowEvent).
	void eventUpdate(const SDL_Event& windowEvent);
protected:
	// Updates
	void updateGameLogic(double time, double deltaTime);	

	// Add a human player (unitPtr) to the game.
	void addHuman(UnitPtr unitPtr);

	// Add a new ai (unitPtr) to the game.
	void addNewAi(UnitPtr unitPtr);
	
	void loadMap(std::string filename);

	// Returns the prefered game width in pixel size.
	double getWidth() const;
	
	// Returns the prefered game height in pixel size.
	double getHeight() const;

	// Inits the game.
	void initGame();

	// Returns a vector of all units visible by the unit (unit).
	std::vector<UnitPtr> calculateUnitsInView(const UnitPtr& unit);	
	void calculateWeaponShooting();

	void doShotDamage(UnitPtr shooter, Bullet properties);
	bool isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const;

	double width_, height_; // The internal map size in the game.
	
	TaskManager* taskManager_;
	unsigned int maxConnections_; // Max number of remove connections allowed.

	bool started_; // The game time is started.
	
	double time_; // Local game time.
	
	// Last added unit id.
	int unitId_;

	typedef std::pair<HumanPlayerPtr,UnitPtr> PairHumanUnit;
	typedef std::pair<AiPlayerPtr,UnitPtr> PairAiUnit;
	
	std::vector<PairHumanUnit> humanPlayers_;
	std::vector<PairAiUnit> aiPlayers_;
	PhysicalEngine* physicalEngine_;
	
	std::map<int,UnitPtr> units_; // <object id, Unit> All units.
	int localUnitId_; // Local 

	std::vector<BuildingPtr> buildings_; // All buildings_.
};

} // namespace zombie

#endif // ZOMBIEGAME_H