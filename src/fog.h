#ifndef FOG_H
#define FOG_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particleengine.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <gui/windowmatrix.h>

#include <array>

namespace zombie {

	struct FogProperties {
		FogProperties() {
		}

		FogProperties(float radius, float speed, float particleSize, mw::Texture particle) :
			radius_(radius),
			speed_(speed),
			particleSize_(particleSize),
			particle_(particle) {
		}

		float radius_;
		float speed_;
		float particleSize_;
		mw::Color color_;
		mw::Texture particle_;
	};

	class Fog {
	public:
		Fog(const FogProperties& fogProperties);

		void draw(float deltaTime);

		void init(Particle& particle);

		void update(float deltaTime, Particle& particle);

	private:
		ParticleEngine<1000, Fog> fog_;
		float radius_;
		float speed_;
		float particleSize_;
	};

	/*
	std::shared_ptr<Fog> loadFog(GameDataEntry& entry) {
	mw::Texture fog = entry.getTexture("image");
	float radius = entry.getFloat("radius");
	mw::Color color = entry.getColor("color");
	return std::make_shared<Fog>(fog, radius, color);
	}
	*/

} // Namespace zombie.

#endif // FOG_H
