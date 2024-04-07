#include "GameLogic.h"

// TODO - Move some methods into a World/Level class before making new levels

void GameLogic::SpawnVegetables()
{
	// if there is room for an extra carrot, spawn one at random (about a second delay on average)
	if (ShouldSpawnVegetable())
	{
		const Play::Point2D vegetablePos = GetNewVegetablePosition();

		// Spawn either cabbage or carrots ... 
		if (Play::RandomRoll(5) == 1) {
			// create the cabbage object
			int id = Play::CreateGameObject(TYPE_CABBAGE, vegetablePos, 8, "cabbage");
			Play::GameObject& obj_cabbage = Play::GetGameObject(id);
			Play::SetSprite(obj_cabbage, "cabbage", CARROT_DECAY_ANIMATION_SPEED);
		}
		else
		{
			// create the carrot object
			int id = Play::CreateGameObject(TYPE_CARROT, vegetablePos, 8, "carrot");
			Play::GameObject& obj_carrot = Play::GetGameObject(id);
			Play::SetSprite(obj_carrot, "carrot", CARROT_DECAY_ANIMATION_SPEED);
		}

		// pop when a vegetable appears
		Play::PlayAudio("carrot_pop");
	}
}

bool GameLogic::ShouldSpawnVegetable()
{
	const int numCarrots = Play::CollectGameObjectIDsByType(TYPE_CARROT).size();
	const int numCabbages = Play::CollectGameObjectIDsByType(TYPE_CABBAGE).size();
	return numCarrots + numCabbages < MAX_NUM_VEGETABLES
		&& Play::RandomRoll(60) == 1;
}

Play::Point2D GameLogic::GetNewVegetablePosition()
{
	// pick a random plot number to spawn a carrot in
	std::vector<int> randomPlotIds = MAP_PLOT_NUM_TO_IDS[Play::RandomRoll(MAP_PLOT_NUM_TO_IDS.size())];

	// then within that plot number, pick a random plot object to spawn the carrot in
	const int randomPlotId = randomPlotIds[Play::RandomRoll(randomPlotIds.size()) - 1];

	// randomise where the carrot spawns within the plot
	Play::GameObject& obj_plot = Play::GetGameObject(randomPlotId);
	const int x = obj_plot.pos.x + Play::RandomRoll(TILE_SIZE) - TILE_SIZE/2;
	const int y = obj_plot.pos.y + Play::RandomRoll(TILE_SIZE) - TILE_SIZE/2;
	return { x, y };
}

void GameLogic::UpdateVegetable(GameObjectType type, int points, int minRegen, int maxRegen, float regenDecayRate)
{
	// Go through each vegetable of the given type and check if it is being eaten by the bunny or if it has decayed.
	for (int vegetableId : Play::CollectGameObjectIDsByType(type))
	{
		Play::GameObject& obj_vegetable = Play::GetGameObject(vegetableId);
		// the vegetable has decayed, so destroy it
		if (Play::IsAnimationComplete(obj_vegetable))
		{
			obj_vegetable.type = TYPE_DESTROYED;
		}
		// bunny eats the vegetable when colliding with it as long as the bunny is still alive.
		else if (Play::IsColliding(Play::GetGameObjectByType(TYPE_BUNNY), obj_vegetable)
			&& gameState.bunnyState == STATE_PLAY)
		{
			obj_vegetable.type = TYPE_DESTROYED;
			gameState.score += points;

			// when bunny eats a vegetable, health is increased
			// the health regenerated per vegetable is reduced depending on the current score, decaying from max to min.
			// healthregen = MIN + (MAX-MIN)*e^(- DECAY_RATE * score)

			// e.g. for carrots, if max regen = 200, min regen = 50, decay rate = 0.005:
			// then regen at score 0 is +200, at score 80 is +150, at score 220 is +100, at score 280 is +125, at score 380 is +110, at score 1000 is +100

			// e.g. for cabbage, if max regen = 300, min regen = 100, decay rate = 0.005:
			// then regen at score 0 is +300, at score 60 is +250, at score 140 is +200, at score 400 is +75, at score 1000+ is +50

			gameState.health += minRegen +
				(maxRegen - minRegen) * exp(-regenDecayRate * gameState.score);
			if (gameState.health > MAX_HEALTH)
			{
				gameState.health = MAX_HEALTH;
			}

			Play::PlayAudio("points");
		}

		// Draw the vegetable
		Play::UpdateGameObject(obj_vegetable);
		Play::DrawObject(obj_vegetable);
	}
}

