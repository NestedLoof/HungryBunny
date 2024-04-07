#include "Farmer.h"

void Farmer::InitialiseFarmers()
{
	// main farmer
	int farmerId = Play::CreateGameObject(TYPE_FARMER, FARMER_START_POS, 8, "farmer");
	Play::GameObject& obj_farmer = Play::GetGameObject(farmerId);
	Play::SetSprite(obj_farmer, "farmerleft_idle2", FARMER_IDLE_ANIMATION_SPEED);
	Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 17, 11 }, false);
	MAP_FARMER_NAME_TO_ID.insert({ "farmer", farmerId });

	// additional farmer for higher difficulty
	int farmer2Id = Play::CreateGameObject(TYPE_FARMER, FARMER2_START_POS, 8, "farmer");
	Play::GameObject& obj_farmer2 = Play::GetGameObject(farmer2Id);
	Play::SetSprite(obj_farmer2, "farmerleft_idle2", FARMER_IDLE_ANIMATION_SPEED);
	Play::Graphics::SetSpriteOrigin(obj_farmer2.spriteId, { 17, 11 }, false);
	MAP_FARMER_NAME_TO_ID.insert({ "farmer2", farmer2Id });
}

void Farmer::UpdateFarmers()
{
	// main farmer
	int farmerId = MAP_FARMER_NAME_TO_ID["farmer"];
	Play::GameObject& obj_farmer = Play::GetGameObject(farmerId);
	UpdateFarmer(obj_farmer);

	// additional farmer for higher difficulty
	if (MAX_NUM_FARMERS == 2)
	{
		int farmer2Id = MAP_FARMER_NAME_TO_ID["farmer2"];
		Play::GameObject& obj_farmer2 = Play::GetGameObject(farmer2Id);
		UpdateFarmer(obj_farmer2);
	}
}

void Farmer::UpdateFarmer(Play::GameObject& obj_farmer)
{
	SpawnBulletIfNecessary(obj_farmer);
	ChooseFarmerAction(obj_farmer);
	RotateFarmer(obj_farmer);
	RecentreFarmerSprite(obj_farmer);
	NormaliseFarmerVelocity(obj_farmer);
	DrawFarmer(obj_farmer);
}

void Farmer::SpawnBulletIfNecessary(Play::GameObject& obj_farmer)
{
	// only spawn a bullet once the shooting animation is on the correct frame. Since the animation frame lasts for about 6 ticks, you get 6 bullets in a burst.
	int farmerSpriteId = obj_farmer.spriteId;
	std::string farmerSpriteName = Play::GetSpriteName(farmerSpriteId);
	bool shooting = farmerSpriteName.find("SHOOT") != std::string::npos;
	if (shooting && obj_farmer.frame == 7)
	{
		SpawnBullet(obj_farmer);

		if (FARMER_MULTISHOOT)
		{
			// spawn 2 more bullet sprays at a +/- 45 degree angle to the farmer's current direction if multishoot is enabled (harder difficulties)
			SpawnBullet(obj_farmer, Play::PLAY_PI / 4);
			SpawnBullet(obj_farmer, -Play::PLAY_PI / 4);
		}
	}

	// update and draw all bullets later in UpdateBullets
}

void Farmer::SpawnBullet(Play::GameObject& obj_farmer, float addedBulletAngle)
{
	// create the bullet object
	int bulletId = Play::CreateGameObject(TYPE_BULLET, obj_farmer.pos, 4, "bullet");
	Play::GameObject& obj_bullet = Play::GetGameObject(bulletId);
	Play::SetSprite(obj_bullet, "waterbullet2", 0);

	// set the rotation depending on the angle it is fired at
	obj_bullet.rotation = CalculateBulletRotation(obj_farmer, addedBulletAngle);

	// set the velocity of the bullet to match the rotation
	obj_bullet.velocity.x = sin(obj_bullet.rotation) * BULLET_SPEED;
	obj_bullet.velocity.y = -cos(obj_bullet.rotation) * BULLET_SPEED;
}

