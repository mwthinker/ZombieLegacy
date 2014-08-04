#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

#include "box2ddef.h"
#include "auxiliary.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <memory>
#include <array>

namespace zombie {

	template <int MAX_PARTICLES>
	class ParticleEngine {
	public:
		ParticleEngine(const mw::Texture& texture, float minSpawnRadius, float maxSpawnRadius, float minSpeed, float maxSpeed, float minTimeLife, float maxTimeLife, float minSize, float maxSize) {
			texture_ = texture;
			minSpawnRadius_ = minSpawnRadius;
			maxSpawnRadius_ = maxSpawnRadius;
			minSpeed_ = minSpeed;
			maxSpeed_ = maxSpeed;
			minTimeLife_ = minTimeLife;
			maxTimeLife_ = maxTimeLife;
			minSize_ = minSize;
			maxSize_ = maxSize;
			restart();
			loop_ = true;
			addColor_ = true;
		}

		void setLoop(bool loop) {
			loop_ = loop;
		}

		void draw(float deltaTime) {
			time_ += deltaTime;

			// Enable 2D textures and point sprites
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);

			// Add colors.
			if (addColor_) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			} else {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			texture_.bind();

			for (auto& particle : particles_) {
				// Is the particle alive?			
				if (particle.timeLeftAlive_ > 0) {
					// Update the position and draw it.
					particle.update(deltaTime);
					color_.alpha_ = particle.timeLeftAlive_ / minTimeLife_;
					color_.glColor4f();
					draw(particle);
				} else {
					if (loop_) {
						// Replace the particle.
						particle = Particle(minSpawnRadius_, maxSpawnRadius_, minSpeed_, maxSpeed_, minTimeLife_, maxTimeLife_, minSize_, maxSize_);
					}
				}
			}
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}

		bool isFinnish() const {
			return time_ > maxTimeLife_;
		}

		void restart() {
			time_ = 0;
			for (auto& particle : particles_) {
				particle = Particle(minSpawnRadius_, maxSpawnRadius_, minSpeed_, maxSpeed_, minTimeLife_, maxTimeLife_, minSize_, maxSize_);
			}
		}

		void setColor(float red, float green, float blue) {
			color_.red_ = red;
			color_.green_ = green;
			color_.blue_ = blue;
		}

		void setBlend(bool addColor) {
			addColor_ = addColor;
		}

	private:
		class Particle {
		public:
			Particle() {
			}

			Particle(float minSpawnRadius, float maxSpawnRadius, float minSpeed, float maxSpeed, float minTimeLife, float maxTimeLife, float minSize, float maxSize) {
				timeLeftAlive_ = random(minTimeLife, maxTimeLife);
				float angle = random(0, 2 * PI);
				position_ = random(minSpawnRadius, maxSpawnRadius) * Position(std::cos(angle), std::sin(angle));
				angle = random(0, 2 * PI);
				velocity_ = random(minSpeed, maxSpeed) * Velocity(std::cos(angle), std::sin(angle));
				angle_ = angle;
				particleSize_ = random(minSize, maxSize);
			}

			void update(float deltaTime) {
				timeLeftAlive_ -= deltaTime;

				// Update the position.
				position_ += deltaTime * velocity_;
			}

			Position position_;
			Velocity velocity_;
			float timeLeftAlive_;
			float particleSize_;
			float angle_;
		};

		void draw(const Particle& p) {
			glPushMatrix();
			glRotatef(p.angle_ * 180 / PI);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(p.position_.x - p.particleSize_*0.5f, p.position_.y - p.particleSize_*0.5f);
			glTexCoord2f(1, 0);
			glVertex2f(p.position_.x + p.particleSize_*0.5f, p.position_.y - p.particleSize_*0.5f);
			glTexCoord2f(1, 1);
			glVertex2f(p.position_.x + p.particleSize_*0.5f, p.position_.y + p.particleSize_*0.5f);
			glTexCoord2f(0, 1);
			glVertex2f(p.position_.x - p.particleSize_*0.5f, p.position_.y + p.particleSize_*0.5f);
			glEnd();
			glPopMatrix();
		}

		std::array<Particle, MAX_PARTICLES> particles_;
		mw::Texture texture_;
		mw::Color color_;
		float time_;
		float minSpawnRadius_, maxSpawnRadius_, minSpeed_, maxSpeed_, minTimeLife_, maxTimeLife_, minSize_, maxSize_;
		bool loop_;
		bool addColor_;
	};

} // Namespace zombie.

#endif // PARTICLEENGINE_H
