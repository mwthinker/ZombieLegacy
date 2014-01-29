#ifndef WEAPONITEM_H
#define WEAPONITEM_H

#include "object.h"
#include "weapon.h"

namespace zombie {

	class Unit;

	class WeaponItem : public Object {
	public:
		WeaponItem(float x, float y, const Weapon& weapon) {
			radius_ = 0.5;
		}		

		void createBody(b2World* world) {
			// Box2d properties.
			b2BodyDef bodyDef;
			bodyDef.position.Set(x_, y_);
			bodyDef.angle = 0.0;
			body_ = world->CreateBody(&bodyDef);
			body_->SetUserData(this);

			// Add tensor. Should not be a physical object.
			{
				b2CircleShape circle;
				circle.m_p.Set(0, 0);
				circle.m_radius = radius_;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circle;
				fixtureDef.density = 0.0f;
				fixtureDef.friction = 0.0f;
				fixtureDef.isSensor = true;

				// Add Body fixture.
				b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
				fixture->SetUserData(this);
			}
		}

		Weapon getWeapon() const  {
			return weapon;
		}

		Position getPosition() const {
			return body_->GetPosition();
		}

		float getRadius() const {
			return radius_;
		}

		b2Body* getBody() const override {
			return body_;
		}

		void destroyBody(b2World* world) override {
			world->DestroyBody(body_);
			body_ = nullptr;
		}

		bool isDestroyed() const override {
			return body_ == nullptr;
		}

	private:
		Weapon weapon;
		float radius_;
		float x_, y_;


		b2Body* body_;
	};

} // Namespace zombie.

#endif // WEAPONITEM_H