void GameLogic::UpdateVegetables()
{
	// spawn in any carrots if necessary, up to the maximum number of carrots (MAX_NUM_CARROTS)
	SpawnVegetables();

	// update the vegetable animations, draw them, detect if they are being eaten and update the health and score accordingly, or destroy them if they have decayed.
	UpdateVegetable(TYPE_CARROT, 5, MIN_CARROT_HEALTH_REGEN, MAX_CARROT_HEALTH_REGEN, DECAY_RATE);
	UpdateVegetable(TYPE_CABBAGE, 10, MIN_CABBAGE_HEALTH_REGEN, MAX_CABBAGE_HEALTH_REGEN, DECAY_RATE);
}

// destroy the oldest flowers if there are too many, so we don't lag and the player can't have infinite bullet blockers making it too easy.
void GameLogic::DestroyOldestFlowers()
{
	int numFlowers = FLOWER_OBJECT_ID_TO_COUNT.size();
	int extraFlowers = numFlowers - MAX_NUM_FLOWERS;
	if (extraFlowers <= 0)
	{
		return;
	}

	// sort the flower vector by the counter value so that we can destroy the oldest flowers, which are sorted to be first in the map
 	std::sort(FLOWER_OBJECT_ID_TO_COUNT.begin(), 
		FLOWER_OBJECT_ID_TO_COUNT.end(), 
		[](auto& a, auto& b) { return a.second < b.second; });

	// iterate backwards so we don't mess up the indices when erasing elements
 	for (int i = extraFlowers - 1 ; i >= 0; i--)
	{
		// destroy the flower
		int flowerId = FLOWER_OBJECT_ID_TO_COUNT[i].first;
		Play::GameObject& obj_flower = Play::GetGameObject(flowerId);
		obj_flower.type = TYPE_DESTROYED;

		// remove it from the map of flower to counter
		FLOWER_OBJECT_ID_TO_COUNT.erase(FLOWER_OBJECT_ID_TO_COUNT.begin() + i);
	}
}

void GameLogic::UpdateFlowers()
{
	// remove the oldest flowers if there are too many. Added a counter variable to game objects to track this.
	DestroyOldestFlowers();

	// get all the non-destroyed flower object pointers in a vector
	std::vector<int> flowerIds = Play::CollectGameObjectIDsByType(TYPE_FLOWER);
	std::vector<Play::GameObject*> flowers(flowerIds.size());
	for (int i = 0; i < flowerIds.size(); i++)
	{
		int flowerId = flowerIds[i];
		Play::GameObject& obj_flower = Play::GetGameObject(flowerId);
		flowers[i] = &obj_flower;
	}

	// sort the flowers by their y position so that the flowers are drawn in the correct order from top to bottom
	std::sort(flowers.begin(), flowers.end(), [](const Play::GameObject* a, const Play::GameObject* b) { return a->pos.y > b->pos.y; });

	for (Play::GameObject* obj_flower_ptr : flowers)
	{
		Play::DrawObjectRotated(*obj_flower_ptr);

		std::vector<int> bulletIds = Play::CollectGameObjectIDsByType(TYPE_BULLET);
		for (int bulletId : bulletIds)
		{
			Play::GameObject& obj_bullet = Play::GetGameObject(bulletId);
			if (Play::IsColliding(obj_bullet, *obj_flower_ptr))
			{
				obj_flower_ptr->type = TYPE_DESTROYED;
				obj_bullet.type = TYPE_DESTROYED;
				break;
			}
		}
	}
}

