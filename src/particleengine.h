#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particle.h"
#include "gameshader.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>

#include <array>

namespace zombie {

	template <int MAX_PARTICLES, class Emitter>
	class ParticleEngine {
	public:
		ParticleEngine(Emitter& emitter, const mw::Texture& texture);

		void setLoop(bool loop) {
			loop_ = loop;
		}

		void draw(float deltaTime, const GameShader& shader);

		void restart(const mw::Texture& texture) {
			texture_ = texture;
			time_ = 0;
			for (auto& particle : particles_) {
				emitter_.init(particle);
			}
		}

		void setColor(const mw::Color& color) {
			color_ = color;
		}

		void setBlend(bool addColor) {
			addColor_ = addColor;
		}

		double getTime() const {
			return time_;
		}

	private:
		void setAttributes(const Particle& particle, int index, GLfloat aPos[], GLfloat aTex[]);
		
		std::array<Particle, MAX_PARTICLES> particles_;
		Emitter& emitter_;
		mw::Texture texture_;
		mw::Color color_;

		mw::Matrix44 uColor_;

		double time_;
		bool loop_;
		bool addColor_;
	};

	template <int MAX_PARTICLES, class Emitter>
	ParticleEngine<MAX_PARTICLES, Emitter>::ParticleEngine(Emitter& emitter, const mw::Texture& texture) : emitter_(emitter) {
		loop_ = true;
		addColor_ = true;

		restart(texture);
	}

	template <int MAX_PARTICLES, class Emitter>
	void ParticleEngine<MAX_PARTICLES, Emitter>::draw(float deltaTime, const GameShader& shader) {
		time_ += deltaTime;

		GLfloat aPos[MAX_PARTICLES * 6 * 2]; // 2 triangle per particle. I.e. (3 * 2) * 2 = 12.
		GLfloat aTex[MAX_PARTICLES * 6 * 2]; // 2 per vertex.

		int index = 0;
		for (auto& particle : particles_) {
			// Is the particle alive?
			if (particle.alive) {
				// Update the position and draw it.
				setAttributes(particle, index, aPos, aTex);
				++index;
				emitter_.update(deltaTime, particle);
			} else {
				if (loop_) {
					// Replace the particle.
					emitter_.init(particle);
				}
			}
		}
		// Enable 2D textures and point sprites
		mw::glEnable(GL_BLEND);

		// Add colors.
		if (addColor_) {
			mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		} else {
			mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
				
		texture_.bindTexture();
		shader.setGlLocalAngleU(0);
		shader.setGlColorU(color_);
		shader.setGlTextureU(true);
		shader.setGlVer2dCoordsA(aPos);
		shader.setGlTexCoordsA(aTex);

		mw::glDrawArrays(GL_TRIANGLES, 0, index * 6);
		mw::glDisable(GL_BLEND);
	}

	template <int MAX_PARTICLES, class Emitter>
	void ParticleEngine<MAX_PARTICLES, Emitter>::setAttributes(const Particle& particle, int index, GLfloat aPos[], GLfloat aTex[]) {
		// 1:st triangle.
		aPos[12 * index + 0] = particle.pos_.x - particle.particleSize_*0.5f; aPos[12 * index + 1] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 0] = 0; aTex[12 * index + 1] = 0;

		aPos[12 * index + 2] = particle.pos_.x + particle.particleSize_*0.5f; aPos[12 * index + 3] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 2] = 1; aTex[12 * index + 3] = 0;

		aPos[12 * index + 4] = particle.pos_.x - particle.particleSize_*0.5f; aPos[12 * index + 5] = particle.pos_.y + particle.particleSize_*0.5f;
		aTex[12 * index + 4] = 0; aTex[12 * index + 5] = 1;

		// 2:nd triangle.
		aPos[12 * index + 6] = particle.pos_.x - particle.particleSize_*0.5f; aPos[12 * index + 7] = particle.pos_.y + particle.particleSize_*0.5f;
		aTex[12 * index + 6] = 0; aTex[12 * index + 7] = 1;

		aPos[12 * index + 8] = particle.pos_.x + particle.particleSize_*0.5f; aPos[12 * index + 9] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 8] = 1; aTex[12 * index + 9] = 0;

		aPos[12 * index + 10] = particle.pos_.x + particle.particleSize_*0.5f; aPos[12 * index + 11] = particle.pos_.y + particle.particleSize_*0.5f;
		aTex[12 * index + 10] = 1; aTex[12 * index + 11] = 1;
	}

} // Namespace zombie.

#endif // PARTICLEENGINE_H
