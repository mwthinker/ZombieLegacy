#include "task.h"

#include "graphictask.h"
#include "typedefs.h"
#include "unit.h"
#include "building.h"
#include "gamesound.h"
#include "gamefont.h"
#include "auxiliary.h"

#include <mw/text.h>
#include <SDL_opengl.h>

#include <memory>
#include <sstream>
#include <vector>
#include <queue>
#include <cmath>

namespace zombie {

	GraphicTask::~GraphicTask() {}

	void GraphicTask::drawFirst(double time) {
	}

	void GraphicTask::drawSecond(double time) {
	}

	void GraphicTask::drawThird(double time) {
	}

	HumanAnimation::HumanAnimation(const UnitPtr& unit) {
		unit_ = unit;
		//unit->addEventHandler(std::bind(&HumanAnimation::unitEventHandler,this));
		unit->addEventHandler([&](Unit::UnitEvent unitEvent){
			this->unitEventHandler(unitEvent);
		});

		timeNewFrame_ = 0.0;
		index_ = 0;
		lastTime_ = 0.0;
		color_ = Color(random(),random(),random());

		sprites_.push_back(human1);
		sprites_.push_back(human2);
		sprites_.push_back(human1);
		sprites_.push_back(human3);
	}

	void HumanAnimation::drawSecond(double time) {
		lastTime_ = time;
		draw(0.0);

		// Time is much larger?
		if (time > timeNewFrame_ + 1) {
			// In order for frames to sync to current time.
			timeNewFrame_ = 0.18 + time;
		}

		if (time > timeNewFrame_) {
			index_ = (1 + index_) % sprites_.size();
			timeNewFrame_ += 0.18;
		}
	}

	bool HumanAnimation::isRunning() const {
		return !unit_->isDead();
	}

	// private
	void HumanAnimation::draw(double timestep) {
		Position p = unit_->getPosition();

		// Draw body
		color_.glColor3d();
		glPushMatrix();
		glTranslated(p.x,p.y,0);
		glScaled(unit_->radius(),unit_->radius(),1);
		glRotated(unit_->getState().angle_*180/mw::PI,0,0,1);
		mw::TexturePtr texture = sprites_[index_].getTexture();
		glScaled(texture->getTexWidth()/128.0,texture->getTexHeight()/128.0,1);
		sprites_[index_].draw();
		glPopMatrix();
	}

