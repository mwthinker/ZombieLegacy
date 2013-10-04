#ifndef SKY_H
#define SKY_H

#include "task.h"

namespace zombie {

	// A class responsible of drawing clouds, covering some part of the view.
	// More clouds the higher the view is. Can also show diffent types of weather, 
	// such as rain.
	class Sky : public Task {
	public:
		Sky() {
			remove_ = false;
		}

		void setRemove(bool remove) {
			remove_ = remove;
		}

		bool update(double time) override {


			return !remove_;
		}
		
	private:
		bool remove_;
	};

} // Namespace zombie.

#endif // SKY_H
