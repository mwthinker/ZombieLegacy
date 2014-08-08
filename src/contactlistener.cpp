#include "contactlistener.h"
#include "box2ddef.h"
#include "car.h"
#include "unit.h"
#include "building.h"

#include <mw/opengl.h>

namespace zombie {

	namespace {

		void collision(GameInterface& game, float impulse, Object* ob) {
			if (Car* car = dynamic_cast<Car*>(ob)) {
				game.collision(impulse, *car);
			} else if (Unit* unit = dynamic_cast<Unit*>(ob)) {
				game.collision(impulse, *unit);
			} else if (Building* building = dynamic_cast<Building*>(ob)) {
				game.collision(impulse, *building);
			}
		}

		bool getVisibleObject(b2Contact* contact, MovingObject*& target, MovingObject*& looker) {
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			// Make sure only one of the fixtures was a sensor.
			bool sensorA = fixtureA->IsSensor();
			bool sensorB = fixtureB->IsSensor();
			if (sensorA == sensorB) {
				return false;
			}

			Object* obA = static_cast<Object*>(fixtureA->GetBody()->GetUserData());
			Object* obB = static_cast<Object*>(fixtureB->GetBody()->GetUserData());
			MovingObject* mObA = dynamic_cast<MovingObject*>(obA);
			MovingObject* mObB = dynamic_cast<MovingObject*>(obB);

			// Make sure both are moving objects.
			if (mObA != nullptr && mObB != nullptr && mObA && mObB) {
				if (sensorA) {
					looker = mObA;
					target = mObB;
				} else {
					looker = mObB;
					target = mObA;
				}
				return true;
			}
			return false;
		}

	} // Anonymous namespace.

	ContactListener::ContactListener(GameInterface& gameInterface, float impulseThreshold)
		: gameInterface_(gameInterface), impulseThreshold_(impulseThreshold) {
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		Object* ob1 = static_cast<Object*>(contact->GetFixtureA()->GetUserData());
		Object* ob2 = static_cast<Object*>(contact->GetFixtureB()->GetUserData());
		float maxImpulse = 0;
		for (int32 i = 0; i < impulse->count; ++i) {
			maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
		}

		if (maxImpulse > impulseThreshold_) {
			collision(gameInterface_, maxImpulse, ob1);
			collision(gameInterface_, maxImpulse, ob2);
		}
	}

	void ContactListener::BeginContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->addSeenObject(object);
		}
	}

	void ContactListener::EndContact(b2Contact* contact) {
		MovingObject* object;
		MovingObject* looker;
		if (getVisibleObject(contact, object, looker)) {
			looker->removeSeenObject(object);
		}
	}

} // Namespace zombie.
