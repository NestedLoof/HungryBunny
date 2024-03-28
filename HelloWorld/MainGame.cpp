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
float BUNNY_WATER_ANIMATION_SPEED = 0.15f;
float BUNNY_RUN_ANIMATION_SPEED = 0.25f;
float BUNNY_WALK_ANIMATION_SPEED = 0.15f;
float BUNNY_IDLE_ANIMATION_SPEED = 0.1f;
float FARMER_SHOOT_ANIMATION_SPEED = 0.2f;
float FARMER_RUN_ANIMATION_SPEED = 0.2f;
float FARMER_IDLE_ANIMATION_SPEED = 0.1f;
float ENVIRONMENT_WATER_ANIMATION_SPEED = 0.1f;

// character speeds
float BUNNY_RUN_SPEED = 2.0f;
float BUNNY_WALK_SPEED = 1.0f;
float FARMER_RUN_SPEED = 1.5f;

struct GameState
{
	int score{ 0 };
};
GameState gameState;

// TODO - create carrot and UI objects
enum GameObjectType
{
	TYPE_NULL,
	TYPE_BUNNY, // the player character
	TYPE_FARMER, // the enemy character
	TYPE_ENVIRONMENT, // objects that are part of the environment and do not move
	TYPE_DESTROYED, // all objects of this type are destroyed at the end of each MainGameUpdate/tick
};

//! @brief Updates the player character and redraws it.
void UpdateBunny();
//! @brief Updates the enemy character and redraws it.
void UpdateFarmer();
//! @brief Updates the environment objects and redraws them.
void UpdateEnvironment();
//! @brief Scales up the game's display size depending on the user's current screen resolution.
void InitDisplaySize();
//! @brief Reverts the player's position if they are colliding with an environment object or the leaving the display area.
//! @param object The GameObject to check for collision with environment objects or the display boundaries.
void RevertPositionIfNecessary(GameObject& object);
//! @brief Reverts the player's position if they are colliding with an environment object or the leaving the display area.
//! @param object The GameObject to check for collision with environment objects.
//! @return Returns true if the object is colliding with any environment objects.
bool HasBlockingCollision(GameObject& object);

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	InitDisplaySize();
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();

	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::StartAudioLoop("music");
	Play::Audio::SetLoopingSoundVolume("music", MUSIC_VOLUME);

	// add characters to the world
	Play::CreateGameObject(TYPE_BUNNY, { DISPLAY_WIDTH/2, 24 }, 8, "bunny");
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	Play::SetSprite(obj_bunny, "bunny_downidle", BUNNY_IDLE_ANIMATION_SPEED);

	Play::CreateGameObject(TYPE_FARMER, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT*2/3 }, 8, "farmer");
	GameObject& obj_farmer = Play::GetGameObjectByType(TYPE_FARMER);
	Play::SetSprite(obj_farmer, "farmer_idle", FARMER_IDLE_ANIMATION_SPEED);
	Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 17, 11 }, false);

	// TODO - also create visible objects with collision too e.g. sunflower or extra trees
	// TODO - just draw the invisible water blocks on y = 328, since the others will never be collided with?
	// creates 3 rows of invisible water objects at the top to prevent the player from walking over them
	for (int i = 6; i < DISPLAY_WIDTH; i += 12)
	{
		Play::CreateGameObject(TYPE_ENVIRONMENT, { i, 360 }, 8, "transparent");
		Play::CreateGameObject(TYPE_ENVIRONMENT, { i, 344 }, 8, "transparent");
		Play::CreateGameObject(TYPE_ENVIRONMENT, { i, 328 }, 8, "transparent");
	}
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	UpdateBunny();
	UpdateFarmer();
	UpdateEnvironment();
	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

