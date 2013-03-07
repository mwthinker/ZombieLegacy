#include "task.h"

#include "unit.h"
#include "building.h"
#include "gamesound.h"
#include "gamefont.h"

#include <mw/text.h>
#include <SDL_opengl.h>

#include <memory>
#include <sstream>
#include <vector>
#include <queue>
#include <cmath>

namespace zombie {

	int Task::width = 0;
	int Task::height = 0;

	Task* Task::pull() {
		if (tasks_.empty()) {
			return nullptr;
		}

		Task* task = tasks_.front();
		tasks_.pop();
		return task;
	}	

	void Task::push(Task* task) {
		tasks_.push(task);
	}

	void drawCircle(double cx, double cy, double r, int num_segments, bool filled) {
		double theta = 2 * 3.1415926 / double(num_segments);
		double c = std::cos(theta);
		double s = std::sin(theta);
		double t;

		double x = r ;// Start at angle = 0
		double y = 0;

		if (filled) {
			glBegin(GL_TRIANGLE_FAN);
			glVertex2d(cx,cy);
		} else {
			glBegin(GL_LINE_LOOP);
		}
		for(int i = 0; i < num_segments+1; i++) {
			glVertex2d(x + cx, y + cy);
			//apply the rotation matrix
			t = x;
			x = c * x - s * y;
			y = s * t + c * y;
		}
		glEnd();
	}

	DrawBuildning::DrawBuildning(const BuildingPtr& building) {
		buildning_ = building;
	}

	void DrawBuildning::excecute(double time) {
		draw();
	}

	bool DrawBuildning::isRunning() const {
		return true;
	}

