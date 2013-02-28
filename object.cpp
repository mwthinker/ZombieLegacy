#include "object.h"

namespace zombie {

int Object::lastAddedId_ = 0;

// Represent a object living inside the "zombie world".
Object::Object(int id) {
	id_ = id;
}

Object::~Object() {}

int Object::id() const {
	return id_;
}

} // namespace zombie