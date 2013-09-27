#include "object.h"

namespace zombie {

	int Object::lastId = 0;
	
	b2World* Object::world = nullptr;
	
	WorldHash* Object::worldHash = nullptr;

} // Namespace zombie.
