#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Windows.h"

int DISPLAY_WIDTH = 640;
int DISPLAY_HEIGHT = 368;
int DISPLAY_SCALE = 1;

// audio volumes
float MUSIC_VOLUME = 0.1f;

// animation speeds - lower is slower
float waterAnimationSpeed = 0.15f;
float runAnimationSpeed = 0.25f;
float walkAnimationSpeed = 0.15f;
float idleAnimationSpeed = 0.1f;

// player speeds
int runSpeed = 2;
int walkSpeed = 1;

struct GameState
{
	int score{ 0 };
};
GameState gameState;

enum GameObjectType
{
	TYPE_NULL,
	TYPE_BUNNY, // the player character
	TYPE_DESTROYED, // all objects of this type are destroyed at the end of each MainGameUpdate/tick
};

//! @brief Updates the player character and redraws it.
void UpdatePlayer();
//! @brief Updates the player character based on user input.
void HandleControls();
//! @brief Scales up the game's display size depending on the user's current screen resolution.
void InitDisplaySize();

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	InitDisplaySize();
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();

	Play::LoadBackground("Data\\Backgrounds\\grassbackgroundTEST.png");
	Play::StartAudioLoop("music");
	Play::Audio::SetLoopingSoundVolume("music", MUSIC_VOLUME);


	Play::CreateGameObject(TYPE_BUNNY, { DISPLAY_WIDTH/2, 24 }, 8, "bunny");
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	Play::SetSprite(obj_bunny, "bunny_downidle", idleAnimationSpeed);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	HandleControls();
	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

void UpdatePlayer()
{
	
}

void HandleControls()
{
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);


	// TODO - only do the water animation once, then go back to the idle animation in the same direction
	// TODO - do not allow the player to move while watering
	// TODO - tidy up the code / refactor

	// water animations
	if (Play::KeyDown(Play::KEY_SPACE))
	{
		// check what direction the player is facing and set the correct water animation
		int currentSpriteId = obj_bunny.spriteId;
		string currentSpriteName = Play::GetSpriteName(currentSpriteId);
		
		if (Play::KeyDown(Play::KEY_UP)
			|| Play::KeyDown(Play::KEY_W))
		{
			Play::SetSprite(obj_bunny, "bunny_upwater", waterAnimationSpeed);
		}
		else if (Play::KeyDown(Play::KEY_DOWN)
			|| Play::KeyDown(Play::KEY_S))
		{
			Play::SetSprite(obj_bunny, "bunny_downwater", waterAnimationSpeed);
		}
		else if (Play::KeyDown(Play::KEY_LEFT)
			|| Play::KeyDown(Play::KEY_A))
		{
			Play::SetSprite(obj_bunny, "bunny_leftwater", waterAnimationSpeed);
		}
		else if (Play::KeyDown(Play::KEY_RIGHT)
			|| Play::KeyDown(Play::KEY_D))
		{
			Play::SetSprite(obj_bunny, "bunny_rightwater", waterAnimationSpeed);
		}
		else if (currentSpriteName.find("UP") != std::string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_upwater", waterAnimationSpeed);
		}
		else if ((currentSpriteName.find("DOWN") != std::string::npos))
		{
			Play::SetSprite(obj_bunny, "bunny_downwater", waterAnimationSpeed);
		}
		else if (currentSpriteName.find("LEFT") != std::string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_leftwater", waterAnimationSpeed);
		}
		else if (currentSpriteName.find("RIGHT") != std::string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_rightwater", waterAnimationSpeed);
		}

		//stand still while watering
		obj_bunny.velocity = { 0, 0 };
	}
	// run animations
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_UP) || Play::KeyDown(Play::KEY_W)))
	{
		obj_bunny.velocity = { 0, runSpeed };
		Play::SetSprite(obj_bunny, "bunny_uprun", runAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_DOWN) || Play::KeyDown(Play::KEY_S)))
	{
		obj_bunny.velocity = { 0, -runSpeed };
		Play::SetSprite(obj_bunny, "bunny_downrun", runAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_LEFT) || Play::KeyDown(Play::KEY_A)))
	{
		obj_bunny.velocity = { -runSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftrun", runAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_RIGHT) || Play::KeyDown(Play::KEY_D)))
	{
		obj_bunny.velocity = { runSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightrun", runAnimationSpeed);
	}
	// walk animations
	else if (Play::KeyDown(Play::KEY_UP) || Play::KeyDown(Play::KEY_W))
	{
		obj_bunny.velocity = { 0, walkSpeed };
		Play::SetSprite(obj_bunny, "bunny_upwalk", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_DOWN) || Play::KeyDown(Play::KEY_S))
	{
		obj_bunny.velocity = { 0, -walkSpeed };
		Play::SetSprite(obj_bunny, "bunny_downwalk", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_LEFT) || Play::KeyDown(Play::KEY_A))
	{
		obj_bunny.velocity = { -walkSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftwalk", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_RIGHT) || Play::KeyDown(Play::KEY_D))
	{
		obj_bunny.velocity = { walkSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightwalk", walkAnimationSpeed);
	}
	// idle animations
	else if (obj_bunny.velocity.x < 0)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftidle", idleAnimationSpeed);
	}
	else if (obj_bunny.velocity.x > 0)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightidle", idleAnimationSpeed);
	}
	else if (obj_bunny.velocity.y > 0)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_upidle", idleAnimationSpeed);
	}
	else if (obj_bunny.velocity.y < 0)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_downidle", idleAnimationSpeed);
	}

	// update and draw the player
	Play::UpdateGameObject(obj_bunny);
	if (Play::IsHitboxLeavingDisplayArea(obj_bunny))
	{
		obj_bunny.pos = obj_bunny.oldPos;
	}
	Play::DrawObjectRotated(obj_bunny);
}

// scales up the game if the player's screen is large enough
void InitDisplaySize()
{
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();

	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates for the screen size
	RECT desktop;
	GetWindowRect(hDesktop, &desktop);
	int x = desktop.right;
	int y = desktop.bottom;

	// make the game bigger if the screen resolution is large enough. Leaving extra room so game isn't maximised.
	int extraRoom = 20;
	if (x > (DISPLAY_WIDTH * 4) + extraRoom
		&& y > (DISPLAY_HEIGHT * 4) + extraRoom)
	{
		DISPLAY_SCALE = 4;
	}
	else if (x > (DISPLAY_WIDTH * 3) + extraRoom
		&& y > (DISPLAY_HEIGHT * 3) + extraRoom)
	{
		DISPLAY_SCALE = 3;
	}
	else if (x > (DISPLAY_WIDTH * 2) + extraRoom
		&& y > (DISPLAY_HEIGHT * 2) + extraRoom)
	{
		DISPLAY_SCALE = 2;
	}
}

