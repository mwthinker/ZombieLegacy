#include "object.h"

namespace zombie {

	void Object::destroyBody() {
		b2Body* body = getBody();
		if (body != nullptr) {
			b2World* world = body->GetWorld();
			if (world != nullptr) {
				world->DestroyBody(body);
			}
		}
	}

} // Namespace zombie.
