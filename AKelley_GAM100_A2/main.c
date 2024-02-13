//---------------------------------------------------------
// file:	main.c
// author:	Alexander Kelley
// email:	alex.kelley@digipen.edu
//
// brief:	Main entry point for the sample project
//			of the CProcessing library
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"

float playerPosX = 0;
float playerPosY = 0;
int score = 0;

// shooting variables
int bullet_active[10];
float bullet_posX[10];
float bullet_posY[10];
float bulletSpeed = 10;
float bulletTimer = 1;
float playerFireRate = 3;

// stars
struct STAR {
	int isActive;
	float speed;
	float rotation;
	float rotationSpeed;
	float posX;
	float posY;
	int transparency;
	float size;
};
float timeLastStar = 0;
struct STAR stars[20];

// enemies
struct ENEMY {
	int isActive;
	float speed;
	float posX;
	float posY;
};
float timeLastEnemy = 0;
struct ENEMY enemies[10];

void drawStars(void);
void playerController(void);
void drawPlayer(float posX, float posY);
void drawBullet(float posX, float posY);
void printScore(void);
void shoot(void);
void clearBulletIndex(int i);
void collisionController(void);
void clearEnemyIndex(int i);
void addScore(int scoreToAdd);
void enemyController(void);
void drawEnemies(void);

void game_init(void)
{
	// clear stars array
	for (int i = 0; i < 20; i++) {
		stars[i].isActive = 0;
	}
	//clear bullet array
	for (int i = 0; i < 10; i++) {
		bullet_active[i] = 0;
	}
	//clear enemy array
	for (int i = 0; i < 10; i++) {
		enemies[i].isActive = 0;
	}
	
	score = 0;
	playerPosX = 0;
	playerPosY = 500;
	bulletTimer = 1;
	timeLastStar = 0;
	timeLastEnemy = 0;
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_System_SetWindowSize(800, 600);
}

// use CP_Engine_SetNextGameState to specify this function as the update function
void game_update(void)
{
	// CLEAR BACKGROUND =========================
	CP_Color backgroundColor = (CP_Color_Create(0, 0, 0, 255));
	CP_Graphics_ClearBackground(backgroundColor);


	// GET DELTA T ===============================
	float currentElapsedTime = CP_System_GetDt();
	static float totalElapsedTime = 0;
	bulletTimer += currentElapsedTime;
	timeLastStar += currentElapsedTime;
	timeLastEnemy += currentElapsedTime;

	// GAME LOGIC ================================
	
	playerController();
	enemyController();

	if (CP_Input_MouseDown(MOUSE_BUTTON_1)) {
		shoot();
	}
	if (CP_Input_KeyTriggered(KEY_R))
		CP_Engine_SetNextGameStateForced(game_init, game_update, NULL);
	
	// COLLISIONS ================================

	collisionController();

	// RENDER ====================================
	
	drawStars();
	drawEnemies();
	drawPlayer(playerPosX, playerPosY);

	for (int i = 0; i < 10; ++i) {
		if (bullet_active[i] == 1) {
			drawBullet(bullet_posX[i], bullet_posY[i] -= bulletSpeed);
			if (bullet_posY[i] < 0.f)
				clearBulletIndex(i);
		}
	}
	// restart print line
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextSize(20.0f);
	char buffer[25] = { 0 };
	sprintf_s(buffer, _countof(buffer), "Press ""R"" to Restart");
	CP_Font_DrawText(buffer, 50, 125);
	
	printScore();
}

// use CP_Engine_SetNextGameState to specify this function as the exit function
// this function will be called once just before leaving the current gamestate
void game_exit(void)
{
	// shut down the gamestate and cleanup any dynamic memory
}

// main() the starting point for the program
// CP_Engine_SetNextGameState() tells CProcessing which functions to use for init, update and exit
// CP_Engine_Run() is the core function that starts the simulation
int main(void)
{
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
	CP_Engine_Run();
	return 0;
}

void playerController(void) {
	float edgeOffset = 35.f;

	if (CP_Input_GetMouseX() < edgeOffset)
		playerPosX = edgeOffset;
	else if (CP_Input_GetMouseX() > (CP_System_GetWindowWidth() - edgeOffset))
		playerPosX = CP_System_GetWindowWidth() - edgeOffset;
	else
		playerPosX = CP_Input_GetMouseX();
}

void drawPlayer(float posX, float posY)
{
	CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
	CP_Graphics_DrawTriangle(posX - 30.f, posY + 15.f, posX + 30.f, posY + 15.f, posX, posY - 15.f);
	CP_Graphics_DrawRect(posX, posY, 20.f, 50.f);
}


