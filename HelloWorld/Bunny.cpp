#include "Bunny.h"

void Bunny::InitialiseBunny()
{
	Play::CreateGameObject(TYPE_BUNNY, BUNNY_START_POS, 8, "bunny");
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	Play::SetSprite(obj_bunny, "bunny_downidle", BUNNY_IDLE_ANIMATION_SPEED);
}

void Bunny::UpdateBunny()
{
	switch (gameState.bunnyState)
	{
		case STATE_START_GAME:
		{
			// main menu, first screen in game
			Interface::DisplayStartMenu();
			break;
		}
		case STATE_PLAY:
		{
			// keep playing the game by handling inputs
			HandleControls();
			DrawBunny();
			break;
		}
		case STATE_DEAD:
		{
			// press space to restart the game
			Interface::DisplayInterface();
			DrawBunny();
			break;
		}
	}
}

void Bunny::HandleControls()
{
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	int currentSpriteId = obj_bunny.spriteId;
	std::string currentSpriteName = Play::GetSpriteName(currentSpriteId);

	// don't allow the player to move if they are watering the plants
	if (currentSpriteName.find("WATER") != std::string::npos)
	{
		// once the animation is complete, change the animation and spawn in some flowers
		if (Play::IsAnimationComplete(obj_bunny))
		{
			FinishWateringFlowers(obj_bunny, currentSpriteName);
		}
	}
	// watering can animation
	else if (Play::KeyDown(Play::KEY_SPACE))
	{
		DoBunnyWateringAnimation(obj_bunny, currentSpriteName);
	}
	// running, walking, or idle animations
	else
	{
		HandleBunnyMovement(obj_bunny, currentSpriteName);
	}
}

void Bunny::FinishWateringFlowers(Play::GameObject& obj_bunny, std::string currentSpriteName)
{
	// stop the watering sound effect
	Play::StopAudio("bubble");

	// to plant the flowers at the correct angles, calculate the angle between the flowers
	// by combining the max angle on both sides and diving by the total flowers (not counting the starting one)
	float maxAngle = Play::PLAY_PI / 4;
	if (FLOWER_CIRCLE)
	{
		// special power in hungry mode, flowers spawn in a circle around the player
		maxAngle = Play::PLAY_PI;
	}
	float angleFlowerSeparation = maxAngle * 2 / (NUM_FLOWERS_GROW - 1);

	// check what direction the player was facing and set the correct idle animation
	if (currentSpriteName.find("UP") != std::string::npos)
	{
		// set up idle animation
		Play::SetSprite(obj_bunny, "bunny_upidle", BUNNY_IDLE_ANIMATION_SPEED);

		// create all the flowers
		for (float i = -maxAngle; i <= maxAngle; i += angleFlowerSeparation)
		{
			int id = Play::CreateGameObject(TYPE_FLOWER, obj_bunny.pos + (Play::Point2D{ sin(i), cos(i) } * 16), 8, "sunflower");

			// keep track of the order the flowers were created so that we can destroy the oldest ones if we go over the limit
			FLOWER_OBJECT_ID_TO_COUNT.push_back({ id, FLOWER_COUNTER });
			FLOWER_COUNTER++;
		}
	}
	else if (currentSpriteName.find("DOWN") != std::string::npos)
	{
		// set down idle animation
		Play::SetSprite(obj_bunny, "bunny_downidle", BUNNY_IDLE_ANIMATION_SPEED);

		// create all the flowers
		for (float i = -maxAngle; i <= maxAngle; i += angleFlowerSeparation)
		{
			int id = Play::CreateGameObject(TYPE_FLOWER, obj_bunny.pos + (Play::Point2D{ -sin(i), -cos(i) } *16), 8, "sunflower");

			// keep track of the order the flowers were created so we can destroy the oldest ones if we go over the limit
			FLOWER_OBJECT_ID_TO_COUNT.push_back({ id, FLOWER_COUNTER });
			FLOWER_COUNTER++;
		}
	}
	else if (currentSpriteName.find("LEFT") != std::string::npos)
	{
		// set left idle animation
		Play::SetSprite(obj_bunny, "bunny_leftidle", BUNNY_IDLE_ANIMATION_SPEED);

		// create all the flowers
		for (float i = -maxAngle; i <= maxAngle; i += angleFlowerSeparation)
		{
			int id = Play::CreateGameObject(TYPE_FLOWER, obj_bunny.pos + (Play::Point2D{ -cos(i), -sin(i) } *16), 8, "sunflower");

			// keep track of the order the flowers were created so we can destroy the oldest ones if we go over the limit
			FLOWER_OBJECT_ID_TO_COUNT.push_back({ id, FLOWER_COUNTER });
			FLOWER_COUNTER++;
		}
	}
	else if (currentSpriteName.find("RIGHT") != std::string::npos)
	{
		// set right idle animation
		Play::SetSprite(obj_bunny, "bunny_rightidle", BUNNY_IDLE_ANIMATION_SPEED);

		// create all the flowers
		for (float i = -maxAngle; i <= maxAngle; i += angleFlowerSeparation)
		{
			int id = Play::CreateGameObject(TYPE_FLOWER, obj_bunny.pos + (Play::Point2D{ cos(i), sin(i) } *16), 8, "sunflower");

			// keep track of the order the flowers were created so we can destroy the oldest ones if we go over the limit
			FLOWER_OBJECT_ID_TO_COUNT.push_back({ id, FLOWER_COUNTER });
			FLOWER_COUNTER++;
		}
	}
}

