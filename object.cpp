#include "object.h"

namespace zombie {

int Object::lastAddedId_ = 0;

Object::Object(mw::Packet& packet) {	
	packet >> id_;
}

// Represent a object living inside the "zombie world".
Object::Object(int id) {
	id_ = id;
}

Object::~Object() {}

mw::Packet Object::generatePacket() const {
	mw::Packet packet;
	packet << id_;
	return packet;
}

int Object::id() const {
	return id_;
}

} // namespace zombie