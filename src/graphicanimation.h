#ifndef GRAPHICANIMATION_H
#define GRAPHICANIMATION_H

#include "box2ddef.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/opengl.h>

#include <cassert>

namespace zombie {

	class GraphicAnimation {
	public:
		GraphicAnimation() {
			animation_.setLooping(false);
		}

		GraphicAnimation(Position position, float angle, const Animation& animation) : animation_(animation) {
			position_ = position;
			angle_ = angle;
			animation_.setLooping(false);
		}

		inline void restart(Position position, float angle, const Animation& animation) {
			position_ = position;
			angle_ = angle;
			animation_ = animation;
			animation_.setLooping(false);
			assert(!animation_.isLooping()); // May cause a infinity loop in zombieGame.
		}

		void draw(float deltaTime, const GameShader& shader) {
			shader.useGlShader();
			shader.setGlColorU(1, 1, 1);
			shader.setGlGlobalPositionU(position_);
			shader.setGlLocalAngleU(angle_);
			animation_.draw(deltaTime, 0, 0, 1, 1, shader);
		}

		bool toBeRemoved() const {
			return animation_.isEnded();
		}
	
	private:
		Animation animation_;
		Position position_;
		float angle_;
	};

} // Namespace zombie.

#endif // GRAPHICANIMATION_H
