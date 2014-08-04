#ifndef FOG_H
#define FOG_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particleengine.h"
#include "graphic.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <array>

namespace zombie {

	class Fog : public Graphic {
	public:
		Fog(const mw::Texture& fog, float radius, const mw::Color& color);

		void draw(float deltaTime);

		inline bool toBeRemoved() const {
			return false;
		}

		void update(Position center);

	private:
		ParticleEngine<1000> fog_;
		float radius_;
		Position center_;
		mw::Color color_;
	};

} // Namespace zombie.

#endif // FOG_H