void GameLogic::UpdateDestroyed()
{
	// get all objects of type destroyed and actually destroy them.
	std::vector<int> vDestroyed = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);
	for (int id : vDestroyed)
	{
		Play::GameObject& obj_destroyed = Play::GetGameObject(id);
		Play::DestroyGameObject(id);
	}
}
void GameLogic::InitialiseEnvironment()
{
	// creates a row of invisible water objects to prevent the player from walking over them.
	for (int x = 6; x < DISPLAY_WIDTH; x += 10)
	{
		Play::CreateGameObject(TYPE_ENVIRONMENT, { x, 328 }, 8, "transparent");
	}

	// create 2 columns of fences at each side of the screen.
	for (int y = 24; y < 320; y += 8)
	{
		Play::CreateGameObject(TYPE_ENVIRONMENT, { 8, y }, 4, "transparent");
		Play::CreateGameObject(TYPE_ENVIRONMENT, { 632, y }, 4, "transparent");
	}

	// create the row of fences at the bottom with a small gap in the middle where the gate is.
	for (int x = 0; x < 300; x += 8)
	{
		Play::CreateGameObject(TYPE_ENVIRONMENT, { x, 24 }, 4, "transparent");
	}
	for (int x = 348; x <= 640; x += 8)
	{
		Play::CreateGameObject(TYPE_ENVIRONMENT, { x, 24 }, 4, "transparent");
	}

	// tree in top left
	Play::CreateGameObject(TYPE_ENVIRONMENT, { 24, 312 }, 4, "transparent");

	// well
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (30 * 16), (19 * 16) }, 8, "transparent");

	// Chicken hut
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (39 * 16), (19 * 16) }, 28, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (39 * 16), (20 * 16) }, 28, "transparent");

	// Grains Storage hut
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (39 * 16), (13 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (38 * 16), (13 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (38 * 16), (14 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (38 * 16), (15 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (39 * 16), (15 * 16) }, 8, "transparent");

	// bails of hay on bottom right corner
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (37 * 16), (7 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (37 * 16), (6 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (37 * 16), (5 * 16) }, 4, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (37 * 16), (4 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (38 * 16), (7 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (39 * 16), (7 * 16) }, 8, "transparent");

	Play::CreateGameObject(TYPE_ENVIRONMENT, { 33 * 16 - 8, 40 }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { 33 * 16 - 8, 44 }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (37 * 16), (4 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (33 * 16), (3 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (34 * 16), (3 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (35 * 16), (3 * 16) }, 8, "transparent");
	Play::CreateGameObject(TYPE_ENVIRONMENT, { (36 * 16), (3 * 16) }, 8, "transparent");
}
void GameLogic::InitialisePlots()
{
	// create carrot plots - each plot is a 16x16 square, and each plot number is a different area that contains multiple plot squares.
	std::map<int, std::vector<Play::Vector2D>> mapPlotNumToCoords;
	mapPlotNumToCoords[1] = std::vector<Play::Vector2D>{ { 4, 6 }, { 4, 7 }, { 4, 8 } };
	mapPlotNumToCoords[2] = std::vector<Play::Vector2D>{ { 4, 12 }, { 4, 13 } };
	mapPlotNumToCoords[3] = std::vector<Play::Vector2D>{ { 4, 17 } };
	mapPlotNumToCoords[4] = std::vector<Play::Vector2D>{ { 9, 6 },  { 9, 7 }, { 9, 8 }, { 10, 6 }, { 10, 7 }, { 10, 8 } };
	mapPlotNumToCoords[5] = std::vector<Play::Vector2D>{ { 9, 13 },  { 9, 14 }, { 9, 15 }, { 9, 16 },
													{ 10, 13 },  { 10, 14 }, { 10, 15 }, { 10, 16 },
													{ 11, 13 },  { 11, 14 }, { 11, 15 }, { 11, 16 },
													{ 12, 13 },  { 12, 14 }, { 12, 15 }, { 12, 16 },
													{ 13, 13 },  { 13, 14 }, { 13, 15 }, { 13, 16 },
													{ 14, 6 },  { 14, 7 }, { 14, 8 }, { 14, 9 }, { 14, 10 }, { 14, 11 },
													{ 14, 12 }, { 14, 13 }, { 14, 14 }, { 14, 15 }, { 14, 16 },
													{ 15, 6 },  { 15, 7 }, { 15, 8 }, { 15, 9 }, { 15, 10 }, { 15, 11 },
													{ 15, 12 }, { 15, 13 }, { 15, 14 }, { 15, 15 }, { 15, 16 },
													{ 16, 6 },  { 16, 7 }, { 16, 8 }, { 16, 9 }, { 16, 10 }, { 16, 11 },
													{ 16, 12 }, { 16, 13 }, { 16, 14 }, { 16, 15 }, { 16, 16 } };
	mapPlotNumToCoords[6] = std::vector<Play::Vector2D>{ { 21, 6 } };
	mapPlotNumToCoords[7] = std::vector<Play::Vector2D>{ { 22, 11 } };
	mapPlotNumToCoords[8] = std::vector<Play::Vector2D>{ { 22, 15 }, { 22, 16 } };
	mapPlotNumToCoords[9] = std::vector<Play::Vector2D>{ { 27, 4 }, { 27, 5 }, { 27, 6 } };
	mapPlotNumToCoords[10] = std::vector<Play::Vector2D>{ { 27, 14 }, { 27, 15 } };
	mapPlotNumToCoords[11] = std::vector<Play::Vector2D>{ { 33, 6 }, { 33, 7 } };
	mapPlotNumToCoords[12] = std::vector<Play::Vector2D>{ { 34, 15 }, { 34, 16 }, { 34, 17 } };

	// create each of the plot objects (16x16 squares) in the correct positions, then add all of their ids to the global map to spawn carrots in later.
	for (auto& [plotNum, plotCoords] : mapPlotNumToCoords)
	{
		for (auto& coord : plotCoords)
		{
			// create a transparent plot object
			int id_plot = Play::CreateGameObject(TYPE_PLOT, { (coord.x * 16) - 8, (coord.y * 16) - 8 }, 0, "transparent");
			MAP_PLOT_NUM_TO_IDS[plotNum].push_back(id_plot);
		}
	}
}

void GameLogic::RestartGame(Difficulty difficulty)
{
	// reset game state stuff
	gameState.difficulty = difficulty;
	InitialiseDifficulty();
	gameState.bunnyState = STATE_PLAY;
	gameState.menuState = MENU_GAME_OVER;
	gameState.health = MAX_HEALTH;
	gameState.score = 0;
	IS_HIGH_SCORE = false;
	Play::StartAudioLoop("music");
	Play::Audio::SetLoopingSoundVolume("music", MUSIC_VOLUME);

	// reset bunny position
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	obj_bunny.pos = BUNNY_START_POS;
	obj_bunny.velocity = { 0, 0 };
	obj_bunny.rotation = 0;
	obj_bunny.frame = 0;

	// reset farmers
	int farmerId = MAP_FARMER_NAME_TO_ID["farmer"];
	Play::GameObject& obj_farmer = Play::GetGameObject(farmerId);
	obj_farmer.pos = FARMER_START_POS;
	obj_farmer.velocity = { 0, 0 };
	obj_farmer.rotation = 0;
	obj_farmer.frame = 0;
	Play::SetSprite(obj_farmer, "farmerleft_idle2", 0);

	int farmer2Id = MAP_FARMER_NAME_TO_ID["farmer2"];
	Play::GameObject& obj_farmer2 = Play::GetGameObject(farmer2Id);
	obj_farmer2.pos = FARMER2_START_POS;
	obj_farmer2.velocity = { 0, 0 };
	obj_farmer2.rotation = 0;
	obj_farmer2.frame = 0;
	Play::SetSprite(obj_farmer2, "farmerleft_idle2", 0);

	// destroy all objects in the previous game
	DestroyAllObjectsForType(TYPE_CARROT);
	DestroyAllObjectsForType(TYPE_CABBAGE);
	DestroyAllObjectsForType(TYPE_BULLET);
	DestroyAllObjectsForType(TYPE_FLOWER);
	FLOWER_OBJECT_ID_TO_COUNT.clear();
	FLOWER_COUNTER = 0;
}

void GameLogic::DestroyAllObjectsForType(GameObjectType type)
{
	for (int id : Play::CollectGameObjectIDsByType(type))
	{
		Play::GetGameObject(id).type = TYPE_DESTROYED;
	}
}

void GameLogic::InitialiseDifficulty()
{
	// reset default difficulty values (medium)
	MAX_HEALTH = 450;
	FARMER_MULTISHOOT = false;
	FARMER_ACTION_DELAY = 150;
	MAX_NUM_VEGETABLES = 2;
	BUNNY_RUN_SPEED = 2.25f;
	BUNNY_WALK_SPEED = 1.25f;
	FARMER_RUN_SPEED = 1.75f;
	BULLET_SPEED = 5.0f;
	MAX_NUM_FARMERS = 1;
	NUM_FLOWERS_GROW = 5;
	FLOWER_CIRCLE = false;

	// set some gameplay variables based on the difficulty
	switch (gameState.difficulty)
	{
		case DIFFICULTY_EASY:
			MAX_HEALTH = 600; // higher max health
			FARMER_ACTION_DELAY = 200; // enemy does actions less frequently
			MAX_NUM_VEGETABLES = 3;
			/*BUNNY_RUN_SPEED = 2.5f;
			BUNNY_WALK_SPEED = 1.5f;
			FARMER_RUN_SPEED = 1.5f;*/
			BULLET_SPEED = 4.0f;
			NUM_FLOWERS_GROW = 7;
			break;
		case Difficulty::DIFFICULTY_MEDIUM:
			// same as defaults
			break;
		case Difficulty::DIFFICULTY_HARD:
			MAX_HEALTH = 400; // lower max health
			FARMER_MULTISHOOT = true; // farmer can shoot multiple bullets
			FARMER_ACTION_DELAY = 100; // enemy does actions more frequently
			/*BUNNY_RUN_SPEED = 2.0f;
			BUNNY_WALK_SPEED = 1.0f;
			FARMER_RUN_SPEED = 2.0f;*/
			BULLET_SPEED = 6.0f;
			NUM_FLOWERS_GROW = 3;
			break;
		case Difficulty::DIFFICULTY_HUNGRY:
			FARMER_MULTISHOOT = true;
			FARMER_ACTION_DELAY = 50; // farmers' actions are manic
			MAX_NUM_VEGETABLES = 3;
			MAX_NUM_FARMERS = 2;
			NUM_FLOWERS_GROW = 16;
			FLOWER_CIRCLE = true;
			break;
	}
}

void GameLogic::GameOver()
{
	// game state stuff
	gameState.bunnyState = STATE_DEAD;
	gameState.health = 0;

	// sort out audio
	Play::StopAudio("music");
	Play::StopAudio("bubble"); // don't accidentally loop watering sound if killed mid-animation
	Play::PlayAudio("failure_drum_sound");

	// make the bunny lie on the ground
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	obj_bunny.velocity = { 0,0 };
	obj_bunny.acceleration = { 0,0 };
	obj_bunny.rotation = Play::PLAY_PI * 3 / 2;
	Play::SetSprite(obj_bunny, "bunny_leftidle", 0);

	Scores::SaveScore();
}
