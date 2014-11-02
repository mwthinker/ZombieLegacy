#ifndef BUILDING_H
#define BUILDING_H

#include "object.h"

#include <limits>
#include <array>
#include <cmath>

namespace zombie {

	class Building : public Object {
	public:
		Building(Position p1, Position p2, Position p3, Position p4) : corners_({p1, p2, p3, p4}) {
			body_ = nullptr;
		}

		const std::array<Position, 4>& getCorners() const {
			return corners_;
		}

		b2Body* getBody() const override {
			return body_;
		}

	private:
		void createBody(b2World* world) override {
			Position center = 0.5f * (corners_[0] + corners_[2]);
			float w = (corners_[0] - corners_[1]).Length();
			float h = (corners_[2] - corners_[1]).Length();
			float angle = std::atan2(corners_[1].y - corners_[0].y, corners_[1].x - corners_[0].x);

			// Create body.
			{
				b2BodyDef bodyDef;
				bodyDef.fixedRotation = true;
				bodyDef.position = center;

				body_ = world->CreateBody(&bodyDef);
				body_->SetUserData(this);
			}

			// Create fixture to body.
			{
				b2PolygonShape box;
				box.SetAsBox(w*0.5f, h*0.5f, ZERO, angle); // Expected parameters is half the side.

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &box;
				fixtureDef.friction = 0.3f;
				b2Fixture* fixture = body_->CreateFixture(&fixtureDef);
				fixture->SetUserData(this);
			}
		}

		void destroyBody() override {
			if (body_ != nullptr) {
				b2World* world = body_->GetWorld();
				if (world != nullptr) {
					world->DestroyBody(body_);
				}
				body_ = nullptr;
			}
		}		

		std::array<Position, 4> corners_;
		float radius_;
		float length_, width_;

		b2Body* body_;
	};

} // Namespace zombie.

#endif // BUILDING_H
