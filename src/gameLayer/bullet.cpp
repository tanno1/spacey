#include <bullet.h>

void Bullet::render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas)
{
	renderer.renderRectangle({ position + glm::vec2(25, 25), 15, 15 }, bulletsTexture, Colors_White, {}, 0, bulletsAtlas.get(1, 1));
}

void Bullet::update(float deltaTime)
{
	position += fireDirection * deltaTime  * 1500.f;
}