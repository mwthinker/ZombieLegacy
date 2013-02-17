#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "input.h"
#include "state.h"
#include "weapon.h"

#include <mw/packet.h>

namespace zombie {

enum Net {NET_ID,
	NET_START,
	NET_UPDATE,
	NET_INPUT,
	NET_SHOOT,
	NET_RELOAD,
	NET_NEW_UNIT,
	NET_

};

mw::Packet& operator<<(mw::Packet& packet, const Weapon& weapon);
mw::Packet& operator>>(mw::Packet& packet, Weapon& weapon);

mw::Packet& operator<<(mw::Packet& packet, const Net& net);
mw::Packet& operator>>(mw::Packet& packet, Net& net);

mw::Packet& operator<<(mw::Packet& packet, const Input& input);
mw::Packet& operator>>(mw::Packet& packet, Input& input);

mw::Packet& operator<<(mw::Packet& packet, const mw::MathVector& v);
mw::Packet& operator>>(mw::Packet& packet, mw::MathVector& v);

mw::Packet& operator>>(mw::Packet& packet, State& state);
mw::Packet& operator<<(mw::Packet& packet, const State& state);

} // namespace zombie.

#endif // PROTOCOL_H