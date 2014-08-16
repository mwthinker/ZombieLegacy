#ifndef TREE_H
#define TREE_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "object.h"

#include <memory>

namespace zombie {

	class Tree : public Object {
	public:
		Tree(const Position& position) : position_(position) {
			body_ = nullptr;
			radius_ = random(0.5f, 2);
			angle_ = random(0, 2*PI);
		}

		void createBody(b2World* world) {
			// Box2d properties.
			b2BodyDef bodyDef;
			bodyDef.type = b2_staticBody;
			bodyDef.position = position_;
			bodyDef.angle = angle_;

			body_ = world->CreateBody(&bodyDef);
			body_->SetUserData(this);

			b2CircleShape circle;
			circle.m_p.Set(0, 0);
			circle.m_radius = radius_;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.friction = 0.0f;
			
			// Add Body fixture.
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}

		float getRadius() const {
			return radius_;
		}

		Position getPosition() const {
			return position_;
		}

		float getAngle() const {
			return angle_;
		}

		b2Body* getBody() const override {
			return body_;
		}

	private:
		Position position_;
		float radius_;
		float angle_;
		b2Body* body_;
	};

} // Namespace zombie.

#endif // WEAPON_H
