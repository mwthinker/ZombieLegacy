#ifndef BUILDING_H
#define BUILDING_H

#include "object.h"
#include "typedefs.h"
#include "physicalengine.h"

#include <limits>

namespace zombie {

class Border : public StaticPhyscalUnit {
public:
	Border(Position centre, double radius) {
		centre_ = centre;
		radius_ = radius;
	}

	// Override member from class StaticPhysicalUnit
	bool isInsideApproximate(double x, double y, double radius) const {
		Position p = centre_;
		return (x - p.x_)*(x - p.x_) + (y - p.y_)*(y - p.y_) < (getRadius() - radius)*(getRadius() - radius);
	}
	
	// Override member from class StaticPhysicalUnit
	double stiffness() const override {
		return 150.0;
	}
	
	// Override member from class StaticPhysicalUnit
	Position penetration(double x, double y, double radius) const override {
		return centre_.normalize()*radius_ - Position(x,y);
	}

	double getRadius() const override {
		return radius_;
	}

	Position getPosition() const override {
		return centre_;
	}
private:
	Position centre_;
	double radius_;
};

typedef std::shared_ptr<StaticPhyscalUnit> StaticPhUnitPtr;

class Building : public Object, public StaticPhyscalUnit {
public:
	Building(double x, double y, double width, double height) {
		Position position = Position(x,y);

		corners_.push_back(position);
		corners_.push_back(position + Position(width,0));
		corners_.push_back(position + Position(width,height));
		corners_.push_back(position + Position(0,height));
		init();
	}

	Building(const std::vector<Position>& corners) : corners_(corners) {
		init();
	}

	const std::vector<Position>& getCorners() const {
		return corners_;
	}

	bool isInside(double x, double y) const override {
		return isPointInPolygon(x,y);
	}

	// Override member from class StaticPhysicalUnit
	bool isInsideApproximate(double x, double y, double radius) const override {
		return (radius_ + radius)*(radius_ + radius) > (Position(x,y) - position_)*(Position(x,y) - position_);
	}
	
	// Override member from class StaticPhysicalUnit
	double stiffness() const override {
		return 150.0;
	}
	
	// Override member from class StaticPhysicalUnit
	Position penetration(double x, double y, double radius) const override {
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

	double getRadius() const override {
		return radius_;
	}
	
	Position getPosition() const override {
		return position_;
	}

protected:
	void init() {
		double xLeft = std::numeric_limits<double>::max();
		double xRight = -xLeft;
		double yUp = -xLeft;
		double yDown = xLeft;
		
		for (Position p : corners_) {
			xRight = std::max(xRight,p.x_);
			yUp = std::max(yUp,p.y_);
			xLeft = std::min(xLeft,p.x_);
			yDown = std::min(yDown,p.y_);
		}
		
		position_ = Position((xLeft + xRight)/2,(yDown + yUp)/2);
		radius_ = (position_ - Position(xLeft,yDown)).magnitude();
	}
	
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

	Position position_;
	double radius_;
	std::vector<Position> corners_;
};

} // namespace zombie.

#endif // BUILDING_H