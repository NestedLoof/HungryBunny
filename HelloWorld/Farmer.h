#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Play2.h"
#include "Utilities.h"
#include "GameLogic.h"

class Farmer
{
public:

	//! @brief Creates the enemy object.
	static void InitialiseFarmers();
	//! @brief Updates all enemy characters (spawns 2 farmers in hungry mode).
	static void UpdateFarmers();
	//! @brief Updates the position of the farmer, bouncing off walls if necessary, then draws it.
	static void DrawFarmer(Play::GameObject& obj_farmer);
	//! @brief Updates the enemy character and redraws it.
	static void UpdateFarmer(Play::GameObject& obj_farmer);
	//! @brief Creates a bullet every time the farmer shoots.
	static void SpawnBulletIfNecessary(Play::GameObject& obj_farmer);
	//! @brief Spawns in a bullet and sets its velocity.
	//! @param addedBulletAngle Add an angle to the bullet's rotation, so can fire off in multiple directions in hard mode.
	static void SpawnBullet(Play::GameObject& obj_farmer, float addedBulletAngle = 0);
	//! @brief Calculates the bullet rotation based on the farmer's rotation, a random added angle and potentially an added angle due to multishoot.
	//! @param addedBulletAngle Add an angle to the bullet's rotation, so can fire off in multiple directions in hard mode.
	static float CalculateBulletRotation(Play::GameObject& obj_farmer, float addedBulletAngle);
	//! @brief Updates and draws the bullets. Detects if there are any collisions or if the bullets are leaving the display area.
	static void UpdateBullets();
	//! @brief Sets the farmer's action to either move or shoot.
	static void ChooseFarmerAction(Play::GameObject& obj_farmer);
	//! @brief Rotates the farmer to face the player.
	static void RotateFarmer(Play::GameObject& obj_farmer);
	//! @brief Recentre the farmer's sprite so that it it is centred on the character correctly.
	static void RecentreFarmerSprite(Play::GameObject& obj_farmer);
	//! @brief Normalises the farmer's velocity so that it moves at a constant speed even when moving diagonally.
	static void NormaliseFarmerVelocity(Play::GameObject& obj_farmer);

};