void printScore(void)
{
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextSize(30.0f);
	char buffer[50] = { 0 };
	sprintf_s(buffer, _countof(buffer), "SCORE: %i", score);
	CP_Font_DrawText(buffer, 50, 100);
}


void shoot(void)
{
	//float currentElapsedTime = CP_System_GetDt();
	//static float totalElapsedTime = 0;
	//totalElapsedTime += currentElapsedTime;

	if (bulletTimer > (1/playerFireRate) ) {
		// find empty index and assign it
		for (int i = 0; i < 10; ++i) {
			if (bullet_active[i] == 0) {
				bullet_active[i] = 1;
				bullet_posX[i] = playerPosX;
				bullet_posY[i] = 460.f;
				break;
			}

		}
		bulletTimer = 0;
	}

}

void addScore(int scoreToAdd) {
	score += scoreToAdd;
}

void drawBullet(float posX, float posY) {
	CP_Settings_Fill(CP_Color_Create(100, 200, 150, 255));
	CP_Graphics_DrawCircle(posX, posY, 10.f);
}


void clearBulletIndex(int i) {
	bullet_active[i] = 0;
}
void clearEnemyIndex(int i) {
	enemies[i].isActive = 0;
}

void drawStars(void) {
	for (int i = 0; i < 20; ++i) {
		if (stars[i].isActive) {
			stars[i].posX += CP_Input_GetMouseDeltaX() * -(stars[i].size)*0.5f;
			stars[i].posY += stars[i].speed;
			CP_Settings_NoStroke();
			CP_Settings_Fill(CP_Color_Create(255, 255, 255, stars[i].transparency));
			CP_Graphics_DrawTriangleAdvanced(stars[i].posX, stars[i].posY - (10.f* stars[i].size), stars[i].posX - (10.f * stars[i].size), stars[i].posY + (10.f * stars[i].size), stars[i].posX + (10.f * stars[i].size), stars[i].posY + (10.f * stars[i].size), stars[i].rotation += stars[i].rotationSpeed);

			// delete stars outside of window
			if (stars[i].posY > CP_System_GetWindowHeight()) {
				stars[i].isActive = 0;
			}
		}
		// create new stars
		else if (timeLastStar > 0.5f) {
			float sizeMult = CP_Random_RangeFloat(0.1f, 1.f);
			stars[i].isActive = 1;
			stars[i].posX = CP_Random_RangeFloat(0, (float)CP_System_GetWindowWidth());
			stars[i].posY = 0;
			stars[i].rotation = 0;
			stars[i].rotationSpeed = CP_Random_RangeFloat(1.f, 5.f);
			stars[i].speed = sizeMult * CP_Random_RangeFloat(3.f, 5.f);
			stars[i].transparency = CP_Random_RangeInt(50, 110);
			stars[i].size = sizeMult * CP_Random_RangeFloat(0.3f, 0.5f);
			timeLastStar = 0;
		}

	}
}

void collisionController(void) {
	// check bullets against enemies
	for (int i = 0; i < 10; i++) {
		if (bullet_active[i]) {
			for (int j = 0; j < 10; j++) {
				if (enemies[j].isActive) {
					if (CP_Math_Distance(bullet_posX[i], bullet_posY[i], enemies[j].posX, enemies[j].posY) <= 30.f) {
						clearBulletIndex(i);
						clearEnemyIndex(j);
						addScore(50);
					}
				}
			}
		}
	}
}
void enemyController(void) {
	for (int i = 0; i < 10; i++) {
		if (enemies[i].isActive) {
			enemies[i].posY += enemies[i].speed;
			if (enemies[i].posY > (float)CP_System_GetWindowHeight()) {
				clearEnemyIndex(i);
				addScore(-25);
			}
		}
		else if (timeLastEnemy > 0.50f) {
			enemies[i].isActive = 1;
			enemies[i].posX = CP_Random_RangeFloat(50.f, (float)CP_System_GetWindowWidth()-50.f);
			enemies[i].posY = -10;
			enemies[i].speed = CP_Random_RangeFloat(10.f, 15.f);
			timeLastEnemy = 0;
		}

	}
}

void drawEnemies(void) {
	for (int i = 0; i < 10; i++) {
		if (enemies[i].isActive) {
			CP_Settings_Fill(CP_Color_Create(212, 21, 208, 255));
			CP_Graphics_DrawCircle(enemies[i].posX, enemies[i].posY, 30.f);
		}
	}
}