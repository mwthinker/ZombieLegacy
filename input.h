#ifndef INPUT_H
#define INPUT_H

namespace zombie {

struct Input {
	Input() {
		shoot_ = false;
		reload_ = false;
		forward_ = false;
		backward_ = false;
		turnLeft_ = false;
		turnRight_ = false;
		run_ = false;
	}

	bool shoot_;
	bool reload_;
	bool forward_;
	bool backward_;
	bool turnLeft_;
	bool turnRight_;
	bool run_;
};

} // namespace zombie

#endif // INPUT_H