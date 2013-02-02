#ifndef AIUNITMANAGER_H
#define AIUNITMANAGER_H

namespace zombie {

class AiUnitManager {
public:
	AiUnitManager() {}

	void placeZombie();
	void removeZombie();

	void manageZombies() {
		// zombie spawn / 5 second
		// 
	
	}

private:
	double unitAreaRange_;  


};

}

#endif // AIUNITMANAGER_H