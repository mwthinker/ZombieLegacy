#ifndef BUILDING_H
#define BUILDING_H

#include "object.h"
#include "physicalengine.h"

#include <memory>
#include <mw/mathvector.h>
#include "protocol.h"
#include <mw/packet.h>

namespace zombie {

typedef mw::MathVector Position;

class Building : public Object, public StaticPhyscalUnit {
public:
	Building(int id, double x, double y, double width, double height) : Object(id) {
		Position position = Position(x,y);

		corners_.push_back(position);
		corners_.push_back(position + Position(width,0));
		corners_.push_back(position + Position(width,height));
		corners_.push_back(position + Position(0,height));
		init();
	}

	Building(const std::vector<Position>& corners, int id) : Object(id), corners_(corners) {
		init();
	}

	Building(mw::Packet& packet) : Object(packet) {
		Position position;
		while (packet.dataLeftToRead() > 0) {			
			packet >> position;
			if (position.x_ > 0.0) {
				corners_.push_back(position);
			} else {
				break;
			}
		}
		init();
	}

	mw::Packet generatePacket() const {
		mw::Packet packet;
		packet << Object::generatePacket();
		for (const Position& position : corners_) {
			packet << position;
		}
		packet << Position(-1,-1);
		return packet;
	}

	void updateStates(double timestep) {

	}

	const std::vector<Position>& getCorners() const {
		return corners_;
	}

	virtual ~Building() {
	}

	bool isInside(double x, double y) const {
		return isPointInPolygon(x,y);
	}

	// Override member from class StaticPhysicalUnit
	bool isInsideApproximate(double x, double y, double radius) const {
		return (longestSide_ + radius)*(longestSide_ + radius) > (x - massCentre_.x_)*(y - massCentre_.y_);
	}
	
	// Override member from class StaticPhysicalUnit
	double stiffness() const {
		return 100.0;
	}
	
	// Override member from class StaticPhysicalUnit
	Position penetration(double x, double y, double radius) const {
		int size = corners_.size();
		Position distance(1000,1000);
		for (int i = 0; i < size; ++i) {
			// Relative the corner in index i
			Position line = corners_[(i+1) % size] - corners_[i];
			Position point = Position(x,y) - corners_[i];

			double length = point * line.normalize(); // Costly operation
			Position proj = length * line.normalize(); // Costly operation
			Position tmp;
			if (length < 0.0) {
				tmp = point;
			} else if (length*length > line.magnitudeSquared()) {
				tmp = point - line;
			} else {
				tmp = point - proj;
			}
			if (tmp.magnitudeSquared() < distance.magnitudeSquared()) {
				distance = tmp;
			}
		}
		
		// Calculates the penetration depth.
		// If penetration
		if (distance.magnitudeSquared() < radius*radius) {
			distance = distance - distance.normalize() * radius;
		} else {
			distance = Position(0,0);
		}		

		return distance;
	}

	/*
	bool popGameEvent(GameEvent*& gameEvent) {
		return false;
	}
	*/

	double healthPoints() const {
		return 100.0;
	}
	
	void updateHealthPoint(double deltaLife) {
	}

	bool isDead() const {
		return false;
	}

protected:
	void init() {
		massCentre_ = calculateMassCenter();

		double longestDiff = 0;
		for (Position p : corners_) {			
			double tmp = (massCentre_ - p).magnitude();
			if (tmp > longestDiff) {
				longestDiff = tmp;
			}
		}
		longestSide_ = longestDiff;
	}

	Position calculateMassCenter() const {
		Position centre_;
		for (Position p : corners_) {
			centre_ += p;
		}
		return centre_;
	}

	std::vector<Position> corners_;	
	double longestSide_;
	Position massCentre_;

private:
	bool isPointInPolygon(double x, double y) const {
		int polySides = corners_.size();
		int j = polySides-1;
		bool oddNodes = false;

		for (int i = 0; i < polySides; i++) {
			Position pi = corners_[i];
			Position pj = corners_[j];
			if ( (pi.y_< y && pj.y_ >= y || pj.y_ < y && pi.y_ >= y) &&  (pi.x_ <= x || pj.x_ <= x) ) {
				if (pi.x_+(y - pi.y_)/(pj.y_ - pi.y_)*(pj.x_ - pi.x_) < x) {
					oddNodes =! oddNodes;
				}
			}
			j=i;
		}

		return oddNodes;
	}
};

typedef std::shared_ptr<Building> BuildingPtr;

} // namespace zombie.

#endif // BUILDING_H