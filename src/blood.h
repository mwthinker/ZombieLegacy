#ifndef BLOOD_H
#define BLOOD_H

#include "box2ddef.h"
#include "graphic.h"

#include <mw/sprite.h>

namespace zombie {

	class Blood : public Graphic {
	public:
		Blood(Position position);

		void draw(float deltaTime) override;

		bool toBeRemoved() const override;

	private:
		float time_;
		Position position_;
		float duration_;

		mw::Sprite blood_;
	};

} // Namespace zombie.

#endif // BLOOD_H
