#ifndef MISSILE_H
#define MISSILE_H

#include "object.h"
#include "state.h"
#include "gameinterface.h"
#include "box2ddef.h"

namespace zombie {

	class GameInterface;

	class Missile : public Object {
	public:
		Missile(GameInterface& gameInterface, float width, float length, float mass, float speed,
			float explodeTime, float damage, float explosionRadius);

		virtual ~Missile() {
		}

		void createBody(b2World* world, Position position, float angle);

		void update(float time, float timeStep);

		void collision(float impulse) override final;
		
		b2Body* getBody() const override final;

		float getWidth() const;

		float getLength() const;

		Position getPosition() const;

		float getDirection() const;

		State getState() const;

		State previousState() const;

		void destroyBody() override {
			if (body_ != nullptr) {
				b2World* world = body_->GetWorld();
				if (world != nullptr) {
					world->DestroyBody(body_);
				}
				body_ = nullptr;
			}
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
