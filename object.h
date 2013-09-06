#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>
#include <memory>
#include <unordered_map>

namespace zombie {

	class Object;
	typedef std::unordered_map<int, Object*> WorldHash;

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class ZombieGame;

		inline Object() {
			id_ = ++lastId;
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

		static const Object* getObject(int id) {
			unsigned int size = worldHash->size();
			
			Object*& object = (*worldHash)[id];

			if (worldHash->size() > size) {
				object = nullptr;
			}

			return object;
		}

	private:
		int id_;
		static int lastId;
		static b2World* world;
		static WorldHash* worldHash;
	};

} // Namespace zombie.

#endif // OBJECT_H
