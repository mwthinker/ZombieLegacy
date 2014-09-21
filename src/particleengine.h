#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "particle.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>

#include <memory>
#include <array>

namespace zombie {

	class ParticleShader {
	public:
		ParticleShader() {
			if (nbrInstances < 1) {
				shader = std::make_shared<mw::Shader>();
				shader->bindAttribute("aPos");
				shader->bindAttribute("aTex");
				shader->bindAttribute("aColor");
				shader->bindAttribute("aAngle");
				shader->loadAndLinkFromFile("particle.ver.glsl", "particle.fra.glsl");
			}
			++nbrInstances;
		}

		~ParticleShader() {
			--nbrInstances;
		}

		void glUseProgram() {
			shader->glUseProgram();
		}

		void setUniformColor(const mw::Color& color) {
			mw::glUniform4f(shader->getUniformLocation("uColor"), color.red_, color.green_, color.blue_, color.alpha_);
		}

		void setUniformMatrix(const mw::Matrix44& matrix) {
			mw::glUniformMatrix4fv(shader->getUniformLocation("uMat"), 1, false, matrix.data());
		}

		void setVertices(int dim, const GLvoid* data) {
			mw::glEnableVertexAttribArray(shader->getAttributeLocation("aPos"));
			mw::glVertexAttribPointer(shader->getAttributeLocation("aPos"), dim, GL_FLOAT, GL_FALSE, 0, data);
		}

		void setTexCoords(int dim, const GLvoid* data) {
			mw::glEnableVertexAttribArray(shader->getAttributeLocation("aTex"));
			mw::glVertexAttribPointer(shader->getAttributeLocation("aTex"), dim, GL_FLOAT, GL_FALSE, 0, data);
		}

		void setAngles(int dim, const GLvoid* data) {
			mw::glEnableVertexAttribArray(shader->getAttributeLocation("aAngle"));
			mw::glVertexAttribPointer(shader->getAttributeLocation("aAngle"), dim, GL_FLOAT, GL_FALSE, 0, data);
		}
		
	private:
		static mw::ShaderPtr shader;

		static int nbrInstances;
	};

	template <int MAX_PARTICLES, class Emitter>
	class ParticleEngine {
	public:
		ParticleEngine(Emitter& emitter, const mw::Texture& texture);

		void setLoop(bool loop) {
			loop_ = loop;
		}

		void draw(float deltaTime, const mw::Matrix44& matrix);

		void restart() {
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
		void setAttributes(const Particle& particle, int index, GLfloat aPos[], GLfloat aTex[], GLfloat aAngle[]);

		ParticleShader shader_;
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
		texture_ = texture;
		loop_ = true;
		addColor_ = true;

		restart();
	}

	template <int MAX_PARTICLES, class Emitter>
	void ParticleEngine<MAX_PARTICLES, Emitter>::draw(float deltaTime, const mw::Matrix44& matrix) {
		time_ += deltaTime;

		GLfloat aPos[MAX_PARTICLES * 6 * 2]; // 2 triangle per particle. I.e. (3 * 2) * 2 = 12.
		GLfloat aTex[MAX_PARTICLES * 6 * 2]; // 2 per vertex.
		GLfloat aAngle[MAX_PARTICLES * 6]; // 1 per vertex.

		int index = 0;
		for (auto& particle : particles_) {
			// Is the particle alive?
			if (particle.alive) {
				// Update the position and draw it.
				setAttributes(particle, index, aPos, aTex, aAngle);
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

		shader_.glUseProgram();
		texture_.bindTexture();
		shader_.setUniformColor(color_);
		shader_.setUniformMatrix(matrix);		
		
		shader_.setVertices(2, aPos);
		shader_.setTexCoords(2, aTex);
		shader_.setAngles(2, aAngle);

		mw::glDrawArrays(GL_TRIANGLES, 0, index * 6);

		mw::glDisable(GL_BLEND);
	}

	template <int MAX_PARTICLES, class Emitter>
	void ParticleEngine<MAX_PARTICLES, Emitter>::setAttributes(const Particle& particle, int index, GLfloat aPos[], GLfloat aTex[], GLfloat aAngle[]) {
		// 1:st triangle.
		aPos[12 * index + 0] = particle.pos_.x - particle.particleSize_*0.5f;
		aTex[12 * index + 0] = 0;
		aPos[12 * index + 1] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 1] = 0;

		aPos[12 * index + 2] = particle.pos_.x + particle.particleSize_*0.5f;
		aTex[12 * index + 2] = 1;
		aPos[12 * index + 3] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 3] = 0;

		aPos[12 * index + 4] = particle.pos_.x - particle.particleSize_*0.5f;
		aTex[12 * index + 4] = 0;
		aPos[12 * index + 5] = particle.pos_.y + particle.particleSize_*0.5f;;
		aTex[12 * index + 5] = 1;

		// 2:nd triangle.
		aPos[12 * index + 6] = particle.pos_.x - particle.particleSize_*0.5f;
		aTex[12 * index + 6] = 0;
		aPos[12 * index + 7] = particle.pos_.y + particle.particleSize_*0.5f;;
		aTex[12 * index + 7] = 1;

		aPos[12 * index + 8] = particle.pos_.x + particle.particleSize_*0.5f;
		aTex[12 * index + 8] = 1;
		aPos[12 * index + 9] = particle.pos_.y - particle.particleSize_*0.5f;
		aTex[12 * index + 9] = 0;

		aPos[12 * index + 10] = particle.pos_.x + particle.particleSize_*0.5f;
		aTex[12 * index + 10] = 1;
		aPos[12 * index + 11] = particle.pos_.y + particle.particleSize_*0.5f;;
		aTex[12 * index + 11] = 1;

		// Angles for 6 verteces.
		for (int i = 0; i < 6; ++i) {
			aAngle[index + i] = particle.angle_;
		}
	}

} // Namespace zombie.

#endif // PARTICLEENGINE_H
