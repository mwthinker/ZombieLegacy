#ifndef WEAPON2D_H
#define WEAPON2D_H

#include "weapon.h"
#include "animation.h"
#include "auxiliary.h"

#include <mw/sprite.h>
#include <mw/sound.h>

#include <memory>

namespace zombie {
	
	// Describes a Weapon and is responsible of shooting.
	class Weapon2D : public Weapon {
	public:
		Weapon2D() {
		}

		Weapon2D(float damage, float timeBetweenShots, float range, int clipSize, mw::Sprite symbol, Animation animation, float size, Position grip) : Weapon(damage, timeBetweenShots, range, clipSize),
			symbol_(symbol), animation_(animation), grip_(grip), size_(size) {
		}

		void drawSymbol() override {
			symbol_.draw();
		}

		void draw() override {
			glPushMatrix();
			glTranslate2f(-grip_);
			glScale2f(size_);
			animation_.draw(0);
			glPopMatrix();
		}

		WeaponPtr clone() const override {
			return std::make_shared<Weapon2D>(*this);
		}

		void playShotSound() override {
			mw::Sound sound = shoot_;
			sound.play();
		}

		void playReloadSound() override {
			mw::Sound sound = reload_;
			sound.play();
		}

		void setReloadSound(const mw::Sound& sound) {
			reload_ = sound;
		}

		void setShotSound(const mw::Sound& sound) {
			shoot_ = sound;
		}		

	private:
		mw::Sprite symbol_;
		Animation animation_;
		float size_;
		Position grip_;
		mw::Sound shoot_;
		mw::Sound reload_;
	};

} // Namespace zombie.

#endif // WEAPON2D_H