	void HumanAnimation::unitEventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
		case Unit::UnitEvent::SHOOT:
			{
				// In order to be able to play even if the sound is not finnished!
				mw::Sound tmp = shot;				
				tmp.play();
			}
			break;
		case Unit::UnitEvent::RELOADING:
			{
				// In order to be able to play even if the sound is not finnished!
				mw::Sound tmp = reload;				
				tmp.play();
			}
			break;
		case Unit::UnitEvent::DIE:
			break;
		case Unit::UnitEvent::STANDSTILL:
			index_ = 0;
			timeNewFrame_ = lastTime_ + 0.18;
			break;
		case Unit::UnitEvent::WALK:
			// Fall through!
		case Unit::UnitEvent::RUN:
			break;
		}
	}

	ZombieAnimation::ZombieAnimation(const UnitPtr& unit) {
		unit_ = unit;
		//unit->addEventHandler(std::bind(&ZombieAnimation::unitEventHandler,this,Unit::UnitEvent::DIE));
		unit->addEventHandler([&](Unit::UnitEvent unitEvent){
			this->unitEventHandler(unitEvent);
		});

		timeNewFrame_ = 0.0;
		color_ = Color(random(),random(),random());

		sprites_.push_back(zombie1);
		sprites_.push_back(zombie2);
		sprites_.push_back(zombie3);
		sprites_.push_back(zombie4);
		sprites_.push_back(zombie5);
		sprites_.push_back(zombie6);
		index_ = (int) (random() * sprites_.size());
	}

	void ZombieAnimation::drawSecond(double time) {
		draw(0.0);
		//if (walk_) 

		// Time is much larger?
		if (time > timeNewFrame_ + 1) {
			// In order for frames to sync to current time.
			timeNewFrame_ = 0.18 + time;
		}

		if (time > timeNewFrame_) {
			index_ = (1 + index_) % sprites_.size();
			timeNewFrame_ += 0.18;
		}
	}

	bool ZombieAnimation::isRunning() const {
		return !unit_->isDead();
	}

	void ZombieAnimation::draw(double timestep) {
		Position p = unit_->getPosition();

		// Draw body
		color_.glColor3d();
		glPushMatrix();
		glTranslated(unit_->getPosition().x,unit_->getPosition().y,0);
		glScaled(unit_->radius(),unit_->radius(),1);
		glRotated(unit_->getState().angle_*180/mw::PI,0,0,1);
		glScaled(1069.0/128.0,1069.0/128.0,1);
		sprites_[index_].draw();
		glPopMatrix();
	}

	void ZombieAnimation::unitEventHandler(Unit::UnitEvent unitEvent) {
		switch (unitEvent) {
		case Unit::UnitEvent::SHOOT:
			//attack_.play();
			break;
		case Unit::UnitEvent::RELOADING:
			break;
		case Unit::UnitEvent::DIE:
			break;
		}
	}

	Shot::Shot(Position start, Position end, float currentTime) {
		startTime_ = currentTime;
		start_ = start;
		end_ = end;
		running_ = true;
	}

	void Shot::drawSecond(double time) {
		if (time < startTime_ + 2) {
			// Draw view sphere
			glColor3d(1,1,1);
			glBegin(GL_LINES);
			glVertex2d(start_.x,start_.y);
			glVertex2d(end_.x,end_.y);
			glEnd();
		} else {
			running_ = false;
		}
	}

	bool Shot::isRunning() const {
		return running_;
	}
	
	Death::Death(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void Death::drawFirst(double time) {
		if (time < startTime_ + 0.2) {
			// Draw view sphere
			glColor3d(1,0,0);
			drawCircle(x_,y_,(time-startTime_)*5,10,true);			
		} else {
			running_ = false;
		}
	}

	bool Death::isRunning() const {
		return running_;
	}

	BloodSplash::BloodSplash(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void BloodSplash::drawFirst(double time) {
		if (time < startTime_ + 0.1) {
			// Draw view sphere
			glColor3d(1,0,0);
			drawCircle(x_,y_,(time-startTime_)*3.5,10,true);			
		} else {
			running_ = false;
		}
	}

	bool BloodSplash::isRunning() const {
		return running_;
	}

	Blood::Blood(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
		blood_ = blood;
		startScaleX_ = 1;
		startScaleY_ = 1;
		endScaleX_ = 3.5;
		endScaleY_ = 3.5;
		duration_ = 0.15;
	}

	void Blood::drawFirst(double time) {
		double dT = time - startTime_;
		if (dT < duration_) {
			// Draw view sphere
			glPushMatrix();
			glColor3d(0.4,0.4,0.4);			
			glTranslated(x_,y_,0);
			glScaled(getScaleX(dT),getScaleY(dT),1);	 	
			blood_.draw();
			glPopMatrix();
			
		} else {
			running_ = false;
		}
	}

	bool Blood::isRunning() const {
		return running_;
	}

	double Blood::getScaleX(double dT) const {
		double s = startScaleX_ + (startScaleX_+endScaleX_)*(dT/duration_);

		return s;
	}

	double Blood::getScaleY(double dT) const {
		return getScaleX(dT);
	}

	BloodStain::BloodStain(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
		blood_ = blood;
		scaleX_ = 3;
		scaleY_ = 3;
		delay_ = 0.15;	// (Stain appears after splash is finished)
		duration_ = 60;
		mw::Sound tmp = splat;
		tmp.play();
	}

	void BloodStain::drawFirst(double time) {
  		double dT = time - startTime_;
		if (dT > delay_ && dT < duration_) {
			// Draw view sphere
			glPushMatrix();
			glColor3d(0.4,0.4,0.4);			
			glTranslated(x_,y_,0);
			glScaled(scaleX_,scaleY_,1);	 	
			blood_.draw();
			glPopMatrix();
			
		} else if (dT > delay_) {
			running_ = false;
		}
	}

	bool BloodStain::isRunning() const {
		return running_;
	}

	MapDraw::MapDraw(const Map& map) : map_(map) {
		grass_ = drawGrass;
	}

	void MapDraw::draw() {
		glColor3d(0.6,0.6,0.6);
		grassTexture->bind();	

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		double maxX = map_.maxX();
		double maxY = map_.maxY();
		double tH = grassTexture->getTexHeight()*1.0 / grassTexture->getPixHeight();
		double tW = grassTexture->getTexWidth()*1.0 / grassTexture->getPixWidth();

		glBegin(GL_QUADS);
		for (double x = map_.minX(); x < maxX; x += 10.0) {
			for (double y = map_.minY(); y < map_.maxY(); y += 10.0) {				
				glTexCoord2d(0, 0);
				glVertex2d(x, y);
				glTexCoord2d(tW, 0);
				glVertex2d(x+10, y);
				glTexCoord2d(tW, tH);
				glVertex2d(x+10, y+10);
				glTexCoord2d(0, tH);
				glVertex2d(x, y+10);				
			}
		}
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	void MapDraw::drawFirst(double time) {
		draw();
	}

	bool MapDraw::isRunning() const {
		return true;
	}

	RoadDraw::RoadDraw(const Map& map) : map_(map) {
		road_ = drawRoad;
	}

	void RoadDraw::draw() {
		const std::vector<LineFeature>& l = map_.getRoads();
		
		for (auto it = l.begin(); it != l.end(); it++) {
			double xStart =  it->getStart().x;
			double yStart =  it->getStart().y;
			double xEnd = it->getEnd().x;
			double yEnd =  it->getEnd().y;

			double x = xStart;
			double y = yStart;

			for (int i = 1; i < 100; i++) {
				glPushMatrix();
				glColor3d(0.4,0.4,0.4);
				glTranslated(0.5,0.5,0);
				glTranslated(x,y,0);
				//std::cout<<atan2((yStart-yEnd)/(xStart-xEnd));
				glRotated(std::atan2((yStart-yEnd),(xStart-xEnd)),0,0,1);
				glScaled(2,2,1);
				road_.draw();
				glPopMatrix();
				x = xStart + i*(xEnd-xStart)/100;
				y = yStart + i*(yEnd-yStart)/100;
			}
		}
	}

	void RoadDraw::drawFirst(double time) {
		draw();
	}

	bool RoadDraw::isRunning() const {
		return true;
	}

	DrawFake3DBuildning::DrawFake3DBuildning(const BuildingPtr& building) {
		buildning_ = building;
		road_ = drawRoad;
		d_ = (std::rand() % 100) / 300.0;
		height_  = 2 + rand() % (3 - 2 + 1);
		r_ = ((float) rand() / (RAND_MAX)) /5;
		g_ = ((float) rand() / (RAND_MAX)) /5; 
		b_ = ((float) rand() / (RAND_MAX)) /5;

		// Separate front from back *********************************************
		std::vector<Position> corners = building->getCorners();
		corners.pop_back();
		// Find mostleft corner as starting point
		float minX = 9999999;
		unsigned int startPos;
		for (unsigned int i = 0; i < corners.size(); i++) {
			if(corners[i].x < minX){
				startPos = i;
				minX = corners[i].x;
			}
		}
		// define first segment as front/back
		bool front;
		 
		if (corners[circularIndex(startPos+1, corners.size())].y < corners[circularIndex(-1,corners.size())].y) {
			front = true;
		} else {
			front = false;
		}

		bool previous = front;

		// Push back linefeatures to draw!
		
		for(unsigned int i = 0; i < corners.size(); i++) {
			unsigned index = circularIndex(startPos + i,corners.size());
						
			if(front) {
				if(corners[index].x < corners[circularIndex(index+1,corners.size())].x) {
					front_.push_back(LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())]));
				} else {
					back_.push_back(LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())]));
					rightCorner_.push_back((LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())])));
					front = false;
				}				
			} else {
				if(corners[circularIndex(index,corners.size())].x < corners[circularIndex(index+1,corners.size())].x) {
					front_.push_back(LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())]));
					leftCorner_.push_back((LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())])));
					front = true;
					
				} else {
					back_.push_back(LineFeature(corners[circularIndex(index,corners.size())],corners[circularIndex(index+1,corners.size())]));
				}				
			}

		}
		
		// put in lineFeature vectors!
	}

	void DrawFake3DBuildning::drawSecond(double time) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		
		for(LineFeature l : front_) {
			glColor3d(r_,g_,b_);
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(l.getStart().x,l.getStart().y);
			glVertex2d(l.getEnd().x,l.getEnd().y);
			glVertex2d(l.getEnd().x,l.getEnd().y+height_);
			glVertex2d(l.getStart().x,l.getStart().y+height_);
			glEnd();			
		}
		glColor3d(0,0,0);
		for(LineFeature l : front_) {
			glBegin(GL_LINE_STRIP);			
			glVertex2d(l.getStart().x,l.getStart().y);
			glVertex2d(l.getEnd().x,l.getEnd().y);
			glVertex2d(l.getEnd().x,l.getEnd().y+height_);
			glVertex2d(l.getStart().x,l.getStart().y+height_);
			glVertex2d(l.getStart().x,l.getStart().y);
			glEnd();

			// ADD DOOR
			// ax + by + c = 0
			double doorHeight = 1.2;
			double doorWidth = 0.3;
			double a = l.getStart().y - l.getEnd().y;
			double b = l.getEnd().x - l.getStart().x;
			double c = l.getStart().x * l.getEnd().y - l.getEnd().x * l.getStart().y;
			double sX = (l.getStart().x + l.getEnd().x)/2;			
			double sY = (l.getStart().y + l.getEnd().y)/2;

			glColor3d(0,0,0);
			glBegin(GL_TRIANGLE_FAN);
			
			glVertex2d(sX+doorWidth,getLineY(a,b,c,sX+doorWidth));
			glVertex2d(sX+doorWidth,getLineY(a,b,c,sX+doorWidth)+doorHeight);
			glVertex2d(sX-doorWidth,getLineY(a,b,c,sX-doorWidth)+doorHeight);
			glVertex2d(sX-doorWidth,getLineY(a,b,c,sX-doorWidth));

			glEnd();

		}
		glDisable(GL_BLEND);

		

	}

	double getLineY(double a,double b,double c,double x) {
		return (0-c-a*x)/b;
	}

	void DrawFake3DBuildning::drawThird(double time) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		for(LineFeature l : back_) {
			glColor3d(r_,g_,b_);
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(l.getStart().x,l.getStart().y);
			glVertex2d(l.getEnd().x,l.getEnd().y);
			glVertex2d(l.getEnd().x,l.getEnd().y+height_);
			glVertex2d(l.getStart().x,l.getStart().y+height_);
			glEnd();			
		}

		// ROOF
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(r_,g_,b_);
		const auto& corners = buildning_->getCorners();
		for (const Position& p : corners) {
			glVertex2d(p.x,p.y+height_);
		}		
		glEnd();

		

		// HELP AREAS
		for(LineFeature l : rightCorner_) {
			glColor3d(r_,g_,b_);
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(l.getStart().x,l.getStart().y);
			glVertex2d(l.getEnd().x,l.getEnd().y);
			glVertex2d(l.getEnd().x,l.getStart().y);
			glVertex2d(l.getStart().x,l.getStart().y);
			glEnd();			
		}

		// ROOF OUTLINE
		glBegin(GL_LINE_STRIP);
		glColor3d(0,0,0);
		for (const Position& p : corners) {
			glVertex2d(p.x,p.y+height_);
		}		
		glEnd();
		glDisable(GL_BLEND);
	}

	bool DrawFake3DBuildning::isRunning() const {
		return true;
	}

	//private
	void DrawFake3DBuildning::draw() {
		double height = 1.5;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		const auto& corners = buildning_->getCorners();
		
		for (unsigned int i = 0; i < corners.size(); i++) {
			
			//WALLS
			glColor3d(r_,g_,b_);
			if (i == corners.size()-1){
			
			} else {
				//glBegin(GL_LINE_LOOP);
				
				glBegin(GL_TRIANGLE_FAN);
				glVertex2d(corners[i].x,corners[i].y);
				glVertex2d(corners[i+1].x,corners[i+1].y);
				glVertex2d(corners[i+1].x,corners[i+1].y+height_);
				glVertex2d(corners[i].x,corners[i].y+height_);
				glEnd();
			}
		}

		// OUTLINE VERTICAL
		glColor3d(0,0,0);
		glLineWidth(3);
		for (unsigned int i = 0; i < corners.size(); i++) {
			
			glBegin(GL_LINES);
			unsigned int s = corners.size();
			glEnd();
			glBegin(GL_LINES);
			glVertex2d(corners[i].x,corners[i].y);
			glVertex2d(corners[i].x,corners[i].y+height_);
			glEnd();			
		}		

		glLineWidth(0.01f);
		// ROOF
		
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(r_,g_,b_);
		for (const Position& p : corners) {
			glVertex2d(p.x,p.y+height_);
		}		
		glEnd();
		// OUTLINE HORISONTAL
		glLineWidth(3);
		glColor3d(0,0,0);
		glBegin(GL_LINE_STRIP);
		for (unsigned int i = 0; i < corners.size(); i++) {			
			unsigned int s = corners.size();			
			glVertex2d(corners[circularIndex(i,s)].x,corners[circularIndex(i,s)].y+height_);			
		}
		glEnd();			
		glLineWidth(0.01f);
		glDisable(GL_BLEND);		
	}	

	unsigned int circularIndex(int i, int s) {
		unsigned int r = (i + s) % s;
		return r;
	}

	
} // Namespace zombie.
