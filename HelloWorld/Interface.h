#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Play2.h"
#include "Scores.h"
#include "GameLogic.h"

// contains all logic for menus and GUI elements
class Interface
{
public:
	//! @brief Creates the menu objects up front, but only displays them when the player dies.
	static void InitialiseMenus();
	//! @brief Create a menu button and add it to mapButtonNameToId to be used later.
	//! @param buttonName Name of the button to get the button object's ID from the map later.
	//! @param pos Position of the button on the screen.
	static void CreateMenuButton(std::string buttonName, Play::Point2D pos);
	//! @brief Displays the menu if it's game over and tracks button clicks, or displays the score if the player is still alive.
	static void DisplayInterface();
	//! @brief Displays the main menu where you select play.
	static void DisplayStartMenu();
	//! @brief Displays the interface while the player is still alive.
	static void DisplayGUI();
	//! @brief Displays the game over menu and tracks button clicks.
	static void DisplayGameOverMenu();
	//! @brief Displays the player's high scores, with a back button to return to the game over menu.
	static void DisplayHighScoresMenu();
	//!@brief Gets the difficulty string for the current difficulty to display on the screen.
	static std::string GetDifficultyString();
	//! @brief Holds the logic for drawing a button and checking if it was clicked on.
	//! @param obj_button The button object to display.
	//! @param buttonText Text to display on the button
	//! @param scale Optional param that scales the button text size.
	//! @return Returns true if the button is clicked.
	static bool DisplayButton(Play::GameObject& obj_button, std::string buttonText, float scale = 0.75);
	//! @brief Uses the object's position, sprite size and scale to check if it was clicked on by the player.
	//! @param obj Object to check if it was clicked on.
	//! @param button Mouse button to check if it was clicked (left or right).
	//! @return Returns true if the object was clicked on.
	static bool ObjectClickedOn(Play::GameObject& obj, Play::Input::MouseButton button);
	//! @brief Creates the health bar objects.
	static void InitialiseGui();
	//! @brief Updates player health and draws the health bar.
	static void UpdateHealthBar();
	//! @brief Draws the health bar on the screen.
	static void DrawHealthBar();
	//! @brief Calculates the opacity of the health bar since it starts to flash on low health. Also plays the low health sound.
	static float GetHealthBarOpacity();
	//! @brief Display the current score on the screen during gameplay.
	static void UpdateScoreDisplay();
	//! @brief Checks if the player's health should be depleted this frame based on their current action.
	//! @return Returns true if the health should be depleted.
	static bool ShouldDepleteHealth();
};

