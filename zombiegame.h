#ifndef ZOMBIEGAME_H
#define ZOMBIEGAME_H

#include "inputkeyboard.h"
//#include "inputjoystick.h"

#include "mw/localmanager.h"
#include "mw/servermanager.h"
#include "mw/clientmanager.h"

#include "physicalengine.h"
#include "unit.h"
#include "building.h"
#include "bullet.h"

#include "input.h"
#include "task.h"

#include "protocol.h"

#include <SDL.h>
#include <memory> //std::shared_ptr
#include <vector>

#include <string>
#include <fstream>

namespace zombie {

typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<HumanPlayer> HumanPlayerPtr;
typedef std::shared_ptr<AiPlayer> AiPlayerPtr;
typedef std::shared_ptr<RemotePlayer> RemotePlayerPtr;

// Represent a breach of protocol in the network data transmits.
class ProtocolError {
};

enum Status {STATUS_LOCAL, STATUS_SERVER, STATUS_CLIENT};

class ZombieManager : public mw::ServerFilter {
public:
    ZombieManager();
    ~ZombieManager();

	// Connect by choosing the connection type. Set max number of remote
	// connection (maxConnections).
	void connect(unsigned int maxConnections, Status status);
    
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
	virtual void updateGameLogic(double time, double deltaTime) = 0;

	// Returns a vector of all units visible by the unit (unit).
	virtual std::vector<UnitPtr> calculateUnitsInView(const UnitPtr& unit) = 0;

	// Add a human player (unitPtr) to the game.
	void addHuman(UnitPtr unitPtr);

	// Add a new ai (unitPtr) to the game.
	void addNewAi(UnitPtr unitPtr);

	// Add a remote unit (unitPtr).
	void addNewRemote(UnitPtr unitPtr);

	// Override ServerFilter
	bool sendThrough(const mw::Packet& packet, int fromId, int toId, mw::ServerFilter::Type type);

	// Receives data (data) received from user with id (id).
	// First element in (data) must be of a value 
	// defined in PacketType.
	void clientReceiveData(mw::Packet& packet, int id);

	void clientReceiveUpdate(mw::Packet& packet);
	
	void sendAllClientInput(double time, double deltaTime);

	void serverReceiveClientInput(mw::Packet packet, int fromId);
	
	// Shows the protocol for UpdatePacket.
	// N is the number of cars.
	// ---------------
	//   Net net
	//   int id
	//   double time
	// 1 int id
	// 1 Input input
	// 1 State state
	//      ...
	// N int id
	// N Input input
	// N State state
	// ---------------
	void serverSendAllClientUpdate();

	void serverSendNewUnit(UnitPtr newUnit);

	void clientReceiveNewUnit(mw::Packet packet);

	void clientReceiveId(mw::Packet& packet);

	void serverSendId(int toId, int unitId);

	void serverSendStart();

	void serverSendInit(int toId);
	void clientReceiveInit(mw::Packet packet);
	void loadMap(std::string filename);

	mw::ConnectionManager* manager_;
	PhysicalEngine* physicalEngine_;
	
	std::map<int,UnitPtr> units_; // <object id, Unit> All units.
	int localUnitId_; // Local 

	std::vector<BuildingPtr> buildings_; // All buildings_.


	typedef std::pair<HumanPlayerPtr,UnitPtr> PairHumanUnit;
	typedef std::pair<AiPlayerPtr,UnitPtr> PairAiUnit;
	typedef std::pair<RemotePlayerPtr,UnitPtr> PairRemoteUnit;

private:
	// Should only be called by the server. And must be called directly after the connection
	// is started (before anything else is done using the TaskManager*.
	void serverInitGame();

	double width_, height_; // The internal map size in the game.
	
	TaskManager* taskManager_;
	unsigned int maxConnections_; // Max number of remove connections allowed.

	bool started_; // The game time is started.
	
	double time_; // Local game time.
	
	// Last added unit id.
	int unitId_;
	
	std::vector<PairHumanUnit> humanPlayers_;
	std::vector<PairAiUnit> aiPlayers_;
	std::vector<PairRemoteUnit> remotePlayers_;	
};

class ZombieGame : public ZombieManager {
public:
	// Returns the prefered game width in pixel size.
	double getWidth() const;
	
	// Returns the prefered game height in pixel size.
	double getHeight() const;
private:
	void updateGameLogic(double time, double deltaTime);

	std::vector<UnitPtr> calculateUnitsInView(const UnitPtr& unit);	
	void calculateWeaponShooting();

	void doShotDamage(UnitPtr shooter, Bullet properties);
	bool isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const;
};

} // namespace zombie

#endif // ZOMBIEGAME_H