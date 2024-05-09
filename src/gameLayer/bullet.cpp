#include <bullet.h>

void Bullet::render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas)
{
	float spaceShipAngle = atan2(fireDirection.y, -fireDirection.x);
	spaceShipAngle = glm::degrees(spaceShipAngle) + 90.f;

	glm::vec4 textureCoords = bulletsAtlas.get(1, 1);
	if (isEnemy)
	{
		textureCoords = bulletsAtlas.get(0, 0);
	}

	renderer.renderRectangle({ position + glm::vec2(0, -12), 30, 30 }, bulletsTexture, Colors_White, {}, spaceShipAngle, textureCoords);
}

void Bullet::update(float deltaTime)
{
	position += fireDirection * deltaTime  * 1500.f;
}