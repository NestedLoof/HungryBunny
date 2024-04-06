#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Utilities.h"
#include "Play2.h"
#include "Interface.h"

// handles the player character
class Bunny
{
public:

	//! @brief Creates the player character object.
	static void InitialiseBunny();
	//! @brief Handles the game state and updates the main character accordingly.
	static void UpdateBunny();
	//! @brief Draw the player character.
	static void DrawBunny();
	//! @brief Updates the player character based on key presses.
	static void HandleControls();
	//! @brief Stops the watering sound and spawns flowers.
	static void FinishWateringFlowers(Play::GameObject& obj_bunny, std::string currentSpriteName);
	//! @brief Sets the velocity and sprites for run, walk and idle.
	static void HandleBunnyMovement(Play::GameObject& obj_bunny, std::string currentSpriteName);
	//! @brief Performs the watering animation and starts the watering sound.
	static void DoBunnyWateringAnimation(Play::GameObject& obj_bunny, std::string currentSpriteName);
};

