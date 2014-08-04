#ifndef TREE2D_H
#define TREE2D_H

#include "box2ddef.h"
#include "auxiliary.h"
#include "tree.h"

#include <mw/sprite.h>

#include <memory>

namespace zombie {

	class Tree2D : public Tree {
	public:
		Tree2D(const Position& position, const mw::Sprite& tree) : Tree(position), tree_(tree) {
		}

		void draw(float accumulator, float timeStep) override {
			// Draw body.
			glPushMatrix();
			glTranslate2f(getPosition());
			glScale2f(2 * getRadius());
			glRotated(getAngle() * 180 / PI, 0, 0, 1);
			glColor3d(1, 1, 1);
			tree_.draw();
			glPopMatrix();
		}

	private:
		mw::Sprite tree_;
	};

} // Namespace zombie.

#endif // TREE2D_H
