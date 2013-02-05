#include "player.h"
#include "input.h"
#include "unit.h"

#include <cmath>
#include <SDL.h>

namespace zombie {

	double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle) {
		double difference = firstAngle - secondAngle;
		while (difference < -mw::PI) difference += 2*mw::PI;
		while (difference > mw::PI) difference -= 2*mw::PI;
		return difference;
	}

	double calculateAnglePointToPoint(double x, double y, double pX, double pY) {
		return atan2(pY-y,pX-x);
	}

	AiBehavior::AiBehavior() {
	}

	AiBehavior::~AiBehavior() {
	}

	SimpleZombieBehavior::SimpleZombieBehavior() {
		timeBeforeInputChange_ = 5;
		current_.forward_ = true;
		time_ = 0;
	}

	Input SimpleZombieBehavior::calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) {
		Input input;
		int t = static_cast<int>(time);
		input.forward_ = true;

		if ( fmod(t,timeBeforeInputChange_*2) == 0) {
			input.forward_ = true;
		} else if( fmod(t,timeBeforeInputChange_*2) == timeBeforeInputChange_) {
			input.forward_ = false;
			input.backward_ = true;
		}

		return input;

	}


	AgressiveZombieBehavior::AgressiveZombieBehavior() {
	}

	Input AgressiveZombieBehavior::calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) {
		Input input;
		//input.forward_ = true;
		// is there a  valid target? OR is there a new target available?
		if ((targetLocked() && validateTarget()) || setTarget(units)) {
			// Create input ******************
			// Turn to right angle
			calculateDifferenceBetweenAngles(10,15);
			Position selfP = unit->getPosition();
			Position targetP = target_->getPosition();

			double angle = calculateAnglePointToPoint(selfP.x_, selfP.y_, targetP.x_, targetP.y_);
			double direction = unit->moveDirection();

			//std::cout<<"\n " << "Point2PointAngle " << angle;
			//std::cout<<"direction: " << direction;

			if (angle - direction > 0.3) {              //LEFT
				input.turnLeft_ = true;
				input.turnRight_ = false;
			} else if (angle  - direction < 0.1) {       //RIGHT
				input.turnRight_ = true;
				input.turnLeft_ = false;
			} else {                     //NO TURN
				input.turnRight_ = false;
				input.turnLeft_ = false;
			}

			// Walk
			//input.forward_ = true;

			// Attack - implement later
		} else {
			// No targets available - do random walk
			target_ = 0;
			//input.forward_ = true;
			if(randomWalk()) {

			}
			input.turnRight_ = true;
		}
		return input;			
	}

	bool AgressiveZombieBehavior::setTarget(const std::vector<UnitPtr>& units) {

		double minimumDist = 99999999;
		for (UnitPtr unit : units) {
			// if unit is in range
			// if unit is infected
			if(!unit->isInfected()) { 
				target_ = unit;
				return true;
			}
			return false;
		}
		return false;
	}

	bool AgressiveZombieBehavior::validateTarget() {
		return true;
	}

	bool AgressiveZombieBehavior::targetLocked() {
		return target_ != 0;
	}

	bool AgressiveZombieBehavior::randomWalk() {
		int turn = rand() % 5 + 1;
		if (turn == 2) {
			return true;
		} else {
			return true;
		}
	}


	BoringZombieBehavior::BoringZombieBehavior() {
		timeBeforeInputChange_ = 5;
		current_.forward_ = true;
		time_ = 0;
	}

	Input BoringZombieBehavior::calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) {
		Input input;


		input.forward_ = true;

		double d = fmod(time,3);
		if(d<2) {
			input.forward_ = true;
			input.turnLeft_ = false;
			input.turnRight_ = false;
		} else if (d<2.1) {
			input.forward_ = false;
			input.turnLeft_ = true;
			input.turnRight_ = false;
		} else if (d<2.4) {
			input.forward_ = false;
			input.turnLeft_ = false;
			input.turnRight_ = false;
		} else {
			input.forward_ = true;
			input.turnLeft_ = false;
			input.turnRight_ = false;
		}

		return input;
	}

	AiPlayer::AiPlayer(AiBehaviorPtr behavior) : behavior_(behavior) {	

	}	

	AiPlayer::AiPlayer() {
		//behavior_ = AiBehaviorPtr(new SimpleZombieBehavior());
		//behavior_ = AiBehaviorPtr(new AgressiveZombieBehavior());
		behavior_ = AiBehaviorPtr(new BoringZombieBehavior());
	}	

	Input AiPlayer::currentInput() {
		return input_;
	}

	void AiPlayer::calculateInput(const UnitPtr& unit, double time) {
		input_ = behavior_->calculateInput(unit, units_, time);
	}

	void AiPlayer::updateUnitsInView(const std::vector<UnitPtr>& units) {
		units_ = units;
	}

} // namespace zombie.
