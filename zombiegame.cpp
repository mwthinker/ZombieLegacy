#include "inputkeyboard.h"
#include "zombiegame.h"

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

    ZombieManager::ZombieManager() {
		manager_ = 0;
		taskManager_ = new TaskManager();
		physicalEngine_ = new PhysicalEngine();
		
		started_ = false;
		time_ = 0.0;
		unitId_ = 0;

		HumanPlayerPtr humanPlayer(new InputKeyboard(SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_SPACE,SDLK_r));
		humanPlayers_.push_back(PairHumanUnit(humanPlayer,UnitPtr()));
		int a = 500;
	}

    ZombieManager::~ZombieManager() {
		delete manager_;
	}

	void ZombieManager::connect(unsigned int maxConnections, Status status) {
		if (manager_ == 0) {
			started_ = false;
			maxConnections_ = maxConnections;			

			switch (status) {
			case STATUS_LOCAL:
				manager_ = new mw::LocalManager(this);
				manager_->start();
				serverInitGame();
				break;
			case STATUS_SERVER:
				manager_ = new mw::ServerManager(10234,this);
				manager_->start();
				serverInitGame();
				break;
			case STATUS_CLIENT:
				manager_ = new mw::ClientManager(10234,"127.0.0.1");
				manager_->start();
				break;
			}
		}
	}
    
	void ZombieManager::startGame() {
		// Is active? And is game not started? And is server?
		if (manager_ != 0 && !started_ && manager_->getId() == manager_->getServerId()) {
			serverSendStart();
		}
	}	

    void ZombieManager::physicUpdate(Uint32 msDeltaTime) {
		if (manager_ != 0) {
			// Updates
			manager_->update();

			// Receive data.
			mw::Packet packet;
			int id;
			while (id = manager_->pullFromReceiveBuffer(packet)) {
				clientReceiveData(packet, id);
			}

			if (started_) {
				double deltaTime = msDeltaTime/1000.0;

				// Calculate all ai:s input.
				for (auto& pair : aiPlayers_) {
					AiPlayerPtr& aiPlayer = pair.first;					
					UnitPtr& unit = pair.second;
					std::vector<UnitPtr> unitsInView = calculateUnitsInView(unit);
					aiPlayer->calculateInput(unit,unitsInView,time_);
				}

				// Send input for all local units to server.
				// Excecute rpc on the server!
				sendAllClientInput(time_+deltaTime, deltaTime);
				
				// Is server?
				if (manager_->getId() == manager_->getServerId()) {
					updateGameLogic(time_,deltaTime);

					// Update physics.
					physicalEngine_->update(deltaTime);
					
					// Send update data.
					serverSendAllClientUpdate();
				}
				
				time_ += deltaTime;
			}
		}
	}

	void ZombieManager::graphicUpdate(Uint32 msDeltaTime) {
		if (started_) {
			taskManager_->update(msDeltaTime/1000.0);
		} else {
			taskManager_->update(0.0);
		}
	}

	void ZombieManager::eventUpdate(const SDL_Event& windowEvent) {
		if (started_) {
			// Update all human input.
			for (auto& pair : humanPlayers_) {
				HumanPlayerPtr& humanPlayer = pair.first;
				humanPlayer->eventUpdate(windowEvent);
			}
		}
	}

