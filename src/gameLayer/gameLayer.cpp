#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <tiledRenderer.h>
#include <bullet.h>
#include <vector>
#include <enemy.h>>

struct GameplayData
{
	glm::vec2 playerPos = { 100, 100 };

	std::vector<Bullet> bullets;

	std::vector<Enemy> enemies;
};

GameplayData data;

gl2d::Renderer2D renderer;

constexpr int BACKGROUNDS = 4;

gl2d::Texture spaceShipTexture;
gl2d::TextureAtlasPadding spaceShipAtlas;

gl2d::Texture bulletsTexture;
gl2d::TextureAtlasPadding bulletsAtlas;

gl2d::Texture backgroundTexture[BACKGROUNDS];
TiledRenderer tiledRenderer[BACKGROUNDS];

void restartGame()
{
	data = {};
	renderer.currentCamera.follow(data.playerPos, 550, 0, 0, renderer.windowW, renderer.windowH);
}

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	spaceShipTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "spaceship/stitchedFiles/spaceships.png", 128, true);
	spaceShipAtlas = gl2d::TextureAtlasPadding(5, 2, spaceShipTexture.GetSize().x, spaceShipTexture.GetSize().y);

	bulletsTexture.loadFromFileWithPixelPadding(RESOURCES_PATH "spaceship/stitchedFiles/projectiles.png", 500, true);
	bulletsAtlas = gl2d::TextureAtlasPadding(3, 2, bulletsTexture.GetSize().x, bulletsTexture.GetSize().y);

	backgroundTexture[0].loadFromFile(RESOURCES_PATH "background1.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "background2.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "background3.png", true);
	backgroundTexture[3].loadFromFile(RESOURCES_PATH "background4.png", true);

	tiledRenderer[0].texture = backgroundTexture[0];
	tiledRenderer[1].texture = backgroundTexture[1];
	tiledRenderer[2].texture = backgroundTexture[2];
	tiledRenderer[3].texture = backgroundTexture[3];

	tiledRenderer[0].paralaxStrength = 0;
	tiledRenderer[1].paralaxStrength = .5;
	tiledRenderer[2].paralaxStrength = .7;

	restartGame();

	return true;
}


bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion

#pragma region movement

	glm::vec2 move = {};

	if (
		platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up)
		)
	{
		move.y = -1;
	}
	if (
		platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down)
		)
	{
		move.y = 1;
	}
	if (
		platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left)
		)
	{
		move.x = -1;
	}
	if (
		platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right)
		)
	{
		move.x = 1;
	}

	if (move.x != 0 || move.y != 0)
	{
		move = glm::normalize(move);
		move *= deltaTime * 500; // speed
		data.playerPos += move;
		// std::cout << "X, Y:" << move.x << move.y << std::endl;
	}
#pragma endregion

#pragma region follow
	
	renderer.currentCamera.follow(data.playerPos, deltaTime * 300, 10, 200, w, h);

#pragma endregion

#pragma region render background

	for (int i = 0; i < BACKGROUNDS; i++)
	{
		tiledRenderer[i].render(renderer);
	}
#pragma endregion

#pragma region mouse pos

	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	glm::vec2 mouseDirection = mousePos - screenCenter;

	if (glm::length(mouseDirection) == 0.f)
	{
		mouseDirection = { 1, 0 };
	}
	else
	{
		mouseDirection = normalize(mouseDirection);
	}
	// convert direction to angle for ship
	float spaceShipAngle = atan2(mouseDirection.y, -mouseDirection.x);

#pragma endregion

#pragma region handle bullets

	if (platform::isLMousePressed())
	{
		Bullet b;
		b.position = data.playerPos;
		b.fireDirection = mouseDirection;

		data.bullets.push_back(b);
	}

	for (int i = 0; i < data.bullets.size(); i++)
	{
		if (glm::distance(data.bullets[i].position, data.playerPos) > 5'000)
		{
			data.bullets.erase(data.bullets.begin() + i);
			i--;
			continue;
		}

		data.bullets[i].update(deltaTime);

	}

#pragma endregion

#pragma region handle enemies

	for (int i = 0; i < data.enemies.size(); i++)
	{
		if (glm::distance(data.playerPos, data.enemies[i].position) > 4000.f)
		{
			data.enemies.erase(data.enemies.begin() + i);
			i--;
			continue;
		}

		if (data.enemies[i].update(deltaTime, data.playerPos))
		{
			Bullet b;
			b.position = data.enemies[i].position;
			b.fireDirection = data.enemies[i].viewDirection;
			// todo speed
			data.bullets.push_back(b);
		}

	}

#pragma endregion

#pragma region render enemies

	for (auto& e : data.enemies)
	{
		e.render(renderer, spaceShipTexture, spaceShipAtlas);
	}

#pragma endregion



#pragma region render ship

	constexpr float shipSize = 50.f;

	renderSpaceShip(renderer, data.playerPos, shipSize, spaceShipTexture, spaceShipAtlas.get(3, 0), mouseDirection);

	/*renderer.renderRectangle({ data.playerPos - glm::vec2(shipSize/2, shipSize/2), shipSize, shipSize}, spaceShipTexture, Colors_White, {}, glm::degrees(spaceShipAngle) + 90, spaceShipAtlas.get(1, 0));*/

#pragma endregion

#pragma region render bullets

	for (auto& b : data.bullets)
	{
		b.render(renderer, bulletsTexture, bulletsAtlas);
	}

#pragma endregion


	renderer.flush(); // tell gpu compute everything
	
	//ImGui::ShowDemoWindow();
	ImGui::Begin("debug");

	ImGui::Text("Bullets Count: %d", (int)data.bullets.size());
	ImGui::Text("Enemies Count: %d", (int)data.enemies.size());

	if (ImGui::Button("Spawn Enemy"))
	{
		glm::uvec2 shipTypes[] = { {0, 0}, {0, 1}, {2, 0}, {3, 1} };

		Enemy e;
		e.position = data.playerPos;

		e.speed = 350 + rand() % 100;
		e.turnSpeed = 2.f + (rand() % 1000) / 500.f;
		e.type = shipTypes[rand() % 4];
		e.fireRange = 1.5 + (rand() % 1000) / 2000.f;
		e.fireTimeReset = 0.1 + (rand() % 1000) / 500;
		data.enemies.push_back(e);
	}

	if (ImGui::Button("Reset Game"))
	{
		restartGame();
	}

	ImGui::End();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
