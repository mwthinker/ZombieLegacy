#include "gamesprite.h"

#include <mw/sprite.h>
#include <mw/texture.h>

namespace zombie {

	const mw::TexturePtr grassTexture(new mw::Texture("images/grass05.png", [](){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}));

	const mw::Sprite grassSprite(grassTexture);
	const mw::Sprite roadSprite("images/roadtile2.jpg");

	const mw::Sprite zombie1Sprite("images/zombie/zombie1.png");
	const mw::Sprite zombie2Sprite("images/zombie/zombie2.png");
	const mw::Sprite zombie3Sprite("images/zombie/zombie3.png");
	const mw::Sprite zombie4Sprite("images/zombie/zombie4.png");
	const mw::Sprite zombie5Sprite("images/zombie/zombie5.png");
	const mw::Sprite zombie6Sprite("images/zombie/zombie6.png");

	const mw::Sprite human1Sprite("images/human/human1.png");
	const mw::Sprite human2Sprite("images/human/human2.png");
	const mw::Sprite human3Sprite("images/human/human3.png");

	const mw::Sprite bloodSprite("images/blood.png");
	const mw::Sprite carSprite("images/car.png");

} // Namespace zombie.
