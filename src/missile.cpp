#include "missile.h"
#include "inviewquerycallback.h"
#include "unit.h"

namespace zombie {

	Missile::Missile(GameInterface& gameInterface, float width, float length, float speed,
		float explodeTime, float damage, float explosionRadius) : gameInterface_(gameInterface) {
		
		speed_ = speed;
		damage_ = damage;
		explosionRadius_ = explosionRadius;
		width_ = width;
		length_ = length;
		exploded_ = false;
		explodeTime_ = explodeTime;
		time_ = 0;
	}

	void Missile::createBody(b2World* world, Position position, float angle) {
		// Box2d properties.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = position;
		bodyDef.angle = angle;
		body_ = world->CreateBody(&bodyDef);
		body_->SetLinearVelocity(speed_ * Velocity(std::cos(angle), std::sin(angle)));
		body_->SetUserData(this);

		// Body properties.
		{
			b2PolygonShape dynamicBox;
			dynamicBox.SetAsBox(0.5f *length_, 0.5f * width_); // Expected parameters is half the side.

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &dynamicBox;
			fixtureDef.density = mass_ / (length_ * width_);
			fixtureDef.friction = 0.3f;
			b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
			fixture->SetUserData(this);
		}
	}

	void Missile::collision(float impulse) {
		explode();
	}

	void Missile::update(float time, float timeStep) {
		previousState_ = getState();
		time_ += timeStep;

		if (!exploded_ && explodeTime_ < time_) {
			explode();
		}
	}

	void Missile::destroyBody(b2World* world) {
		world->DestroyBody(body_);
		body_ = nullptr;
	}

	bool Missile::toBeRemoved() const {
		return exploded_;
	}

	void Missile::explode() {
		InViewQueryCallback queryCallback;
		b2AABB aabb;
		Position position = body_->GetPosition();
		aabb.lowerBound = position + explosionRadius_ *  Position(0.5f, 0.5f);
		aabb.upperBound = position - explosionRadius_ *  Position(0.5f, 0.5f);
		body_->GetWorld()->QueryAABB(&queryCallback, aabb);

		for (b2Fixture* fixture : queryCallback.foundFixtures) {
			if (fixture->GetUserData() != nullptr) {
				Object* ob = static_cast<Object*>(fixture->GetUserData());
				if (auto unit = dynamic_cast<Unit*>(ob)) {
					unit->updateHealthPoint(-damage_);
					Position dir = unit->getPosition() - position;
					dir.Normalize();

					// Apply some out going force to the object.
					unit->getBody()->ApplyForceToCenter(force_ * dir);
				}
			}
		}
		exploded_ = true;
		gameInterface_.explosion(position, explosionRadius_);
	}

} // Namespace zombie.
