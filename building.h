#ifndef BUILDING_H
#define BUILDING_H

#include "object.h"
#include "typedefs.h"

#include <Box2D/Box2D.h>
#include <limits>

namespace zombie {

class Building : public Object {
public:
	Building(const std::vector<Position>& corners) : corners_(corners) {
		init();

		// Create body.
		{
			b2BodyDef bodyDef;
			bodyDef.fixedRotation = true;
			bodyDef.position.Set(position_.x,position_.y);
		
			body_ = getWorld()->CreateBody(&bodyDef);
			body_->SetUserData(this);
		}
		
		// Create fixture to body.
		{
			unsigned int size = corners.size();
		
			// Create polygon.
			b2Vec2 vertices[b2_maxPolygonVertices];		
			unsigned int count = 0;
			// Save global vertex points to local shape coordinates.
			for (; count < size && count < b2_maxPolygonVertices; ++count) {
				vertices[count] = body_->GetLocalPoint(corners[count]);
			}
		
			b2PolygonShape shape;
			shape.Set(vertices, count);

			b2Fixture* fixture = body_->CreateFixture(&shape,0.f);
			fixture->SetUserData(this);
		}
	}

	~Building() {
		getWorld()->DestroyBody(body_);
	}

	const std::vector<Position>& getCorners() const {
		return corners_;
	}

	bool isInside(float x, float y) const {
		return isPointInPolygon(x,y);
	}

	float getRadius() const {
		return radius_;
	}
	
	Position getPosition() const {
		return position_;
	}

	b2Body* getBody() const override {
		return body_;
	}

protected:
	void init() {
		body_ = nullptr;

		float xLeft = std::numeric_limits<float>::max();
		float xRight = -xLeft;
		float yUp = -xLeft;
		float yDown = xLeft;
		
		for (Position p : corners_) {
			xRight = std::max(xRight,p.x);
			yUp = std::max(yUp,p.y);
			xLeft = std::min(xLeft,p.x);
			yDown = std::min(yDown,p.y);
		}
		
		position_ = Position((xLeft + xRight)/2,(yDown + yUp)/2);
		radius_ = (position_ - Position(xLeft,yDown)).Length();
	}
	
private:
	bool isPointInPolygon(float x, float y) const {
		int polySides = corners_.size();
		int j = polySides-1;
		bool oddNodes = false;

		for (int i = 0; i < polySides; i++) {
			Position pi = corners_[i];
			Position pj = corners_[j];
			if ( (pi.y < y && pj.y >= y || pj.y < y && pi.y >= y) &&  (pi.x <= x || pj.x <= x) ) {
				if (pi.x +(y - pi.y)/(pj.y - pi.y)*(pj.x - pi.x) < x) {
					oddNodes =! oddNodes;
				}
			}
			j=i;
		}

		return oddNodes;
	}

	Position position_;
	float radius_;
	std::vector<Position> corners_;
	
	b2Body* body_;
};

} // namespace zombie.

#endif // BUILDING_H