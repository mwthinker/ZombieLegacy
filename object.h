#ifndef OBJECT_H
#define OBJECT_H

#include <Box2D/Dynamics/b2Body.h>
#include <memory>

namespace zombie {

	class Object;

	class InMemory {
	public:
		friend class Object;

		InMemory() {
		}
		
		bool isValid() const {
			return valid_ && *valid_;
		}

		void disconnect() {
			valid_ = nullptr;
		}
	private:
		InMemory(std::shared_ptr<bool> valid) {
			valid_ = valid;
		}
		
		std::shared_ptr<bool> valid_;
	};

	// Represent a object inside the "zombie world".
	class Object {
	public:
		friend class ZombieGame;

		Object() : inMemory_(std::make_shared<bool>(true)){
		}

		virtual ~Object() {
			*inMemory_.valid_ = false;
		}

		InMemory getInMemory() const {
			return inMemory_;
		}

		virtual b2Body* getBody() const = 0;

		static b2World* getWorld() {
			return world_;
		}
	private:
		static void setWorld(b2World* world) {
			world_ = world;
		}
		
		InMemory inMemory_;

		static b2World* world_;
	};

} // namespace zombie

#endif // OBJECT_H