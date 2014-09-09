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
			auto wPtr = getWindowMatrix();
			mw::Matrix44 old = wPtr->getModel();
			wPtr->useShader();
			wPtr->setColor(1, 1, 1);
			// Draw body.
			wPtr->setModel(old * mw::getTranslateMatrix(getPosition().x, getPosition().y) * mw::getScaleMatrix(2 * getRadius(), 2 * getRadius()) * mw::getRotateMatrix(getAngle(), 0, 0, 1));
			tree_.draw();
			wPtr->setModel(old);
		}

	private:
		mw::Sprite tree_;
	};

} // Namespace zombie.

#endif // TREE2D_H
