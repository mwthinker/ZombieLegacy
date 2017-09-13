#include "car2d.h"
#include "auxiliary.h"
#include "animation.h"

#include <mw/opengl.h>

namespace zombie {

	Car2D::Car2D() : 
		Car(1, 1, 1, 1) {

	}

	Car2D::Car2D(float mass, float life, float width, float length, const Animation& animation) :
		Car(mass, life, width, length), animation_(animation) {
	}

	// Draws the car.
	void Car2D::draw(double accumulator, double timeStep, const GameShader& gameShader) {
		// Draw body.
		const float alpha = (float) (accumulator / timeStep);

		State state = getState();
		state.position_ = alpha * state.position_ + (1.f - alpha) * previousState().position_;

		gameShader.setGlColorU(1, 1, 1);
		gameShader.setGlGlobalPositionU(getPosition());
		gameShader.setGlLocalAngleU(getDirection());
		animation_.draw(timeStep, 0, 0, getLength(), getLength(), gameShader);
	}

	Car2D loadCar() {
		/*
		Animation animation = entry.getChildEntry("moveAnimation").getAnimation();
		float mass = entry.getChildEntry("mass").getFloat();
		float width = entry.getChildEntry("width").getFloat();
		float length = entry.getChildEntry("length").getFloat();
		float life = entry.getChildEntry("life").getFloat();
		return Car2D(mass, life, width, length, animation);
		*/
		return Car2D();
	}
	
} // Namespace zombie.
