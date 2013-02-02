#include "object.h"
#include "input.h"
#include "weapon.h"
#include "physicalunit.h"
#include <mw/mathvector.h>
#include <cmath>
#include <memory>

#include "unit.h"
#include "protocol.h"

namespace zombie {

double angleDifferance(double angleServer,double angleClient) {
	double diff = angleServer-angleClient;
	if (diff < mw::PI) {
		return diff;
	} else {
		return 2*mw::PI -diff;
	}
}

Unit::Unit(double x, double y, double angle, Weapon weapon, bool infected, int id) : PhysicalUnit(id, x,y,0.4, 50.0,0.0,1.0), weapon_(weapon) {		
	angleVelocity_ = 0.0;

	isInfected_ = infected;

	// Properties
	viewDistance_ = 10.0;
	viewAngle_ = 120.0/180.0*mw::PI;
	smallViewDistance_ = 2;

	// Unit's direction Angle.
	angle_ = angle;

	// Health
	healthPoints_ = 100.0;
	isDead_ = false;
}

Unit::~Unit() {
}

bool Unit::toRemove() const {
	return isDead();
}

Unit::Unit(mw::Packet& packet) : PhysicalUnit(packet) {
	packet >> angleVelocity_;
	packet >> angle_;
	packet >> viewDistance_;
	packet >> viewAngle_;
	packet >> smallViewDistance_;
	packet >> healthPoints_;
	packet >> isDead_;
	packet >> currentInput_;
	packet >> weapon_;
	packet >> isInfected_;
}

mw::Packet Unit::generatePacket() const {
	mw::Packet packet;
	packet << PhysicalUnit::generatePacket();
	packet << angleVelocity_;
	packet << angle_;
	packet << viewDistance_;
	packet << viewAngle_;
	packet << smallViewDistance_;
	packet << healthPoints_;
	packet << isDead_;
	packet << currentInput_;
	packet << weapon_;
	packet << isInfected_;
	return packet;
}

// Rpc excecuted on the client. Syncronize the internal state with
// the server.
void Unit::clientUpdate(double serverTime, Input input, State state) {
	/*
	State currentState = getState();

	// Smoth immediate states to server states.
	mw::MathVector positionDifference = state.position_ - currentState.position_;
	double distanceApart = positionDifference.magnitude();

	// Distance to large?
	if (distanceApart > 2.0) {
		// Snap.
        currentState.position_ = state.position_;
	} else if (distanceApart > 0.1) {
		// Smooth.
        currentState.position_ += positionDifference * 0.1;
	}

	// Angle difference to large?
	double angleDiff = calculateDifferenceBetweenAngles(state.angle_,currentState.angle_);
	if (std::abs(angleDiff) > mw::PI/8) {
		// Snap.
		currentState.angle_ = state.angle_;
	} else if (std::abs(angleDiff) > mw::PI/128) {
		// Smoth.
		currentState.angle_ += angleDiff * 0.1;
	}
        
	// Snap derivitive states to server states.
	currentState.velocity_ = state.velocity_;
	currentState.angleVelocity_ = state.angleVelocity_;
        		
	// Update input.
	currentInput_ = input;

	// Update states.
	setState(state);*/
}

// Rpc excecuted on the server. Receives input (input) 
// from the client at the time (time).
void Unit::receiveInput(double serverTime, double clientTime, Input input) {
    // Clientime is behind serverTime? 
	if (clientTime <= serverTime) {
		// Input to old!
		return;
	}	

    const double deltaTime = clientTime - serverTime;
	
    updatePhysics(serverTime, deltaTime, input);
}

void Unit::updatePhysics(double time, double timeStep, Input input) {
	double angle = moveDirection();	
		
	// Move forward or backwards.
	if (input.forward_ && !input.backward_) {
		addForce(Vec3(std::cos(angle),std::sin(angle))*8);
	} else if (!input.forward_ && input.backward_) {
		addForce(-Vec3(std::cos(angle),std::sin(angle))*8);
	} else {
		// In order to make the unit stop when not moving.
		addForce(-getVelocity());
	}

	//std::cout << getState().position_ << std::endl;

	// Turn left or right.
	if (input.turnLeft_ && !input.turnRight_) {			
		turn(3*timeStep);
	} else if (!input.turnLeft_ && input.turnRight_) {
		turn(-3*timeStep);
	}

	//currentInput_ = input;

	// Shoot, reload
	/*
	if (input.shoot_  && weapon_.shoot(time)) {			
		input.shoot_ = false;

		lastBullet_.direction_ = angle;
		lastBullet_.range_ = weapon_.range();
		lastBullet_.postion_ = getPosition();
		lastBullet_.damage_ = weapon_.damage();
	}
	if (input.reload_ && weapon_.reload()) {
		input.reload_ = true;
	}
	*/
}

void Unit::setState(State state) {
	angle_ = state.angle_;
	setPosition(state.position_);
	setVelocity(state.velocity_);
	angleVelocity_ = state.angleVelocity_;
}

State Unit::getState() const {
	State state;
	state.angle_ = angle_;
	state.position_ = getPosition();
	state.velocity_ = getVelocity();
	state.angleVelocity_ = angleVelocity_;
	return state;
}

// Returns the units view field in radians.
//virtual double viewAngle() = 0;	

double Unit::viewDistance() {
	return viewDistance_;
}

double Unit::smallViewDistance() {
	return smallViewDistance_;
}

double Unit::viewAngle() const {
	return viewAngle_;
}

bool Unit::isInside(double x, double y) const {
	Position p = getPosition();
	return (x - p.x_)*(x - p.x_) + (y - p.y_)*(y - p.y_) < radius()*radius();
}

bool Unit::isPointViewable(double x, double y) {
	Position p = Position(x,y) - getPosition();
	double angle = std::atan2(p.y_, p.x_);

	return p.magnitudeSquared() < smallViewDistance()*smallViewDistance() ||
		calculateDifferenceBetweenAngles(angle,angle_ + viewAngle() * 0.5) < 0 
		&& calculateDifferenceBetweenAngles(angle,angle_ - viewAngle() * 0.5) > 0
		&& p.magnitudeSquared() < viewDistance() * viewDistance();
}

double Unit::moveDirection() const {
	return angle_;
}

void Unit::turn(double angle) {
	angle_ += angle;

	if (angle_ > mw::PI) {
		angle_ -= 2*mw::PI;
	} else if (angle_ < -mw::PI) {
		angle_ += 2*mw::PI;
	}
}

double Unit::healthPoints() const {
	return healthPoints_;
}

void Unit::updateHealthPoint(double deltaLife) {
	if (!isDead_) {
		healthPoints_ += deltaLife;
	}
	if (healthPoints_ < 0) {
		isDead_ = true;
	}
}

bool Unit::isDead() const {
	return isDead_;
}

Input Unit::getInput() const {return currentInput_;}

// protected.
double Unit::calculateDifferenceBetweenAngles(double firstAngle, double secondAngle) {
	double difference = firstAngle - secondAngle;
	while (difference < -mw::PI) difference += 2*mw::PI;
	while (difference > mw::PI) difference -= 2*mw::PI;
	return difference;
}

// private.

} // namespace zombie.