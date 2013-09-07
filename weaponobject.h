#ifndef WEAPONOBJECT_H
#define WEAPONOBJECT_H

#include "object.h"
#include "typedefs.h"
#include "weapon.h"

namespace zombie {

	class Unit;

	class WeaponObject : public Object {
	public:
		WeaponObject(float x, float y, const Weapon& weapon) {
			radius_ = 0.5;
			
			// Box2d properties.
			b2BodyDef bodyDef;
			bodyDef.position.Set(x, y);
			bodyDef.angle = 0.0;
			body_ = getWorld()->CreateBody(&bodyDef);
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

		~WeaponObject() {
			getWorld()->DestroyBody(body_);
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

		b2Body* getBody() const {
			return body_;
		}

	private:
		Weapon weapon;
		float radius_;
		b2Body* body_;
	};

} // Namespace zombie.

#endif // WEAPONOBJECT_H
