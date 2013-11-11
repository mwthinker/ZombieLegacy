#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "graphicentity.h"
#include "player.h"
#include "movingobject.h"
#include "state.h"

namespace zombie {

	class GameEntity {
	public:
		GameEntity(MovingObject* object, GraphicEntity* graphic, Player* player) {
			object_ = object;
			graphic_ = graphic;
			player_ = player;
			if (object_ != nullptr) {
				object_->gameEntity_ = this;
			}
		}

		~GameEntity() {
			delete object_;
			delete graphic_;
			delete player_;
		}

		void draw(float time, float timeStep, float accumulator) {
			if (graphic_ != nullptr) {
				graphic_->draw(time, timeStep, accumulator);
			}
		}

		void updatePhysics(float time, float deltaTime) {
			Input input;
			if (player_ != nullptr) {
				player_->calculateInput(time);
				input = player_->currentInput();
			}
			if (object_) {
				object_->updatePhysics(time, deltaTime, input);
			}
		}

		MovingObject* object_;
		GraphicEntity* graphic_;
		Player* player_;
	};

} // Namespace zombie.

#endif // GAMEENTITY_H
