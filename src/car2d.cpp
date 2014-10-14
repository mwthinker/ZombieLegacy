#include "car2d.h"
#include "auxiliary.h"
#include "animation.h"
#include "gamedataentry.h"

#include <mw/opengl.h>

namespace zombie {

	Car2D::Car2D(float mass, float life, float width, float length, const Animation& animation) :
		Car(mass, life, width, length), animation_(animation) {
	}

	// Draws the car.
	void Car2D::draw(float accumulator, float timeStep, const GameShader& gameShader) {
		// Draw body.
		const float alpha = accumulator / timeStep;

		State state = getState();
		state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

		//wPtr->useShader();
		//wPtr->setColor(1, 1, 1);
		//mw::Matrix44 old = wPtr->getModel();
		//wPtr->setModel(old * mw::getTranslateMatrix44(state.position_.x, state.position_.y) * mw::getRotateMatrix44(state.angle_, 0, 0, 1) * mw::getScaleMatrix44(getWidth(), getWidth()));
		//animation_.draw(timeStep, wPtr);
		//wPtr->setModel(old);
	}

	Car2D loadCar(GameDataEntry& entry) {
		Animation animation = entry.getChildEntry("moveAnimation").getAnimation();
		float mass = entry.getChildEntry("mass").getFloat();
		float width = entry.getChildEntry("width").getFloat();
		float length = entry.getChildEntry("length").getFloat();
		float life = entry.getChildEntry("life").getFloat();
		return Car2D(mass, life, width, length, animation);
	}
	
} // Namespace zombie.
