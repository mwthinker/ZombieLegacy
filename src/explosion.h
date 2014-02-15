#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "object.h"
#include "box2ddef.h"

#include <cassert>

namespace zombie {

	class Explosion : Object {
	public:
		Explosion(float damage) {
			damage_ = damage;
			body_ = nullptr;
		}

		void createBody(b2World* world, Position position, float angle) {
			// Create a small circle.
			assert(body_ == nullptr); // Must be a nullptr.
			
			b2Vec2 rayDir(sinf(angle), cosf(angle));

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.fixedRotation = true;
			bd.bullet = true; // Prevent tunneling at high speed
			bd.linearDamping = 10; // Air drag.
			bd.gravityScale = 0;
			bd.position = position;
			bd.linearVelocity = damage_ * rayDir;
			body_ = world->CreateBody(&bd);

			b2CircleShape circleShape;
			circleShape.m_radius = 0.01; // very small

			b2FixtureDef fd;
			fd.shape = &circleShape;
			fd.density = 1;
			fd.friction = 0;
			fd.restitution = 0.99f; // High restitution to reflect off obstacles.
			fd.filter.groupIndex = -1; // Particles should not collide with each other.
			body_->CreateFixture(&fd);
			body_->SetUserData(this);
		}

		void destroyBody(b2World* world) override {
			world->DestroyBody(body_);
			body_ = nullptr;
		}

		bool isDestroyed() const override {
			return body_ == nullptr;
		}

		void draw(float time) override {
			/*
			const WeaponObject* wOb = static_cast<const WeaponObject*>(ob);
			Position p = wOb->getPosition();
			glColor3d(0,0,1);
			drawCircle(p.x,p.y,wOb->getRadius(),6,true);
			return true;
			*/
		}

	private:
		float damage_;

		b2Body* body_;
	};

} // Namespace zombie.

#endif // EXPLOSION_H
