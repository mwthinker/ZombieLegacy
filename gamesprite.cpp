#include "gamesprite.h"

#include <mw/sprite.h>
#include <mw/texture.h>

namespace zombie {

	const mw::TexturePtr grassTexture(new mw::Texture("images/grasstile.jpg"));
	const mw::Sprite drawGrass(grassTexture);
	const mw::Sprite drawRoad("images/roadtile2.jpg");
	
	const mw::Sprite zombie1("images/zombie/zombie1.png");
	const mw::Sprite zombie2("images/zombie/zombie2.png");
	const mw::Sprite zombie3("images/zombie/zombie3.png");
	const mw::Sprite zombie4("images/zombie/zombie4.png");
	const mw::Sprite zombie5("images/zombie/zombie5.png");
	const mw::Sprite zombie6("images/zombie/zombie6.png");

	const mw::Sprite human1("images/human/human1.png");
	const mw::Sprite human2("images/human/human2.png");
	const mw::Sprite human3("images/human/human3.png");

	const mw::Sprite blood("images/blood.png");
	const mw::Sprite car("images/car.png");

} // Namespace zombie.