#ifndef MISSILE_H
#define MISSILE_H

#include "object.h"
#include "state.h"
#include "gameinterface.h"

#include <mw/opengl.h>

namespace zombie {

	class Missile;
	typedef std::shared_ptr<Missile> MissilePtr;

	class Missile : public Object {
	public:
		Missile(GameInterface& gameInterface, float width, float length, float mass, float speed,
			float explodeTime, float damage, float explosionRadius);

		virtual ~Missile() {
		}

		void createBody(b2World* world, Position position, float angle);

		void update(float time, float timeStep) override final;

		void collision(float impulse) override final;

		bool toBeRemoved() const final;

		void destroyBody(b2World* world) override final;
		
		b2Body* getBody() const override {
			return body_;
		}

		float getWidth() const {
			return width_;
		}

		float getLength() const {
			return length_;
		}

		Position getPosition() const {
			return body_->GetPosition();
		}

		float getDirection() const {
			return body_->GetAngle();
		}

		State getState() const {
			State state;
			state.angle_ = body_->GetAngle();
			state.position_ = body_->GetPosition();
			state.velocity_ = body_->GetLinearVelocity();
			state.anglularVelocity_ = body_->GetAngularVelocity();
			return state;
		}

		inline State previousState() const {
			return previousState_;
		}

	private:
		void explode();

		GameInterface& gameInterface_;
		bool exploded_;
		
		float damage_;
		float explosionRadius_;
		float force_;
		float time_;

		b2Body* body_;
		float steeringAngle_;

		float length_, width_;
		float mass_;
		float explodeTime_;
		float speed_;

		State previousState_;
	};

} // Namespace zombie.

#endif // MISSILE_H
