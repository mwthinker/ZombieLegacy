#ifndef LINEFEATURE_H
#define LINEFEATURE_H


#include <memory>
#include <mw/mathvector.h>
#include "protocol.h"
#include <mw/packet.h>

namespace zombie {

typedef mw::MathVector Position;

class LineFeature {
public:
	LineFeature() {
		
	}
	LineFeature(Position p1, Position p2) {
		vertexes_.push_back(p1);
		vertexes_.push_back(p2);
	}

	LineFeature(std::vector<Position> list) {
		
	}

	void addVertex(Position p) {
		vertexes_.push_back(p);	
	}

	Position getStart() {
		return vertexes_[0];
	}

	Position getEnd() {
		return vertexes_[1];
	}

private:
	std::vector<Position> vertexes_;

};



} // namespace zombie.

#endif // LINEFEATURE_H