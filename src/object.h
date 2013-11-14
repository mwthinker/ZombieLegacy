#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>

namespace zombie {

	class Player;

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class Player;

		inline Object(b2World* world) : id_(++lastId), player_(nullptr), world_(world) {
		}

		virtual ~Object() {
		}

		virtual b2Body* getBody() const = 0;

		inline int getId() const {
			return id_;
		}

		inline b2World* getWorld() {
			return world_;
		}

		Player* getPlayer() const {
			return player_;
		}

	private:
		int id_;
		Player* player_;
		b2World* world_;
		static int lastId;
	};

} // Namespace zombie.

#endif // OBJECT_H