// protected
	void ZombieManager::updatePlayer(PlayerPtr player, UnitPtr& unit) {
		if (HumanPlayerPtr humanPlayer = std::dynamic_pointer_cast<HumanPlayer>(humanPlayer)) {
		} else if (AiPlayerPtr ai = std::dynamic_pointer_cast<AiPlayer>(player)) {
			std::vector<UnitPtr> unitsInView = calculateUnitsInView(unit);
			ai->calculateInput(unit,unitsInView,time_);
		}
	}

	void ZombieManager::addHuman(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		units_[unitPtr->id()] = unitPtr;
		humanPlayers_[0].second = unitPtr;
		localUnitId_ = unitPtr->id();
	}

	void ZombieManager::addNewAi(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		units_[unitPtr->id()] = unitPtr;
		AiPlayerPtr aiPlayer(new AiPlayer());
		aiPlayers_.push_back(PairAiUnit(aiPlayer,unitPtr));
	}

	void ZombieManager::addNewRemote(UnitPtr unitPtr) {
		taskManager_->add(new HumanAnimation(unitPtr));
		physicalEngine_->add(unitPtr);
		units_[unitPtr->id()] = unitPtr;
		RemotePlayerPtr remote(new RemotePlayer());
		remotePlayers_.push_back(PairRemoteUnit(remote,unitPtr));
	}

	// Override ServerFilter
	bool ZombieManager::sendThrough(const mw::Packet& packet, int fromId, int toId, mw::ServerFilter::Type type) {
		//std::cout << "SSESES" << std::endl;
		switch (type) {
		case mw::ServerFilter::NEW_CONNECTION:
			// Game not started and connection allowed?
			if (!started_ && manager_->getNbrOfConnections() + 1 <= maxConnections_) {
				serverSendInit(fromId);
				UnitPtr unit(new Unit(3,3,0.3,Weapon(35,0.5,8,12),false,++unitId_));
				addNewRemote(unit);
				serverSendNewUnit(unit);
				serverSendId(fromId, unit->id());

				return true;  // Accept new connection.
			}
			return false;
		case mw::ServerFilter::DISCONNECTED:
			return true; // Doesn't matter.
		case mw::ServerFilter::PACKET:
			{
				Net net = (Net) packet[0];
				switch (net) {
				case NET_INPUT:
					serverReceiveClientInput(packet,fromId);
					return false;
				default:
					// Not server?
					if (manager_->getServerId() != fromId) {
						throw ProtocolError();
					}

					// Let server through and send data.
					return true;
				};
			}
			break;
		}
		return true;
	}

	// Receives data (data) received from user with id (id).
	// First element in (data) must be of a value 
	// defined in PacketType.
	void ZombieManager::clientReceiveData(mw::Packet& packet, int id) {
		Net net = (Net) packet[0];
		switch (net) {
		case NET_UPDATE:
			clientReceiveUpdate(packet);
			break;
		case NET_INIT:
			// Real client (not server)?
			if (manager_->getServerId() != manager_->getId()) {
				clientReceiveInit(packet);
			}
			break;
		case NET_NEW_UNIT:
			// Real client (not server)?
			if (manager_->getServerId() != manager_->getId()) {
				clientReceiveNewUnit(packet);
			}
			break;
		case NET_ID:
			clientReceiveId(packet);
			break;
		case NET_START:
			started_ = true;
			break;
		}
	}

	void ZombieManager::clientReceiveUpdate(mw::Packet& packet) {
		Net net;
		packet >> net;
		double time;
		packet >> time;
		int id;
		packet >> id;
		Input input;
		packet >> input;
		State state;
		packet >> state;
	}
	
	void ZombieManager::sendAllClientInput(double time, double deltaTime) {
		// NET_CLIENT_INPUT       Net 		
		// clientTime             double		
		// id1 for unit1          int	
		// input1 for unit1       Input
		// ...		
		// idN for unitN          int
		// inputN for unitN       Input
		mw::Packet inputPacket;
		inputPacket << Net::NET_INPUT;
		inputPacket << time + deltaTime;
		for (auto& pair : humanPlayers_) {
			HumanPlayerPtr& human = pair.first;
			UnitPtr& unit = pair.second;
			inputPacket << unit->id();
			inputPacket << human->currentInput();
		}

		for (auto& pair : aiPlayers_) {
			AiPlayerPtr& ai = pair.first;
			UnitPtr& unit = pair.second;
			inputPacket << unit->id();
			inputPacket << ai->currentInput();
		}

		for (auto& pair : remotePlayers_) {
			RemotePlayerPtr& remote = pair.first;
			UnitPtr& unit = pair.second;
			inputPacket << unit->id();
			inputPacket << remote->currentInput();
		}

		manager_->pushToSendBuffer(inputPacket);
	}	

	void ZombieManager::serverReceiveClientInput(mw::Packet packet, int fromId) {
		Net net;
		packet >> net;
		double time ;
		packet >> time;

		while (packet.dataLeftToRead() > 0) {
			int objectId;
			packet >> objectId;
			Input input;
			packet >> input;

			units_[objectId]->receiveInput(time_, time, input);
		}
	}
	
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
	void ZombieManager::serverSendAllClientUpdate() {
		mw::Packet updatePacket;
		updatePacket << Net::NET_UPDATE << time_;
		
		for (auto& pair: units_) {
			UnitPtr& unit = pair.second;
			updatePacket << unit->id() << unit->getInput() << unit->getState();
		}

		// Send UpdatePacket unreliable to all clients.
		manager_->pushToSendBuffer(updatePacket,mw::PacketType::UN_RELIABLE);
	}

	void ZombieManager::serverSendNewUnit(UnitPtr newUnit) {
		mw::Packet packet;
		packet << Net::NET_NEW_UNIT;
		packet << newUnit->generatePacket();
		manager_->pushToSendBuffer(packet,mw::PacketType::RELIABLE);
	}

	void ZombieManager::clientReceiveNewUnit(mw::Packet packet) {
		Net net;
		packet >> net;
		UnitPtr unit(new Unit(packet));
		units_[unit->id()] = unit;
	}

	void ZombieManager::clientReceiveId(mw::Packet& packet) {
		Net net;
		packet >> net;
		packet >> localUnitId_;
		addHuman(units_[localUnitId_]);
	}

	void ZombieManager::serverSendId(int toId, int unitId) {
		mw::Packet packet;
		packet << Net::NET_ID;
		packet << unitId;
		manager_->pushToSendBuffer(packet,mw::PacketType::RELIABLE,toId);
	}

	void ZombieManager::serverSendStart() {
		mw::Packet packet;
		packet << Net::NET_START;
		manager_->pushToSendBuffer(packet,mw::PacketType::RELIABLE);
	}

	void ZombieManager::serverSendInit(int toId) {
		// dataType               Net
		// mapWidth, mapHeight    double, double
		// N                      char
		// building1              Building
		// ...
		// buildingN
		// K                      char
		// unit1                  Unit
		// ...
		// unitK
		mw::Packet packet;
		packet << Net::NET_INIT;
		packet << width_ << height_;
		packet << (char) buildings_.size();
		for (BuildingPtr building : buildings_) {
			packet << building->generatePacket();
		}
		for (auto& pair : units_) {
			UnitPtr& unit = pair.second;
			packet << unit->generatePacket();
		}

		manager_->pushToSendBuffer(packet,mw::PacketType::RELIABLE,toId);
	}

	void ZombieManager::clientReceiveInit(mw::Packet packet) {		
		Net net;
		packet >> net;
		packet >> width_ >> height_;
		int n;
		packet >> n;
		for (int i = 0; i < n; ++i) {
			BuildingPtr building(new Building(packet));
			buildings_.push_back(building);
		}
		packet >> n;
		for (int i = 0; i < n; ++i) {
			UnitPtr unit(new Unit(packet));
			addNewRemote(unit);
		}
	}

	void ZombieManager::loadMap(std::string filename) {
		std::fstream mapFile(filename.c_str(),std::fstream::in);
		
		if (mapFile.good()) {
			mapFile >> width_ >> height_;
		}
		
		while (mapFile.good()) {
			std::vector<Position> corners;
			while (mapFile.good()) {
				Position p;
				mapFile >> p.x_ >> p.y_;
				if (p.x_ < 0 || p.y_ < 0) {
					break;
				} else {
					corners.push_back(p);
				}
			}
			BuildingPtr building(new Building(corners,++unitId_));
			buildings_.push_back(building);

			taskManager_->add(new DrawBuildning(building));
			physicalEngine_->add(building);
			//std::cout << "A";
		}
		mapFile.close();
	}

	// private
	void ZombieManager::serverInitGame() {
		// Add human controlled by first input device.
		UnitPtr human(new Unit(8,10,0.3,Weapon(35,0.5,8,12),false,++unitId_));		
		addHuman(human);

		// Add zombie with standard behavior.
		for (int i = 8; i < 11; i++){
			for(int j = 8; j < 11; j++) {				
				UnitPtr zombie(new Unit(8+i,10+j,0.3*i+j,Weapon(35,0.5,8,12),false,++unitId_));				
				addNewAi(zombie);
			}
		}

		loadMap("buildings.txt");
	}

	// ZombieGame

	double ZombieGame::getWidth() const {
		return 400.0;
	}

	double ZombieGame::getHeight() const {
		return 400.0;
	}

	void ZombieGame::updateGameLogic(double time, double deltaTime) {
	}

	std::vector<UnitPtr> ZombieGame::calculateUnitsInView(const UnitPtr& unit) {
		std::vector<UnitPtr> unitsInView;
		for (auto& pair: units_) {
			UnitPtr& distantUnit = pair.second;
			Position p = distantUnit->getPosition();
			if (unit->isPointViewable(p.x_,p.y_) && isVisible(distantUnit,unit)) {
				unitsInView.push_back(distantUnit);
			}
		}

		return unitsInView;
	}
	
	void ZombieGame::calculateWeaponShooting() {
		/*
		for (auto& pair : units_) {
			UnitPtr& unit = pair.second;
			Bullet properties;
			bool shooting = unit->shooting(properties);
			if (!unit->isDead() && shooting) {
				if (properties.speed_ < 0) {
					// Weapon with infinit velocity
					// Calculate damage and target, and send hitevent.
					doShotDamage(unit, properties);
				} else {

				}
			}
		}
		*/
	}

	void ZombieGame::doShotDamage(UnitPtr shooter, Bullet properties) {
		double step = 0.0;
		double stepLength = 0.05;
		Position dr = Position(std::cos(properties.direction_),std::sin(properties.direction_));
		Position p = properties.postion_;
		bool hit = false;
		while (step < properties.range_ && !hit) {
			p += dr*stepLength;
			step += stepLength;
			/*
			for (std::vector<Object*>::iterator it = objects_.begin(); it != objects_.end(); ++it) {
				Object* ob = *it;
				if (shooter != ob && !ob->isDead() && ob->isInside(p.x_,p.y_)) {
					WeaponHit* hitEvent = new WeaponHit(ob->id(),p);
					ob->updateHealthPoint(-properties.pointDamage_);
					eventQueue_.add(hitEvent);
					hit = true;
					break;
				}
				//std::cout << step <<"\n";
			}*/
		}		
	}	

	bool ZombieGame::isVisible(UnitPtr unitToBeSeen, UnitPtr unitThatSees) const {
		Position dr = unitToBeSeen->getPosition() - unitThatSees->getPosition();
		double length = dr.magnitude();
		double stepLength = 0.1;
		double step = 0.0;
		dr = dr.normalize();
		
		Position p = unitThatSees->getPosition();
		while (step < length) {
			p += dr*stepLength;
			step += stepLength;
			for (const BuildingPtr& building : buildings_) {
				if (building->isInside(p.x_,p.y_)) {
					return false;
				}
			}
		}
		return true;
	}	

} // namespace zombie