void Bunny::HandleBunnyMovement(Play::GameObject& obj_bunny, std::string currentSpriteName)
{
	// run animations
	if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_UP) || Play::KeyDown(Play::KEY_W)))
	{
		obj_bunny.velocity = { 0, BUNNY_RUN_SPEED };
		Play::SetSprite(obj_bunny, "bunny_uprun", BUNNY_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_DOWN) || Play::KeyDown(Play::KEY_S)))
	{
		obj_bunny.velocity = { 0, -BUNNY_RUN_SPEED };
		Play::SetSprite(obj_bunny, "bunny_downrun", BUNNY_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_LEFT) || Play::KeyDown(Play::KEY_A)))
	{
		obj_bunny.velocity = { -BUNNY_RUN_SPEED, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftrun", BUNNY_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_RIGHT) || Play::KeyDown(Play::KEY_D)))
	{
		obj_bunny.velocity = { BUNNY_RUN_SPEED, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightrun", BUNNY_RUN_ANIMATION_SPEED);
	}
	// walk animations
	else if (Play::KeyDown(Play::KEY_UP) || Play::KeyDown(Play::KEY_W))
	{
		obj_bunny.velocity = { 0, BUNNY_WALK_SPEED };
		Play::SetSprite(obj_bunny, "bunny_upwalk", BUNNY_WALK_ANIMATION_SPEED);

	}
	else if (Play::KeyDown(Play::KEY_DOWN) || Play::KeyDown(Play::KEY_S))
	{
		obj_bunny.velocity = { 0, -BUNNY_WALK_SPEED };
		Play::SetSprite(obj_bunny, "bunny_downwalk", BUNNY_WALK_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_LEFT) || Play::KeyDown(Play::KEY_A))
	{
		obj_bunny.velocity = { -BUNNY_WALK_SPEED, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftwalk", BUNNY_WALK_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_RIGHT) || Play::KeyDown(Play::KEY_D))
	{
		obj_bunny.velocity = { BUNNY_WALK_SPEED, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightwalk", BUNNY_WALK_ANIMATION_SPEED);
	}
	// idle animations
	// check what direction the player was facing and set the correct idle animation
	else if (currentSpriteName.find("UP") != std::string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_upidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("DOWN") != std::string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_downidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("LEFT") != std::string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("RIGHT") != std::string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
}

void Bunny::DoBunnyWateringAnimation(Play::GameObject& obj_bunny, std::string currentSpriteName)
{
	// check what direction the player was facing and set the correct water animation
	if (currentSpriteName.find("UP") != std::string::npos)
	{
		Play::SetSprite(obj_bunny, "bunny_upwater", BUNNY_WATER_ANIMATION_SPEED);
	}
	else if ((currentSpriteName.find("DOWN") != std::string::npos))
	{
		Play::SetSprite(obj_bunny, "bunny_downwater", BUNNY_WATER_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("LEFT") != std::string::npos)
	{
		Play::SetSprite(obj_bunny, "bunny_leftwater", BUNNY_WATER_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("RIGHT") != std::string::npos)
	{
		Play::SetSprite(obj_bunny, "bunny_rightwater", BUNNY_WATER_ANIMATION_SPEED);
	}

	// stand still while watering
	obj_bunny.velocity = { 0, 0 };

	// start the watering sound effect
	Play::StartAudioLoop("bubble");
	Play::Audio::SetLoopingSoundVolume("bubble", 0.5);
}

void Bunny::DrawBunny()
{
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	Play::UpdateGameObject(obj_bunny);
	Play2::RevertPositionIfNecessary(obj_bunny);
	Play::DrawObjectRotated(obj_bunny);
}