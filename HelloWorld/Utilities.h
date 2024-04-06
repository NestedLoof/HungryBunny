#pragma once
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

// screen size statics
inline int DISPLAY_WIDTH = 640;
inline int DISPLAY_HEIGHT = 368;
inline int DISPLAY_SCALE = 1; // will be increased if the screen is large enough in Play2::InitDisplaySize()
inline int TILE_SIZE = 16;

// audio volume
inline float MUSIC_VOLUME = 0.1f;

// gameplay statics
inline int MAX_HEALTH = 600;
inline int MIN_CARROT_HEALTH_REGEN = 100;
inline int MAX_CARROT_HEALTH_REGEN = 200;
inline int MIN_CABBAGE_HEALTH_REGEN = 300;
inline int MAX_CABBAGE_HEALTH_REGEN = 150;
inline float DECAY_RATE = 0.005f;
inline int MAX_NUM_VEGETABLES = 2;
inline int NUM_FLOWERS_GROW = 5;
inline int MAX_NUM_FLOWERS = 50;
inline bool FLOWER_CIRCLE = false; // if true, flowers will grow in a circle around the bunny
inline int FLOWER_COUNTER = 0; // incremented each time a flower is created

// settings to change the farmer difficulty
inline bool FARMER_MULTISHOOT = false; // if true, farmer will shoot multiple bursts at once, at different angles
inline int FARMER_ACTION_DELAY = 200; // 1 in X change to shoot or change direction on any given frame
inline int MAX_NUM_FARMERS = 1; // number of farmers to spawn (only 1 or 2 supported)
inline std::map<std::string, int> MAP_FARMER_NAME_TO_ID; // used to retrieve each farmer object by name

// starting positions
inline Play::Point2D BUNNY_START_POS = { DISPLAY_WIDTH / 2, (2 * 16) - 8 };
inline Play::Point2D FARMER_START_POS = { (35 * 16) - 8, (11 * 16) - 8 };
inline Play::Point2D FARMER2_START_POS = { (5 * 16) - 8, (11 * 16) - 8 };

// animation speeds - lower is slower
inline float BUNNY_WATER_ANIMATION_SPEED = 0.15f;
inline float BUNNY_RUN_ANIMATION_SPEED = 0.25f;
inline float BUNNY_WALK_ANIMATION_SPEED = 0.15f;
inline float BUNNY_IDLE_ANIMATION_SPEED = 0.1f;
inline float FARMER_SHOOT_ANIMATION_SPEED = 0.2f;
inline float FARMER_RUN_ANIMATION_SPEED = 0.2f;
inline float FARMER_IDLE_ANIMATION_SPEED = 0.1f;
inline float CARROT_DECAY_ANIMATION_SPEED = 0.03f;

// character speeds
inline float BUNNY_RUN_SPEED = 2.5f;
inline float BUNNY_WALK_SPEED = 1.5f;
inline float FARMER_RUN_SPEED = 1.5f;
inline float BULLET_SPEED = 6.0f;

// set to true if the current score is a high score
inline bool IS_HIGH_SCORE = false;

// carrot spawning map
inline std::map<int, std::vector<int>> MAP_PLOT_NUM_TO_IDS;

// menu button map
inline std::map<std::string, int> MAP_BUTTON_NAME_TO_ID;

// keep track of the order flowers are created so when the player hits the max number of flowers, the oldest one is destroyed
inline std::vector<std::pair<int, int>> FLOWER_OBJECT_ID_TO_COUNT;

// state of the game
enum BunnyState
{
	STATE_START_GAME,
	STATE_PLAY,
	STATE_DEAD,
};

// used to navigate through menu screens
enum MenuState
{
	MENU_GAME_OVER,
	MENU_HIGH_SCORES,
};

enum Difficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_MEDIUM,
	DIFFICULTY_HARD,
	DIFFICULTY_HUNGRY,
};

struct GameState
{
	int score{ 0 };
	BunnyState bunnyState = STATE_START_GAME;
	MenuState menuState = MENU_GAME_OVER;
	Difficulty difficulty = DIFFICULTY_EASY;
	int health = MAX_HEALTH;
};
inline GameState gameState;

enum GameObjectType
{
	TYPE_NULL,
	TYPE_BUNNY, // the player character
	TYPE_FARMER, // the enemy character
	TYPE_ENVIRONMENT, // objects that are part of the environment and do not move
	TYPE_DESTROYED, // all objects of this type are destroyed at the end of each MainGameUpdate/tick
	TYPE_CARROT,
	TYPE_CABBAGE, // better than carrots, but rarer
	TYPE_BULLET,
	TYPE_PLOT, // where vegetables can spawn
	TYPE_HEALTH_BACKGROUND,
	TYPE_HEALTH_BAR,
	TYPE_SCORE_BACKGROUND,
	TYPE_BUTTON, // menu button without the text
	TYPE_FLOWER,
	// TODO - add an object that briefly appears when the bunny collects a vegetable (e.g. +5, similar to 1up in mario or damage text in some MMOs)
	// TODO - add farmer boundary objects to stop him from going through the gates and getting stuck under the fence?
};
