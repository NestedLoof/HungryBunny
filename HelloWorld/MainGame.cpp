#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Windows.h"

int DISPLAY_WIDTH = 640;
int DISPLAY_HEIGHT = 368;
int DISPLAY_SCALE = 1;

struct GameState
{
	int score{ 0 };
};
GameState gameState;

void HandlePlayerControls();

// TODO REMOVE
void UpdateFan();
void UpdateTools();
void UpdateCoinsAndStars();

enum GameObjectType
{
	TYPE_NULL,
	TYPE_BUNNY,
	TYPE_FAN,
	TYPE_TOOL,
	TYPE_COIN,
	TYPE_STAR,
	TYPE_DESTROYED
};

// scales up the game if the screen resolution is large enough
void InitDisplaySize()
{
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();

	RECT desktop;
	GetWindowRect(hDesktop, &desktop);

	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates for the screen size
	int x = desktop.right;
	int y = desktop.bottom;
	if (x > (DISPLAY_WIDTH * 4) + 40 && y > (DISPLAY_HEIGHT * 4) + 40)
	{
		DISPLAY_SCALE = 4;
	}
	else if (x > (DISPLAY_WIDTH * 3) + 40 && y > (DISPLAY_HEIGHT * 3) + 40)
	{
		DISPLAY_SCALE = 3;
	}
	else if (x > (DISPLAY_WIDTH * 2) + 40 && y > (DISPLAY_HEIGHT * 2) + 40)
	{
		DISPLAY_SCALE = 2;
	}
}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	InitDisplaySize();
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();

	Play::LoadBackground("Data\\Backgrounds\\grassbackground3.png");
	Play::StartAudioLoop("music");
	Play::Audio::SetLoopingSoundVolume("music", 0.02);
	Play::CreateGameObject(TYPE_BUNNY, { DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 }, 8, "bunny");
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	Play::SetSprite(obj_bunny, "bunny_downidle", 0.05f);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	HandlePlayerControls();

	Play::PresentDrawingBuffer();
	return Play::KeyDown( KEY_ESCAPE );
}

void HandlePlayerControls()
{
	GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	float waterAnimationSpeed = 0.15f;
	float runAnimationSpeed = 0.3f;
	float walkAnimationSpeed = 0.15f;
	float idleAnimationSpeed = 0.1f;
	int runSpeed = 2;
	int walkSpeed = 1;

	// current sprite
	int currentSpriteId = obj_bunny.spriteId;
	string currentSpriteName = Play::GetSpriteName(currentSpriteId);
	for (auto& c : currentSpriteName)
	{
		c = tolower(c);
	}

	//vector<string> waterSprites = { "bunny_leftwater", "bunny_rightwater", "bunny_upwater", "bunny_downwater" };
	vector<string> upSprites = { "bunny_upidle_8", "bunny_upwalk_8", "bunny_uprun_8", "bunny_upwater_8" };
	vector<string> downSprites = { "bunny_downidle_8", "bunny_downwalk_8", "bunny_downrun_8", "bunny_downwater_8" };
	vector<string> leftSprites = { "bunny_leftidle_8", "bunny_leftwalk_8", "bunny_leftrun_8", "bunny_leftwater_8" };
	vector<string> rightSprites = { "bunny_rightidle_8", "bunny_rightwalk_8", "bunny_rightrun_8", "bunny_rightwater_8" };

	// water animation
	if (Play::KeyDown(Play::KEY_SPACE))
	{
		if (count(upSprites.begin(), upSprites.end(), currentSpriteName) > 0)
		{
			Play::SetSprite(obj_bunny, "bunny_upwater", waterAnimationSpeed);
		}
		else if (count(downSprites.begin(), downSprites.end(), currentSpriteName) > 0)
		{
			Play::SetSprite(obj_bunny, "bunny_downwater", waterAnimationSpeed);
		}
		else if (count(leftSprites.begin(), leftSprites.end(), currentSpriteName) > 0)
		{
			Play::SetSprite(obj_bunny, "bunny_leftwater", waterAnimationSpeed);
		}
		else if (count(rightSprites.begin(), rightSprites.end(), currentSpriteName) > 0)
		{
			Play::SetSprite(obj_bunny, "bunny_rightwater", waterAnimationSpeed);
		}

		obj_bunny.velocity = { 0, 0 };
	}
	// run animations
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_UP) || Play::KeyDown(Play::KEY_W)))
	{
		obj_bunny.velocity = { 0, runSpeed };
		Play::SetSprite(obj_bunny, "bunny_uprun", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_DOWN) || Play::KeyDown(Play::KEY_S)))
	{
		obj_bunny.velocity = { 0, -runSpeed };
		Play::SetSprite(obj_bunny, "bunny_downrun", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_LEFT) || Play::KeyDown(Play::KEY_A)))
	{
		obj_bunny.velocity = { -runSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_leftrun", walkAnimationSpeed);
	}
	else if (Play::KeyDown(Play::KEY_SHIFT)
		&& (Play::KeyDown(Play::KEY_RIGHT) || Play::KeyDown(Play::KEY_D)))
	{
		obj_bunny.velocity = { runSpeed, 0 };
		Play::SetSprite(obj_bunny, "bunny_rightrun", walkAnimationSpeed);
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
	

	Play::UpdateGameObject(obj_bunny);
	if (Play::IsHitboxLeavingDisplayArea(obj_bunny, Play::ALL))
		obj_bunny.pos = obj_bunny.oldPos;
	Play::DrawObjectRotated(obj_bunny);
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