// TODO - only do the full water animation once, then go back to the idle animation in the same direction
// TODO - do not allow the player to move while watering
// TODO - tidy up the code / refactor or create controls class
void UpdateBunny()
{
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	int currentSpriteId = obj_bunny.spriteId;
	string currentSpriteName = Play::GetSpriteName(currentSpriteId);

	// water animations
	if (Play::KeyDown(Play::KEY_SPACE))
	{
		// check what direction the player was facing and set the correct water animation
		if (currentSpriteName.find("UP") != string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_upwater", BUNNY_WATER_ANIMATION_SPEED);
		}
		else if ((currentSpriteName.find("DOWN") != string::npos))
		{
			Play::SetSprite(obj_bunny, "bunny_downwater", BUNNY_WATER_ANIMATION_SPEED);
		}
		else if (currentSpriteName.find("LEFT") != string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_leftwater", BUNNY_WATER_ANIMATION_SPEED);
		}
		else if (currentSpriteName.find("RIGHT") != string::npos)
		{
			Play::SetSprite(obj_bunny, "bunny_rightwater", BUNNY_WATER_ANIMATION_SPEED);
		}

		//stand still while watering
		obj_bunny.velocity = { 0, 0 };
	}
	// run animations
	else if (Play::KeyDown(Play::KEY_SHIFT)
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
	else if (currentSpriteName.find("UP") != string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_upidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("DOWN") != string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_downidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("LEFT") != string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftidle", BUNNY_IDLE_ANIMATION_SPEED);
	}
	else if (currentSpriteName.find("RIGHT") != string::npos)
	{
		obj_bunny.velocity = { 0, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightidle", BUNNY_IDLE_ANIMATION_SPEED);
	}

	// update and draw the player
	Play::UpdateGameObject(obj_bunny);
	RevertPositionIfNecessary(obj_bunny);
	Play::DrawObjectRotated(obj_bunny);
}

void UpdateFarmer()
{
	GameObject& obj_farmer = Play::GetGameObjectByType(TYPE_FARMER);

	// shoot
	if (Play::KeyDown(Play::KEY_ENTER))
	{
		Play::SetSprite(obj_farmer, "farmer_shoot", FARMER_SHOOT_ANIMATION_SPEED);
		obj_farmer.velocity = { 0, 0 };
	}
	// run
	else if (Play::KeyDown(Play::KEY_I))
	{
		obj_farmer.velocity = { 0, FARMER_RUN_SPEED };
		Play::SetSprite(obj_farmer, "farmer_run", FARMER_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_K))
	{
		obj_farmer.velocity = { 0, -FARMER_RUN_SPEED };
		Play::SetSprite(obj_farmer, "farmer_run", FARMER_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_J))
	{
		obj_farmer.velocity = { -FARMER_RUN_SPEED, 0 };
		Play::SetSprite(obj_farmer, "farmer_run", FARMER_RUN_ANIMATION_SPEED);
	}
	else if (Play::KeyDown(Play::KEY_L))
	{
		obj_farmer.velocity = { FARMER_RUN_SPEED, 0 };
		Play::SetSprite(obj_farmer, "farmer_run", FARMER_RUN_ANIMATION_SPEED);
	}
	// idle
	else
	{
		obj_farmer.velocity = { 0, 0 };
		Play::SetSprite(obj_farmer, "farmer_idle", FARMER_IDLE_ANIMATION_SPEED);
	}

	// farmer sits on the left side of the sprite, so we need to adjust the origin
	Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 17, 11 }, false);

	// update and draw the player
	Play::UpdateGameObject(obj_farmer);
	RevertPositionIfNecessary(obj_farmer);
	Play::DrawObjectRotated(obj_farmer);
}

void UpdateEnvironment()
{
	vector<int> ids = Play::CollectGameObjectIDsByType(TYPE_ENVIRONMENT);
	for (int id : ids)
	{
		GameObject& obj = Play::GetGameObject(id);
		Play::DrawObjectRotated(obj);
	}
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

void RevertPositionIfNecessary(GameObject& object)
{
	if (Play::IsHitboxLeavingDisplayArea(object)
		|| HasBlockingCollision(object))
	{
		object.pos = object.oldPos;
	}
}
// checks if the object is colliding with any environment objects
bool HasBlockingCollision(GameObject& object)
{
	vector<int> environmentIds = Play::CollectGameObjectIDsByType(TYPE_ENVIRONMENT);
	for (int environmentId : environmentIds)
	{
		GameObject& obj_environment = Play::GetGameObject(environmentId);
		if (Play::IsColliding(object, obj_environment))
		{
			return true;
		}
	}
	return false;
}