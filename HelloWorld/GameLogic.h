#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Scores.h"
#include "Play.h"
#include "Play2.h"

// contains gameplay logic and environment
class GameLogic
{
public:
	//! @brief Reset or destroy all objects in the game to start again.
	static void RestartGame(Difficulty difficulty);
	//! @brief Destroys all objects of the given type
	static void DestroyAllObjectsForType(GameObjectType type);
	//! @brief Sets the gameplay variables based on the selected difficulty.
	static void InitialiseDifficulty();
	//! @brief Sorts out all of the player and game state variables to end the game when the player dies.
	static void GameOver();

	//! @brief Spawns in any carrots or other veg if necessary.
	static void SpawnVegetables();
	//! @brief Added random element to determine if a vegetable should spawn when the number of veg falls below the max.
	static bool ShouldSpawnVegetable();
	//! @brief Gets a randomised valid location for the next carrot to grow.
	static Play::Point2D GetNewVegetablePosition();
	//! @brief Detects if a carrot is being eaten and draws all carrots.
	static void UpdateCarrots();
	//! @brief Updates and draws the flowers that can block bullets (temporary walls)
	static void UpdateFlowers();
	//! @brief Destroy the oldest flowers if there are too many, so we don't lag and the player can't have infinite bullet blockers making it too easy.
	static void DestroyOldestFlowers();
	//! @brief Destroys all objects set to be destroyed.
	static void UpdateDestroyed();
	//! @brief Initialises the environment objects where the characters can't walk.
	static void InitialiseEnvironment();
	//! @brief Initialises the map of plots where carrots can spawn.
	static void InitialisePlots();
};

