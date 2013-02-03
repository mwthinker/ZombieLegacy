#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "unit.h"

#include <cmath>
#include <SDL.h>

namespace zombie {

double calculateDifferenceBetweenAngles(double firstAngle, double secondAngle);

double calculateAnglePointToPoint(double x, double y, double pX, double pY);

class Player {
public:
	virtual Input currentInput() = 0;
};

class HumanPlayer : public Player {
public:
	virtual void eventUpdate(const SDL_Event& windowEvent) = 0;
};

class RemotePlayer {
public:
	void setInput(Input input);

	Input currentInput();
private:
	Input input_;
};

class AiBehavior {
public:
	AiBehavior();

	virtual ~AiBehavior();

	virtual Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time) = 0;
private:

};

class SimpleZombieBehavior : public AiBehavior {
public:
	SimpleZombieBehavior();

	Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time);

private: 
	int timeBeforeInputChange_;
	Input current_;
	double time_;

};

class BoringZombieBehavior : public AiBehavior {
public:
	BoringZombieBehavior();

	Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time);

private: 
	int timeBeforeInputChange_;
	Input current_;
	double time_;

};

class AgressiveZombieBehavior : public AiBehavior {
public:
	AgressiveZombieBehavior();

	Input calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time);

	bool setTarget(const std::vector<UnitPtr>& units);

	bool validateTarget();

	bool targetLocked();

	bool randomWalk();
private: 
	int timeBeforeInputChange_;
	UnitPtr target_;
	Input current_;
	double time_;

};
typedef std::shared_ptr<AiBehavior> AiBehaviorPtr;

class AiPlayer : public Player {
public:
	AiPlayer(AiBehaviorPtr behavior);

	AiPlayer();

	Input currentInput();

	void calculateInput(const UnitPtr& unit, const std::vector<UnitPtr>& units, double time);

	Input input_;
	AiBehaviorPtr behavior_;
};

} // namespace zombie.

#endif // LOCALPLAYER_H

