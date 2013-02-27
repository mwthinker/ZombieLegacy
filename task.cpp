#include "task.h"

#include <vector>
#include <queue>
#include <SDL_opengl.h>
#include <cmath>

#include "unit.h"
#include "building.h"
#include "gamesound.h"
#include "gamefont.h"

#include <memory>
#include <sstream>
#include <mw/text.h>

namespace zombie {

	typedef std::shared_ptr<Unit> UnitPtr;

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
		return !buildning_->isDead();
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
	}

	void HumanAnimation::excecute(double time) {
		draw(0.0);
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
	}

	void HumanAnimation::unitEventHandler(Unit::UnitEvent unitEvent) {
		std::cout << unitEvent << "\n";
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
		}
	}

	ZombieAnimation::ZombieAnimation(const UnitPtr& unit) {
		unit_ = unit;
		//unit->addEventHandler(std::bind(&ZombieAnimation::unitEventHandler,this,Unit::UnitEvent::DIE));
		unit->addEventHandler([&](Unit::UnitEvent unitEvent){
			this->unitEventHandler(unitEvent);
		});
	}

	void ZombieAnimation::excecute(double time) {
		draw(0.0);
	}

	bool ZombieAnimation::isRunning() const {
		return !unit_->isDead();
	}

	void ZombieAnimation::draw(double timestep) {
		Position p = unit_->getPosition();
		glColor3d(0.8,0.4,0.4);
		// Draw body		
		drawCircle(p[0],p[1],unit_->radius(),20,true);
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
		return !buildning_->isDead();
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
		Position centre = buildning_->getMassCentre();
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

		std::vector<LineFeature> l = map_.getRoads();
		
		for (auto it = l.begin(); it != l.end(); it++) {
			int xStart =  it->getStart().x_;
			int yStart =  it->getStart().y_;
			int xEnd = it->getEnd().x_;
			int yEnd =  it->getEnd().y_;

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

	void RoadDraw::excecute(double time) {
		draw();
	}

	bool RoadDraw::isRunning() const {
		return true;
	}

} // Namespace zombie.
