#include "protocol.h"

#include "input.h"
#include "weapon.h"

#include <mw/packet.h>

namespace zombie {

mw::Packet& operator<<(mw::Packet& packet, const Weapon& weapon) {
	packet << weapon.clipSize_;
	packet << weapon.damage_;
	packet << weapon.timeBetweenShots_;
	packet << weapon.range_;
	packet << weapon.bulletsInWeapon_;
	packet << weapon.lastShotTime_;
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, Weapon& weapon) {
	packet >> weapon.clipSize_;
	packet >> weapon.damage_;
	packet >> weapon.timeBetweenShots_;
	packet >> weapon.range_;
	packet >> weapon.bulletsInWeapon_;
	packet >> weapon.lastShotTime_;	
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const Net& net) {
	packet << static_cast<char>(net);
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, Net& net) {
	char tmp;
	packet >> tmp;
	net = static_cast<Net>(tmp);
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const Input& input) {
	char data = input.forward_;
	data <<= 1;	
	data += input.backward_;
	data <<= 1;	
	data += input.turnLeft_;
	data <<= 1;
	data += input.turnRight_;
	data <<= 1;
	data += input.reload_;
	data <<= 1;
	data += input.shoot_;
	packet << data;	
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, Input& input) {
	char data;
	packet >> data;
	char bit = 1;
	input.shoot_ = (bit & data) > 0;
	bit <<= 1;
	input.reload_ = (bit & data) > 0;
	bit <<= 1;
	input.turnRight_ = (bit & data) > 0;
	bit <<= 1;
	input.turnLeft_ = (bit & data) > 0;
	bit <<= 1;
	input.backward_ = (bit & data) > 0;
	bit <<= 1;	
	input.forward_ = (bit & data) > 0;
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const mw::MathVector& v) {
	packet << v.x_;
	packet << v.y_;
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, mw::MathVector& v) {
	packet >> v.x_;
	packet >> v.y_;
	v.z_ = 0.0;
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, State& state) {
	packet >> state.angle_;
	packet >> state.angleVelocity_;
	packet >> state.position_;
	packet >> state.velocity_;
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const State& state) {
	packet << state.angle_;
	packet << state.angleVelocity_;
	packet << state.position_;
	packet << state.velocity_;
	return packet;
}

} // namespace zombie.