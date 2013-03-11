#ifndef BUILDING_H
#define BUILDING_H

#include "object.h"
#include "typedefs.h"

#include <Box2D/Box2D.h>
#include <limits>

namespace zombie {

class Building : public Object {
public:
	Building(double x, double y, double width, double height) {
		Position position = Position(x,y);

		corners_.push_back(position);
		corners_.push_back(position + Position(width,0));
		corners_.push_back(position + Position(width,height));
		corners_.push_back(position + Position(0,height));
		init();
	}

	Building(b2World* world, const std::vector<Position>& corners) : corners_(corners) {
		int count = 0;
		b2Vec2 vertices[b2_maxPolygonVertices];
		for (; count < corners.size() && count < b2_maxPolygonVertices; ++count) {
			vertices[count] = b2Vec2(corners[count].x_,corners[count].y_);
		}
		b2PolygonShape polygon;
		polygon.Set(vertices, count);

		b2BodyDef bodyDef;
		bodyDef.userData = this;

		//b2Body* groundBody = world->CreateBody(&bodyDef);
		//groundBody->CreateFixture(&polygon,0.f);
		
		init();
	}

	Building(const std::vector<Position>& corners) : corners_(corners) {
		init();
	}

	const std::vector<Position>& getCorners() const {
		return corners_;
	}

	bool isInside(double x, double y) const {
		return isPointInPolygon(x,y);
	}

	double getRadius() const {
		return radius_;
	}
	
	Position getPosition() const {
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
	
	b2Body* body_;
};

} // namespace zombie.

#endif // BUILDING_H