float Farmer::CalculateBulletRotation(Play::GameObject& obj_farmer, float addedBulletAngle)
{
	float rotation;

	// rotate the bullet to match the farmer's rotation
	int farmerSpriteId = obj_farmer.spriteId;
	std::string farmerSpriteName = Play::GetSpriteName(farmerSpriteId);
	if (farmerSpriteName.find("RIGHT") != std::string::npos)
	{
		// the bullet sprite is facing down, so add 90 degrees to the farmer's rotation
		rotation = obj_farmer.rotation + Play::PLAY_PI / 2;
	}
	else
	{
		// rotate an extra 180 degrees when the farmer is left-facing
		rotation = obj_farmer.rotation + (3 * Play::PLAY_PI / 2);
	}

	// add a slight randomness to the bullet's direction (+/- 0.15 radians ~= 8 degrees)
	rotation += (Play::RandomRoll(30) - static_cast<float>(15)) / 100;
	rotation += addedBulletAngle; // due to multishoot
	return rotation;
}

void Farmer::UpdateBullets()
{
	std::vector<int> bulletIds = Play::CollectGameObjectIDsByType(TYPE_BULLET);
	for (int bulletId : bulletIds)
	{
		Play::GameObject& obj_bullet = Play::GetGameObject(bulletId);
		Play::UpdateGameObject(obj_bullet);
		Play::DrawObjectRotated(obj_bullet);

		// destroy the bullet if it leaves the display area
		if (Play2::IsHitboxLeavingDisplayArea(obj_bullet))
		{
			obj_bullet.type = TYPE_DESTROYED;
		}
		// destroy the bullet if it collides with the player
		else if (Play::IsColliding(Play::GetGameObjectByType(TYPE_BUNNY), obj_bullet))
		{
			obj_bullet.type = TYPE_DESTROYED;
			// only game over once! Not when the farmers carry on shooting the bunny on the game over screen.
			if (gameState.bunnyState == STATE_PLAY)
			{
				GameLogic::GameOver();
			}
		}
	}
}

void Farmer::ChooseFarmerAction(Play::GameObject& obj_farmer)
{
	// only shoot once then change to idle. Don't allow other actions while shooting
	int currentSpriteId = obj_farmer.spriteId;
	std::string currentSpriteName = Play::GetSpriteName(currentSpriteId);
	bool shooting = currentSpriteName.find("SHOOT") != std::string::npos;
	if (shooting)
	{
		if (Play::IsAnimationComplete(obj_farmer))
		{
			obj_farmer.velocity = { 0, 0 };
			if (currentSpriteName.find("RIGHT") != std::string::npos)
			{
				Play::SetSprite(obj_farmer, "farmerright_idle", FARMER_IDLE_ANIMATION_SPEED);
			}
			else
			{
				Play::SetSprite(obj_farmer, "farmerleft_idle2", FARMER_IDLE_ANIMATION_SPEED);
			}
		}
	}
	else
	{
		// randomly shoot or walk around based on a random roll. The chance of performing an action is dependent on the difficulty (harder = more frequent, so lower delay).
		int actionRoll = Play::RandomRoll(FARMER_ACTION_DELAY);
		if (actionRoll == 1)
		{
			obj_farmer.velocity = { 0, 0 };
			Play::SetSprite(obj_farmer, "farmerleft_shoot2", FARMER_SHOOT_ANIMATION_SPEED);
			Play::PlayAudio("farmer_shoot_short");
		}
		else if (actionRoll == 2)
		{
			obj_farmer.velocity.y = FARMER_RUN_SPEED;
		}
		else if (actionRoll == 3)
		{
			obj_farmer.velocity.y = -FARMER_RUN_SPEED;
		}
		else if (actionRoll == 4)
		{
			obj_farmer.velocity.x = FARMER_RUN_SPEED;
		}
		else if (actionRoll == 5)
		{
			obj_farmer.velocity.x = -FARMER_RUN_SPEED;
		}
	}
}

