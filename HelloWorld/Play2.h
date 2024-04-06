#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Utilities.h"

// contains some utility functions
class Play2
{
public:
	// draws a font and allows specifying the scale of the text.
	static void DrawFontTextWithScale(const char* fontId, std::string text, Play::Point2D pos, Play::Align justify, float scale);
	static void DrawStringWithScale(int fontId, Play::Point2f pos, std::string text, float scale);

	// modified version of IsLeavingDisplayArea from Play.h that uses the collision radius instead of the sprite size.
	static bool IsHitboxLeavingDisplayArea(Play::GameObject& obj);

	//! @brief Reverts the player's position if they are colliding with an environment object or the leaving the display area.
	//! @param object The GameObject to check for collision with environment objects or the display boundaries.
	//! @return Returns true if the object reverted its position.
	static bool RevertPositionIfNecessary(Play::GameObject& object);
	//! @brief Reverts the player's position if they are colliding with an environment object or the leaving the display area.
	//! @param object The GameObject to check for collision with environment objects.
	//! @return Returns true if the object is colliding with any environment objects.
	static bool HasBlockingCollision(Play::GameObject& object);

	//! @brief Scales up the game's display size depending on the user's current screen resolution.
	static void InitDisplaySize();
};

