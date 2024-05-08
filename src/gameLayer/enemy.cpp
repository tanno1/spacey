#include <enemy.h>
#include <tiledRenderer.h>

void Enemy::render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites, gl2d::TextureAtlasPadding& atlas)
{
	renderSpaceShip(renderer, position, enemyShipSize, sprites, atlas.get(type.x, type.y), viewDirection);
}

void Enemy::update(float deltaTime, glm::vec2 playerPos)
{
	// calculate direciton to player
	glm::vec2 directionToPlayer = playerPos - position;

	// if enemy is on player, change direction
	if (glm::length(directionToPlayer) == 0)
	{
		directionToPlayer = { 1, 0 };
	}
	else
	{
		directionToPlayer = glm::normalize(directionToPlayer);
	};

	// init new direction vector
	glm::vec2 newDirection = {};
	
	// if enemy is on player
	if (glm::length(directionToPlayer + viewDirection) <= 0.2)
	{
			if (rand() % 2)
			{
				newDirection = glm::vec2(directionToPlayer.y, -directionToPlayer.x);
			}
			else
			{
				newDirection = glm::vec2(-directionToPlayer.y, directionToPlayer.x);
			}
	}
	else
	{
		newDirection = deltaTime * turnSpeed * directionToPlayer + viewDirection;
	}

	float length = glm::length(newDirection);
	viewDirection = glm::normalize(newDirection);

	length = glm::clamp(length, 0.1f, 3.f);
	position += viewDirection * deltaTime * speed;

}