void Farmer::RotateFarmer(Play::GameObject& obj_farmer)
{
	// check if the farmer is current shooting in order to set the correct left or right-facing sprite
	int currentSpriteId = obj_farmer.spriteId;
	std::string currentSpriteName = Play::GetSpriteName(currentSpriteId);
	bool shooting = currentSpriteName.find("SHOOT") != std::string::npos;

	// find angle between farmer and bunny
	// atan2 will return an angle in radians between -pi and pi
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	float angle = atan2(obj_bunny.pos.y - obj_farmer.pos.y, obj_bunny.pos.x - obj_farmer.pos.x);

	// set the farmer facing right, towards the player
	if (angle >= -Play::PLAY_PI / 2
		&& angle <= Play::PLAY_PI / 2)
	{
		if (shooting)
		{
			Play::SetSprite(obj_farmer, "farmerright_shoot", FARMER_SHOOT_ANIMATION_SPEED);
		}
		else if (obj_farmer.velocity != Play::Vector2D{ 0, 0 })
		{
			Play::SetSprite(obj_farmer, "farmerright_run", FARMER_RUN_ANIMATION_SPEED);
		}
		else
		{
			Play::SetSprite(obj_farmer, "farmerright_idle", FARMER_IDLE_ANIMATION_SPEED);
		}

		if (angle >= -Play::PLAY_PI / 4
			&& angle <= Play::PLAY_PI / 4)
		{
			obj_farmer.rotation = angle;
		}
		// if the angle is beyond 45 degrees, limit the rotation
		else if (angle > Play::PLAY_PI / 4)
		{
			obj_farmer.rotation = Play::PLAY_PI / 4;
		}
		else if (angle < -Play::PLAY_PI / 4)
		{
			obj_farmer.rotation = -Play::PLAY_PI / 4;
		}
	}
	// set the farmer facing left, towards the player
	else
	{
		if (shooting)
		{
			Play::SetSprite(obj_farmer, "farmerleft_shoot2", FARMER_SHOOT_ANIMATION_SPEED);
		}
		else if (obj_farmer.velocity != Play::Vector2D{ 0, 0 })
		{
			Play::SetSprite(obj_farmer, "farmerleft_run2", FARMER_RUN_ANIMATION_SPEED);
		}
		else
		{
			Play::SetSprite(obj_farmer, "farmerleft_idle2", FARMER_IDLE_ANIMATION_SPEED);
		}

		// handle the cases where the angle is between 135 and 180 degrees, or -135 and -180 degrees. This must be done separately since the angle switches from -pi to pi.
		if (angle <= -3 * Play::PLAY_PI / 4)
		{
			obj_farmer.rotation = angle + Play::PLAY_PI;
		}
		else if (angle >= 3 * Play::PLAY_PI / 4)
		{
			obj_farmer.rotation = angle - Play::PLAY_PI;
		}
		// limit the rotation to 45 degrees
		else if (angle < 3 * Play::PLAY_PI / 4
			&& angle > 0)
		{
			obj_farmer.rotation = -Play::PLAY_PI / 4;
		}
		else //if (angle > -3 * Play::PLAY_PI / 4
			//&& angle < 0)
		{
			obj_farmer.rotation = Play::PLAY_PI / 4;
		}
	}
}

void Farmer::RecentreFarmerSprite(Play::GameObject& obj_farmer)
{
	// since the farmer is off-centre in the sprite, recentre the origin
	int currentSpriteId = obj_farmer.spriteId;
	std::string currentSpriteName = Play::GetSpriteName(currentSpriteId);
	if (currentSpriteName.find("RIGHT") != std::string::npos)
	{
		// if facing right, all centred on the left side.
		Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 17, 11 }, false);
	}
	else if (currentSpriteName.find("SHOOT") != std::string::npos)
	{
		// if shooting left, centred on the right side.
		Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 100, 11 }, false);
	}
	else
	{
		// otherwise, the left-facing animations are centred on the left side.
		Play::Graphics::SetSpriteOrigin(obj_farmer.spriteId, { 11, 11 }, false);
	}
}

void Farmer::NormaliseFarmerVelocity(Play::GameObject& obj_farmer)
{
	// if the farmer is travelling diagonally, normalise the velocity so that it moves at a constant speed
	if (obj_farmer.velocity.x != 0 && obj_farmer.velocity.y != 0)
	{
		obj_farmer.velocity.Normalize();
		obj_farmer.velocity *= FARMER_RUN_SPEED;
	}
}

void Farmer::DrawFarmer(Play::GameObject& obj_farmer)
{
	Play::UpdateGameObject(obj_farmer);

	// make the farmer "bounce" off objects
	if (Play2::RevertPositionIfNecessary(obj_farmer))
	{
		obj_farmer.velocity *= -1;
	}

	obj_farmer.scale = 1.5f; // scale the farmer sprite up
	Play::DrawObjectRotated(obj_farmer);
}
