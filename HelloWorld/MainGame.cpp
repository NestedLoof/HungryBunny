#pragma once
#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Play.h"
#include "Utilities.h"
#include "Bunny.h"
#include "Farmer.h"
#include "Interface.h"
#include "Play2.h"
#include "GameLogic.h"

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play2::InitDisplaySize();
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CentreAllSpriteOrigins();

	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::StartAudioLoop("bird_water"); // ambient noise
	Play::Audio::SetLoopingSoundVolume("bird_water", MUSIC_VOLUME);

	Bunny::InitialiseBunny();
	Farmer::InitialiseFarmers();
	Interface::InitialiseGui();
	Interface::InitialiseMenus();
	GameLogic::InitialiseEnvironment();
	GameLogic::InitialisePlots();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	Play::DrawBackground();

	Bunny::UpdateBunny();

	// only do the game related things after the start menu
	if (gameState.bunnyState != STATE_START_GAME)
	{
		GameLogic::UpdateVegetables();
		Farmer::UpdateFarmers();
		GameLogic::UpdateFlowers();
		Farmer::UpdateBullets();
		Interface::DisplayInterface();
	}

	GameLogic::UpdateDestroyed();

	Play::PresentDrawingBuffer();
	return Play::KeyDown(KEY_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