	//private
	void DrawBuildning::draw() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glColor3d(0.1,0.9,0.4);
		glBegin(GL_LINE_LOOP);
		const auto& corners = buildning_->getCorners();
		for (const Position& p : corners) {
			glVertex2d(p.x_,p.y_);
		}		
		glEnd();		
		glDisable(GL_BLEND);
	}

	HumanAnimation::HumanAnimation(const UnitPtr& unit) {
		unit_ = unit;
		//unit->addEventHandler(std::bind(&HumanAnimation::unitEventHandler,this));
		unit->addEventHandler([&](Unit::UnitEvent unitEvent){
			this->unitEventHandler(unitEvent);
		});

		timeNewFrame_ = 0.0;
		walk_ = true;
		index_ = 0;
		lastTime_ = 0.0;

		sprites_.push_back(human1);
		sprites_.push_back(human2);
		sprites_.push_back(human1);
		sprites_.push_back(human3);
	}

	void HumanAnimation::excecute(double time) {
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

		glColor3d(0.7,1,0.7);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,true);

		// innerSpawnRadius
		drawCircle(p[0],p[1],10,20,false);
		// outerSpawnRadius
		drawCircle(p[0],p[1],20,20,false);


		glColor3d(1,1,1);
				
		// Draw view sphere
		//drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+0.1*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+0.1*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		
		glEnd();
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/

		// Draw body
		glColor3d(1,1,1);
		glPushMatrix();
		glTranslated(p.x_,p.y_,0);
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
		walk_ = true;
		index_ = 0;

		sprites_.push_back(zombie1);
		sprites_.push_back(zombie2);
		sprites_.push_back(zombie3);
		sprites_.push_back(zombie4);
		sprites_.push_back(zombie5);
		sprites_.push_back(zombie6);
	}

	void ZombieAnimation::excecute(double time) {
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
		glColor3d(1,1,1);
		glPushMatrix();
		glTranslated(unit_->getPosition().x_,unit_->getPosition().y_,0);
		glScaled(unit_->radius(),unit_->radius(),1);
		glRotated(unit_->getState().angle_*180/mw::PI,0,0,1);
		glScaled(1069.0/128.0,1069.0/128.0,1);
		sprites_[index_].draw();
		glPopMatrix();

		/*
		glColor3d(0.8,0.4,0.4);
		drawCircle(p[0],p[1],unit_->radius(),20,true);
		glColor3d(1,1,1);
		glBegin(GL_LINES);		
		double gg = unit_->radius();
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());		

		glEnd();
		*/
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

	SurvivorAnimation::SurvivorAnimation(const UnitPtr& unit) {
		unit_ = unit;
	}

	void SurvivorAnimation::excecute(double time) {
		draw(0.0);
	}

	bool SurvivorAnimation::isRunning() const {
		return !unit_->isDead();
	}

	void SurvivorAnimation::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(0,1,0);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,false);
		//drawCircle(p[0],p[1],unit_->radius()*0.5,20,false);

		glColor3d(1,1,1);
				
		// Draw view sphere
		//drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance());
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance());
		
		double gg = unit_->radius();
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()-3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()-3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		//glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2));
		//glVertex2d(p[0]+0.04*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+0.04*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2));
		glVertex2d(p[0]-unit_->radius()*std::cos(unit_->moveDirection()+3.14/2)+0.06*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]-unit_->radius()*std::sin(unit_->moveDirection()+3.14/2)+0.06*std::sin(unit_->moveDirection())*unit_->viewDistance());		

		glEnd();
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/
	}

	Shot::Shot(double x, double y, double currentTime) {
		startTime_ = currentTime;
		x_ = x;
		y_ = y;
		running_ = true;
	}

	void Shot::excecute(double time) {
		if (time < startTime_ + 2) {
			// Draw view sphere
			glColor3d(0,1,0);
			drawCircle(x_,y_,0.5,10,true);			
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

	void Death::excecute(double time) {
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

	void BloodSplash::excecute(double time) {
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

	HumanAnimation3D::HumanAnimation3D(const UnitPtr& unit) {
		unit_ = unit;
		lastTime_ = 0.0;
	}

	void HumanAnimation3D::excecute(double time) {
		draw(time-lastTime_);
	}

	bool HumanAnimation3D::isRunning() const {
		return !unit_->isDead();
	}

	// private
	void HumanAnimation3D::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(1,1,1);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,false);
		// ViewRadius
		drawCircle(p[0],p[1],300,20,false);
		// SpawnRadius
		drawCircle(p[0],p[1],400,20,false);

		glColor3d(1,1,1);
				
		// Draw view sphere
		//drawCircle(p[0],p[1],unit_->viewDistance(),20,false);
		glBegin(GL_LINES);
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() - 0.5*unit_->viewAngle())*unit_->viewDistance());
		//glVertex2d(p[0],p[1]);
		//glVertex2d(p[0]+std::cos(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance(),p[1]+std::sin(unit_->moveDirection() + 0.5*unit_->viewAngle())*unit_->viewDistance());
		
		glVertex2d(p[0],p[1]);
		glVertex2d(p[0]+0.1*std::cos(unit_->moveDirection())*unit_->viewDistance(),p[1]+0.1*std::sin(unit_->moveDirection())*unit_->viewDistance());		
		
		glEnd();
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/
	}

	HumanStatus::HumanStatus(const UnitPtr& unit, Player player) {
		unit_ = unit;
		lastTime_ = 0.0;
		name_ = mw::Text("", font15);
		ammo_ = mw::Text("", font15);
		life_ = mw::Text("", font15);
		player_ = player;
	}

	void HumanStatus::excecute(double time) {
		draw(time-lastTime_);
	}

	bool HumanStatus::isRunning() const {
		return !unit_->isDead();
	}

	void HumanStatus::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(1,1,1);

		name_.setText("Human");
		life_.setText("Life");

		std::stringstream stream;
		Weapon w = unit_->getWeapon();
		stream << w.getBulletsInWeapon() << " (" << w.clipSize() << ")";
		ammo_.setText(stream.str());
		stream.str("");
		stream <<  " (" << unit_->healthPoints() << ")";
		life_.setText(stream.str());
		
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,Task::width,0,Task::height,-1,1);		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//glScaled(100,100,1);
		glTranslated(0,75,0);
		name_.draw();


		//glTranslated(0,-font15->getCharacterSize()*1.2,0);
		glTranslated(0,-25,0);

		ammo_.draw();
		
		glTranslated(0,-25,0);

		life_.draw();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
		/*
		// Draw small view sphere
		drawCircle(p[0],p[1],unit_->smallViewDistance(),20,false);
		*/
	}

	Buildning3DTask::Buildning3DTask(const BuildingPtr& building) {
		buildning_ = building;
	}

	void Buildning3DTask::excecute(double time) {
		draw();
	}

	bool Buildning3DTask::isRunning() const {
		return true;
	}

	//private
	void Buildning3DTask::draw() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		
		// Walls.
		glColor3d(0.1,0.9,0.4);
		glBegin(GL_QUAD_STRIP);
		const auto& corners = buildning_->getCorners();
		int size = corners.size();
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
			glVertex3d(corners[i].x_,corners[i].y_,1);
		}
		glEnd();

		// Roof.
		glColor3d(0.6,0.2,0.4);
		Position centre = buildning_->getPosition();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(centre.x_,centre.y_,1);
		for (int i = 0; i < size; ++i) {
			glVertex3d(corners[i].x_,corners[i].y_,0);
		}
		glEnd();
		glDisable(GL_BLEND);
	}

	MapDraw::MapDraw(const Map& map) : map_(map) {
		grass_ = drawGrass;
	}

	void MapDraw::draw() {		
		for (int x = static_cast<int>( map_.minX()); x<static_cast<int>( map_.maxX()); x=x+10) {
			for (int y = static_cast<int>( map_.minY()); y<static_cast<int>( map_.maxY()); y=y+10) {
				glPushMatrix();
				glColor3d(0.4,0.4,0.4);
				glTranslated(0.5,0.5,0);
				glTranslated(x,y,0);
				glScaled(10,10,1);	 	
				grass_.draw();
				glPopMatrix();
			}
		}
	}

	void MapDraw::excecute(double time) {
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
			double xStart =  it->getStart().x_;
			double yStart =  it->getStart().y_;
			double xEnd = it->getEnd().x_;
			double yEnd =  it->getEnd().y_;

			double x = xStart;
			double y = yStart;

			for (int i = 1; i < 100; i++) {
				glPushMatrix();
				glColor3d(0.4,0.4,0.4);
				glTranslated(0.5,0.5,0);
				glTranslated(x,y,0);
				//std::cout<<atan2((yStart-yEnd)/(xStart-xEnd));
				//glRotated(std::atan2((yStart-yEnd),(xStart-xEnd)),0,0,1);
				glScaled(2,2,1);
				road_.draw();
				glPopMatrix();
				x = xStart + i*(xEnd-xStart)/100;
				y = yStart + i*(yEnd-yStart)/100;
			}
		}
	}

	void RoadDraw::excecute(double time) {
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
		r_ = ((double) rand() / (RAND_MAX));
		g_ = ((double) rand() / (RAND_MAX)); 
		b_ = ((double) rand() / (RAND_MAX));
	}

	void DrawFake3DBuildning::excecute(double time) {
		draw();
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
				glVertex2d(corners[i].x_,corners[i].y_);
				glVertex2d(corners[i+1].x_,corners[i+1].y_);
				glVertex2d(corners[i+1].x_,corners[i+1].y_+height_);
				glVertex2d(corners[i].x_,corners[i].y_+height_);
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
			glVertex2d(corners[i].x_,corners[i].y_);
			glVertex2d(corners[i].x_,corners[i].y_+height_);
			glEnd();
			
		}
		

		glLineWidth(0.01);
		// ROOF
		
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(r_,g_,b_);
		for (const Position& p : corners) {
			glVertex2d(p.x_,p.y_+height_);
		}		
		glEnd();
		// OUTLINE HORISONTAL
		glLineWidth(3);
		glColor3d(0,0,0);
		glBegin(GL_LINE_STRIP);
		for (unsigned int i = 0; i < corners.size(); i++) {			
			unsigned int s = corners.size();			
			glVertex2d(corners[circularIndex(i,s)].x_,corners[circularIndex(i,s)].y_+height_);			
			//std::cout << "i: "<< i <<"x: "<<corners[circularIndex(i,s)].x_<< " y: "<<corners[circularIndex(i,s)].y_+height << std::endl;
			//int aa = 55;
		}
		glEnd();			
		glLineWidth(0.01);


		glDisable(GL_BLEND);
		
	}

	
	

	unsigned int circularIndex(int i, int s) {
		unsigned int r = (i + s) % s;
		return r;
	}
} // Namespace zombie.
