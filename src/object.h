#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>
#include <unordered_map>

namespace zombie {

	class GameEntity;

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class ZombieEngine;
		friend class GameEntity;

		inline Object() {
			id_ = ++lastId;
			gameEntity_ = nullptr;
		}

		virtual ~Object() {
		}

		virtual b2Body* getBody() const = 0;

		inline int getId() const {
			return id_;
		}

		static b2World* getWorld() {
			return world;
		}

		GameEntity* getGameEntity() const {
			return gameEntity_;
		}

	private:
		int id_;
		GameEntity* gameEntity_;
		static int lastId;
		static b2World* world;
	};

} // Namespace zombie.

#endif // OBJECT_H
