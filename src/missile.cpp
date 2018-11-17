#include "missile.h"
#include "inviewquerycallback.h"
#include "unit.h"
#include "gameinterface.h"
#include "auxiliary.h"

namespace zombie {

	Missile::Missile() : body_(nullptr) {
	}

	Missile::Missile(GameInterface* gameInterface, float width, float length, float mass) : 
		gameInterface_(gameInterface), body_(nullptr),
		width_(width),
		length_(length),
		mass_(mass) {

	}

	void Missile::create(Position position, float angle, float speed, float explodeTime, float damage, float explosionRadius, float force) {
		speed_ = speed;
		damage_ = damage;
		explosionRadius_ = explosionRadius;
		explode_ = false;
		explodeTime_ = explodeTime;
		time_ = 0;
		force_ = force;
		body_->SetActive(true);
		body_->SetAwake(true);
		// Set the position and current angle.
		body_->SetTransform(position, angle);

		// Set the velocity of the states.
		body_->SetLinearVelocity(speed_ * directionVector(angle));
		body_->SetAngularVelocity(0);
	}

	void Missile::createBody(b2World* world) {
		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = ZERO;
		bodyDef.angle = 0;
		body_ = world->CreateBody(&bodyDef);
		body_->SetUserData(this);

		// Body properties.
		{
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(0.5f * length_, 0.5f * width_); // Expected parameters is half the side.

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = mass_ / (length_ * width_);
			fixtureDef.friction = 0.3f;
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}

		previousState_ = getState();
	}

	void Missile::collision(float impulse) {
		explode_ = true;
	}

	void Missile::updatePhysics(double time, double timeStep) {
		previousState_ = getState();
		time_ += (float) timeStep;

		if (explode_ || explodeTime_ < time_) {
			explode();
		}
	}

	b2Body* Missile::getBody() const {
		return body_;
	}

	float Missile::getWidth() const {
		return width_;
	}

	float Missile::getLength() const {
		return length_;
	}

	Position Missile::getPosition() const {
		return body_->GetPosition();
	}

	float Missile::getDirection() const {
		return body_->GetAngle();
	}

	State Missile::getState() const {
		return State(body_->GetPosition(),
			body_->GetLinearVelocity(),
			body_->GetAngle(),
			body_->GetAngularVelocity());
	}

	State Missile::previousState() const {
		return previousState_;
	}

	void Missile::explode() {
		InViewQueryCallback queryCallback;
		b2AABB aabb;
		float angle = body_->GetAngle();
		Position explosionPosition = body_->GetPosition() + 0.5f * length_ * Position(std::cos(angle), std::sin(angle));
		aabb.lowerBound = explosionPosition - explosionRadius_ *  Position(0.5f, 0.5f);
		aabb.upperBound = explosionPosition + explosionRadius_ *  Position(0.5f, 0.5f);
		body_->GetWorld()->QueryAABB(&queryCallback, aabb);

		for (b2Fixture* fixture : queryCallback.foundFixtures) {
			if (fixture->GetUserData() != nullptr) {
				Object* ob = static_cast<Object*>(fixture->GetUserData());
				if (auto unit = dynamic_cast<Unit*>(ob)) {
					unit->updateHealthPoint(-damage_);
					Position dir = unit->getPosition() - explosionPosition;
					dir.Normalize();

					// Apply some out going force to the object.
					unit->getBody()->ApplyForceToCenter(force_ * dir, true);
				}
			}
		}
		explode_ = false;
		gameInterface_->explosion(explosionPosition, explosionRadius_);
		body_->SetActive(false);
		body_->SetAwake(false);
	}

} // Namespace zombie.
