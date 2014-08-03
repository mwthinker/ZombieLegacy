#include "box2ddef.h"
#include "auxiliary.h"
#include "particleengine.h"
#include "fog.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include <array>

namespace zombie {

	namespace {

		void drawInversCircel(float radius, int num_segments) {
			float theta = 2 * PI / num_segments;
			float c = std::cos(theta);
			float s = std::sin(theta);
			float t;

			float x = radius;// Start at angle = 0
			float y = 0;

			glBegin(GL_TRIANGLE_STRIP);
			for (int i = 0; i < num_segments + 1; i++) {
				glVertex2f(x, y);
				glVertex2f(x * 100, y * 100);

				//apply the rotation matrix
				t = x;
				x = c * x - s * y;
				y = s * t + c * y;
			}
			glEnd();
		}

	}

	Fog::Fog(const mw::Texture& fog, float radius) : fog_(fog, radius, radius, 0.1f, 0.4f, 2.f, 5.f, 0.5f, 2.f) {
		radius_ = radius;
		//fog_.setColor(0.2f, 0.2f, 0.2f
	}

	void Fog::update(Position center) {
		center_ = center;
	}

	void Fog::draw(float deltaTime) {
		glPushMatrix();
		glTranslate2f(center_);
		fog_.draw(deltaTime);
		glRotatef(120);
		fog_.draw(deltaTime);
		glRotatef(120);
		fog_.draw(deltaTime);
		//glColor3f(0.2f, 0.2f, 0.2f);
		glColor3f(1.f, 1.f, 1.f);
		drawInversCircel(radius_, 20);
		glPopMatrix();
	}

} // Namespace zombie.
