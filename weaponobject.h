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
				circle.m_radius = 2;

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

		b2Body* getBody() const {
			return body_;
		}

	private:
		Weapon weapon;
		b2Body* body_;
	};

} // namespace zombie.

#endif // WEAPONOBJECT_H