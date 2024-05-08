#pragma once
#include <gl2d/gl2d.h>

constexpr float enemyShipSize = 50.f;

struct Enemy
{
	glm::ivec2 type = {};		// used to index in texture atlas
	glm::vec2 position = {};

	glm::vec2 viewDirection = { 1, 0 };

	void render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites, gl2d::TextureAtlasPadding& atlas);
};