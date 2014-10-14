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

		void draw(float accumulator, float timeStep, const GameShader& gameShader) override {
			gameShader.setGlColorU(1, 1, 1);
			gameShader.setGlTextureU(true);
			gameShader.setGlPositionU(position_);
			gameShader.setGlAngleU(getAngle());
			//tree_.draw(position_.x, position_.y);
			// Draw body.
			//wPtr->setModel(old * mw::getTranslateMatrix44(getPosition().x, getPosition().y) * mw::getScaleMatrix44(2 * getRadius(), 2 * getRadius()) * mw::getRotateMatrix44(getAngle(), 0, 0, 1));
			//tree_.draw(position_.x, position_.y);
		}

	private:
		mw::Sprite tree_;
		Position position_;
	};

} // Namespace zombie.

#endif // TREE2D_H
