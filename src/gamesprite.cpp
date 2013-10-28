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
	const mw::Sprite treeSprite("images/zombie/objects/tree.png");

	const mw::Sprite bloodSprite("images/blood.png");
	const mw::Sprite carSprite("images/car.png");

} // Namespace zombie.
