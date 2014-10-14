#include "fog.h"
#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <gui/windowmatrix.h>

namespace zombie {

	namespace {

		void drawInversCircel(float radius) {
			const int num_segments = 10;

			float theta = 2 * PI / 30;
			float c = std::cos(theta);
			float s = std::sin(theta);

			float x = radius;// Start at angle = 0
			float y = 0;

			GLfloat triangleStrip[(num_segments + 1) * 4];

			for (int i = 0; i < num_segments + 1; i++) {
				triangleStrip[i * 4 + 0] = x;
				triangleStrip[i * 4 + 1] = y;
				triangleStrip[i * 4 + 2] = x * 10;
				triangleStrip[i * 4 + 3] = y * 10;

				//apply the rotation matrix
				float t = x;
				x = c * x - s * y;
				y = s * t + c * y;
			}

			//mPtr->setTexture(false);
			//mPtr->setVertexPosition(2, triangleStrip);
			//mPtr->glDrawArrays(GL_TRIANGLE_STRIP, 0, (num_segments + 1) * 4);
		}

	} // Anonymous namespace.

	Fog::Fog(const FogProperties& fogProperties) : fog_(*this, fogProperties.particle_) {
		radius_ = fogProperties.radius_;

		fog_.setBlend(false);
		fog_.setLoop(true);
		speed_ = fogProperties.speed_;
		particleSize_ = fogProperties.particleSize_;
	}

	void Fog::draw(float deltaTime) {
		//mw::Matrix44 m = mPtr->getProjection() * mPtr->getModel();
		/*
		fog_.draw(deltaTime, m);
		m = m * mw::getRotateMatrix44(3 * PI / 2, 0, 0, 1);
		fog_.draw(deltaTime, m);
		m = m * mw::getRotateMatrix44(3 * PI / 2, 0, 0, 1);
		fog_.draw(deltaTime, m);
		drawInversCircel(radius_, mPtr);
		*/
	}

	void Fog::init(Particle& particle) {
		particle.pos_ = ZERO;
		float phi = random(0, 2 * PI);
		particle.vel_ = speed_ * Position(std::sin(phi), std::cos(phi));
		particle.particleSize_ = particleSize_;
		particle.alive = true;
		particle.lifeTime_ = 0;
		particle.angle_ = random(0, 2 * PI);
	}

	void Fog::update(float deltaTime, Particle& particle) {

	}

} // Namespace zombie.
