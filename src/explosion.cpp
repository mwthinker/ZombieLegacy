#include "explosion.h"
#include "box2ddef.h"
#include "auxiliary.h"
#include "graphic.h"
#include "gamedataentry.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/sound.h>

namespace zombie {

	namespace {

		mw::Color interpolate(float ratio, const mw::Color& c1, const mw::Color& c2) {
			if (ratio < 0) {
				return c1;
			} else if (ratio > 1) {
				return c2;
			} else {
				mw::Color c;
				c.red_ = c1.red_ * (1 - ratio) + c2.red_ * ratio;
				c.green_ = c1.green_ * (1 - ratio) + c2.green_ * ratio;
				c.blue_ = c1.blue_ * (1 - ratio) + c2.blue_ * ratio;
				c.alpha_ = c1.alpha_ * (1 - ratio) + c2.alpha_ * ratio;
				return c;
			}
		}

	} // Anonymous namespace.

	Explosion::Explosion(Position position, const ExplosionProperties& exPr) :
		position_(position),
		time_(0),
		delay_(exPr.delay_),
		sound_(exPr.sound_),
		speed_(exPr.speed_),
		blastRadius_(exPr.blastRadius_),
		lifeTime_(exPr.blastRadius_ / exPr.speed_),
		particleSize_(exPr.blastRadius_ * 0.3f),
		particleEngine_(*this, exPr.particle_) {

		particleEngine_.setLoop(false);
		particleEngine_.setBlend(true);
		particleEngine_.setColor(mw::Color(1, 1, 1, 1));
	}

	void Explosion::draw(float deltaTime, const GameShader& shader) {
		if (time_ < deltaTime) {
			// Only called one time!
			sound_.play();
		}
		time_ += deltaTime;

		if (time_ > delay_) {
			//particleEngine_.draw(deltaTime, wPtr->getProjection() * wPtr->getModel());
		}
	}

	float Explosion::getTimeDuration() const {
		return lifeTime_ + 1;
	}

	bool Explosion::toBeRemoved() const {
		return time_ > getTimeDuration();
	}

	void Explosion::restart() {
		particleEngine_.restart();
		time_ = 0;
	}

	void Explosion::init(Particle& particle) {
		particle.pos_ = position_;
		float theta = random(0, PI);
		float phi = random(0, 2 * PI);
		particle.vel_.x = speed_ * std::sin(theta) * std::cos(phi);
		particle.vel_.y = speed_ * std::sin(theta) * std::sin(phi);
		particle.particleSize_ = particleSize_*0.8f;
		particle.alive = true;
		particle.lifeTime_ = 0;
		particle.angle_ = 0;
	}

	void Explosion::update(float deltaTime, Particle& particle) {
		particle.lifeTime_ += deltaTime;
		particle.pos_ += deltaTime * particle.vel_;

		static mw::Color colors[] = {
			mw::Color(1, 1, 1, 1),
			mw::Color(1, 0.5, 0.5, 1.0f),
			mw::Color(0.5, 0.5, 0.5, 0.3f),
			mw::Color(0.5, 0.5, 0.5, 0.0f)
		};
		if (time_ < 0.4 * lifeTime_) {
			particle.particleSize_ = time_ / 0.4f * 1.5f * deltaTime / 0.017f;
			particleEngine_.setColor(interpolate((time_) / particle.lifeTime_, colors[0], colors[1]));
		} else if (time_ < 0.8f * lifeTime_) {
			particle.particleSize_ = particleSize_;
			float ratio = (time_ - 0.4f * lifeTime_) / (0.4f * lifeTime_);
			particleEngine_.setColor(interpolate(ratio, colors[1], colors[2]));
		} else {
			particle.particleSize_ += deltaTime * 0.3f;
			particle.vel_ = 0.95f * deltaTime / 0.017f * particle.vel_;
			float ratio = (time_ - 0.8f * lifeTime_) / (0.2f * lifeTime_ + 1);
			particleEngine_.setColor(interpolate(ratio, colors[2], colors[3]));
		}

		if (particle.lifeTime_ > getTimeDuration()) {
			particle.alive = false;
		}
	}

	ExplosionProperties loadExplosion(GameDataEntry& entry) {
		ExplosionProperties explosionProperties;
		explosionProperties.delay_ = entry.getChildEntry("timeDelay").getFloat();
		explosionProperties.speed_ = entry.getChildEntry("speed").getFloat();
		explosionProperties.blastRadius_ = entry.getChildEntry("blastRadius").getFloat();
		explosionProperties.particle_ = entry.getChildEntry("particleImage").getTexture();
		explosionProperties.sound_ = entry.getChildEntry("sound").getSound();
		return explosionProperties;
	}

} // Namespace zombie.
