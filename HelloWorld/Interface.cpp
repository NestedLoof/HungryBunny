#include "Interface.h"

// create the menu objects up front, but only display them when the correct menu is being displayed
void Interface::InitialiseMenus()
{
	// start menu buttons
	CreateMenuButton("easy", { 2 * DISPLAY_WIDTH / 13, 50 });
	CreateMenuButton("medium", { 5 * DISPLAY_WIDTH / 13, 50 });
	CreateMenuButton("hard", { 8 * DISPLAY_WIDTH / 13, 50 });
	CreateMenuButton("hungry", { 11 * DISPLAY_WIDTH / 13, 50 });

	// game over menu buttons
	CreateMenuButton("restart", { DISPLAY_WIDTH / 4, 50 });
	CreateMenuButton("scores", { 2 * DISPLAY_WIDTH / 4, 50 });
	CreateMenuButton("mainmenu", { 3 * DISPLAY_WIDTH / 4, 50 });

	// high score menu buttons
	CreateMenuButton("back", { DISPLAY_WIDTH / 2, 50 });
}

void Interface::CreateMenuButton(std::string buttonName, Play::Point2D pos)
{
	int buttonId = Play::CreateGameObject(TYPE_BUTTON, pos, 0, "button_medium_up");
	Play::GameObject& obj_button = Play::GetGameObject(buttonId);
	obj_button.scale = 3;
	MAP_BUTTON_NAME_TO_ID.insert({ buttonName, buttonId });
}

void Interface::DisplayInterface()
{
	// when the player is alive, display the GUI
	if (gameState.bunnyState == STATE_PLAY)
	{
		DisplayGUI();
		return;
	}

	// only display the menus when the player dies
	if (gameState.menuState == MENU_GAME_OVER)
	{
		DisplayGameOverMenu();
	}
	else if (gameState.menuState == MENU_HIGH_SCORES)
	{
		DisplayHighScoresMenu();
	}
}

void Interface::DisplayStartMenu()
{
	// display game name and instructions
	Play2::DrawFontTextWithScale("32px", "HUNGRY BUNNY", { DISPLAY_WIDTH / 2, 260 }, Play::CENTRE, 0.7);
	Play2::DrawFontTextWithScale("64px", "Use WASD keys to move", { DISPLAY_WIDTH / 2, 190 }, Play::CENTRE, 0.4);
	Play2::DrawFontTextWithScale("64px", "Hold SHIFT to run", { DISPLAY_WIDTH / 2, 150 }, Play::CENTRE, 0.4);
	Play2::DrawFontTextWithScale("64px", "Press SPACE to plant flowers", { DISPLAY_WIDTH / 2, 110 }, Play::CENTRE, 0.4);

	// draw the buttons for the text to sit on top of
	int easyButtonId = MAP_BUTTON_NAME_TO_ID["easy"];
	Play::GameObject& obj_easyButton = Play::GetGameObject(easyButtonId);
	Play::DrawObjectRotated(obj_easyButton);

	int mediumButtonId = MAP_BUTTON_NAME_TO_ID["medium"];
	Play::GameObject& obj_mediumButton = Play::GetGameObject(mediumButtonId);
	Play::DrawObjectRotated(obj_mediumButton);

	int hardButtonId = MAP_BUTTON_NAME_TO_ID["hard"];
	Play::GameObject& obj_hardButton = Play::GetGameObject(hardButtonId);
	Play::DrawObjectRotated(obj_hardButton);

	int hungryButtonId = MAP_BUTTON_NAME_TO_ID["hungry"];
	Play::GameObject& obj_hungryButton = Play::GetGameObject(hungryButtonId);
	Play::DrawObjectRotated(obj_hungryButton);

	// display and handle button interactions
	if (DisplayButton(obj_easyButton, "Easy", 0.75))
	{
		GameLogic::RestartGame(Difficulty::DIFFICULTY_EASY);
		Play::PlayAudio("play");
		return;
	}

	if (DisplayButton(obj_mediumButton, "Medium", 0.75))
	{
		GameLogic::RestartGame(Difficulty::DIFFICULTY_MEDIUM);
		Play::PlayAudio("play");
		return;
	}

	if (DisplayButton(obj_hardButton, "Hard", 0.75))
	{
		GameLogic::RestartGame(Difficulty::DIFFICULTY_HARD);
		Play::PlayAudio("play");
		return;
	}

	if (DisplayButton(obj_hungryButton, "Hungry", 0.75))
	{
		GameLogic::RestartGame(Difficulty::DIFFICULTY_HUNGRY);
		Play::PlayAudio("play");
		return;
	}
}

void Interface::DisplayGUI()
{
	UpdateHealthBar();
	UpdateScoreDisplay();
}

void Interface::DisplayGameOverMenu()
{
	// display the game over and score text
	Play::DrawFontText("32px", "GAME OVER", { DISPLAY_WIDTH / 2, 250 }, Play::CENTRE);
	std::string scoreText = "Score: " + std::to_string(gameState.score);
	if (IS_HIGH_SCORE)
	{
		scoreText += " (HIGH SCORE!)";
	}
	Play::DrawFontText("64px", scoreText, { DISPLAY_WIDTH / 2, 150 }, Play::CENTRE);

	// draw the buttons for the text to sit on top of
	int restartButtonId = MAP_BUTTON_NAME_TO_ID["restart"];
	Play::GameObject& obj_restartButton = Play::GetGameObject(restartButtonId);
	Play::DrawObjectRotated(obj_restartButton);

	int scoresButtonId = MAP_BUTTON_NAME_TO_ID["scores"];
	Play::GameObject& obj_scoresButton = Play::GetGameObject(scoresButtonId);
	Play::DrawObjectRotated(obj_scoresButton);

	int mainMenu = MAP_BUTTON_NAME_TO_ID["mainmenu"];
	Play::GameObject& obj_mainMenuButton = Play::GetGameObject(mainMenu);
	Play::DrawObjectRotated(obj_mainMenuButton);

	// display and handle button interactions
	if (DisplayButton(obj_restartButton, "Play Again", 0.45))
	{
		GameLogic::RestartGame(gameState.difficulty);
		Play::PlayAudio("play");
		return;
	}

	if (DisplayButton(obj_scoresButton, "High Scores", 0.45))
	{
		gameState.menuState = MENU_HIGH_SCORES;
		Play::PlayAudio("button");
		return;
	}

	if (DisplayButton(obj_mainMenuButton, "Main Menu", 0.45))
	{
		gameState.bunnyState = STATE_START_GAME;
		Play::PlayAudio("button");
		return;
	}
}

void Interface::DisplayHighScoresMenu()
{
	// draw a back button for the text to sit on top of
	int backButtonId = MAP_BUTTON_NAME_TO_ID["back"];
	Play::GameObject& obj_backButton = Play::GetGameObject(backButtonId);
	Play::DrawObjectRotated(obj_backButton);

	// display the high scores text
	Play::DrawFontText("64px", "HIGH SCORES (" + GetDifficultyString() + ")", { DISPLAY_WIDTH / 2, 300 }, Play::CENTRE);

	std::vector<int> highScores = Scores::GetHighScores();
	if (highScores.size() == 0)
	{
		// handle case where there are no high scores yet
		Play2::DrawFontTextWithScale("64px", "No high scores yet!", { DISPLAY_WIDTH / 2, 200 }, Play::CENTRE, 0.5);
	}
	else
	{
		// display all high scores, numbered
		int yPos = 240;
		for (int i = 0; i < highScores.size(); i++)
		{
			Play2::DrawFontTextWithScale("64px", std::to_string(i + 1) + ". " + std::to_string(highScores[i]), { DISPLAY_WIDTH / 2, yPos }, Play::CENTRE, 0.4);
			yPos -= 30;
		}
	}

	// just a back button to return to the game over screen
	if (DisplayButton(obj_backButton, "Back"))
	{
		gameState.menuState = MENU_GAME_OVER;
		Play::PlayAudio("button");
		return;
	}
}

// gets the difficulty string for the current difficulty to display
std::string Interface::GetDifficultyString()
{
	switch (gameState.difficulty)
	{
	case DIFFICULTY_EASY:
		return "Easy";
	case DIFFICULTY_MEDIUM:
		return "Medium";
	case DIFFICULTY_HARD:
		return "Hard";
	case DIFFICULTY_HUNGRY:
		return "Hungry";
	}
}

bool Interface::DisplayButton(Play::GameObject& obj_button, std::string buttonText, float scale)
{
	// draw the button and text
	std::string buttonSpriteName = Play::GetSpriteName(obj_button.spriteId);
	bool buttonPressed = buttonSpriteName.find("DOWN") != std::string::npos;
	Play::Point2D buttonTextPos = { obj_button.pos.x, buttonPressed ? obj_button.pos.y - 5 : obj_button.pos.y }; // move the restart button text down a few pixels when it's pressed down
	Play2::DrawFontTextWithScale("64px", buttonText, buttonTextPos, Play::CENTRE, scale);

	// return true when the player releases the click on the restart button
	bool leftClickReleased = !Play::Input::GetMouseDown(Play::Input::MouseButton::BUTTON_LEFT);
	if (buttonPressed && leftClickReleased)
	{
		Play::SetSprite(obj_button, "button_medium_up", 0);
		return true;
	}

	// check if the player is currently clicking the restart button, and if so change the sprite to the pressed down version
	if (ObjectClickedOn(obj_button, Play::Input::MouseButton::BUTTON_LEFT))
	{
		Play::SetSprite(obj_button, "button_medium_down", 0);
	}
	else
	{
		Play::SetSprite(obj_button, "button_medium_up", 0);
	}

	// button was not pressed (and released)
	return false;
}

bool Interface::ObjectClickedOn(Play::GameObject& obj, Play::Input::MouseButton button)
{
	// checks if the given mouse button has been clicked and the mouse position is within the bounds of the object sprite
	Play::Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId) * obj.scale;
	Play::Vector2f spritePos = obj.pos;
	Play::Point2D mousePos = Play::GetMousePos();
	return Play::Input::GetMouseDown(button)
		&& mousePos.x > spritePos.x - spriteSize.width / 2
		&& mousePos.x < spritePos.x + spriteSize.width / 2
		&& mousePos.y > spritePos.y - spriteSize.height / 2
		&& mousePos.y < spritePos.y + spriteSize.height / 2;
}

void Interface::InitialiseGui()
{
	// add health bar background 
	Play::CreateGameObject(TYPE_HEALTH_BACKGROUND, { (26 * 16) ,(22 * 16) - 8 }, 0, "health_background_tall");

	// creates 160 columns of health bar in different x positions
	// only draw as many as we need to display the current health later
	Play::GameObject& obj_health_bkgr = Play::GetGameObjectByType(TYPE_HEALTH_BACKGROUND);
	Play::Point2D healthBarPos = obj_health_bkgr.pos;
	for (int i = -80; i < 80; i += 1)
	{
		Play::CreateGameObject(TYPE_HEALTH_BAR, { (healthBarPos.x + i), healthBarPos.y }, 0, "healthbar_tall");
	}

	// create score display background. Draw the text on top each frame later
	Play::CreateGameObject(TYPE_SCORE_BACKGROUND, { (36 * 16) ,(22 * 16) - 8 }, 0, "score_background_tall");
}

void Interface::UpdateHealthBar()
{
	// take away up to 1 health each frame depending on the current action
	if (ShouldDepleteHealth())
	{
		gameState.health--;
	}

	//if the health bar is empty, game over
	if (gameState.health <= 0)
	{
		GameLogic::GameOver();
		return;
	}

	DrawHealthBar();
}

void Interface::DrawHealthBar()
{
	// draw the health bar background first
	Play::GameObject& obj_health_bkgr = Play::GetGameObjectByType(TYPE_HEALTH_BACKGROUND);
	Play::DrawObject(obj_health_bkgr);

	// to calculate how many health bars to draw, find the fraction of health bar remaining (healthBarCnt / MAX_HEALTH) and multiply by the total number of health bars
	std::vector<int> vHealthBarIDs = Play::CollectGameObjectIDsByType(TYPE_HEALTH_BAR);
	float numHealthBarsToDraw = gameState.health * vHealthBarIDs.size() / MAX_HEALTH;

	// use that to calculate how many health bars to draw, then draw them
	for (int i = 0; i < numHealthBarsToDraw; i++)
	{
		int healthBarId = vHealthBarIDs[i];
		Play::GameObject& obj_healthbar = Play::GetGameObject(healthBarId);
		Play::DrawObjectRotated(obj_healthbar, GetHealthBarOpacity());
	}
}

float Interface::GetHealthBarOpacity()
{
	// flash the health bar slowly below 35% health, then faster below 15% health
	// to make a low health flashing effect, half the opacity of the health bar when the player is low on health every other 30 or 15 frames
	float healthOpacity = 1.0f;
	float healthPercentage = gameState.health * 100 / MAX_HEALTH;
	if (healthPercentage < 35
		&& healthPercentage > 15
		&& Play::frameCount % 60 < 30)
	{
		// between 35% and 15% health
		healthOpacity = 0.5f;
		if (Play::frameCount % 60 == 1)
		{
			// if the previous audio with the same name isn't stopped, it will continually get quieter every time it's played
			Play::Audio::StopSound("countdownslow"); 
			Play::Audio::StartSound("countdownslow", false, 0.5f);
		}
	}
	else if (healthPercentage <= 15
		&& Play::frameCount % 30 < 15)
	{
		// below 15% health
		healthOpacity = 0.5f;
		if (Play::frameCount % 30 == 1)
		{
			// if the previous audio with the same name isn't stopped, it will continually get quieter every time it's played
			Play::Audio::StopSound("countdownfast"); 
			Play::Audio::StartSound("countdownfast", false, 0.5f);
		}
	}

	return healthOpacity;
}

void Interface::UpdateScoreDisplay()
{
	// draw the background first
	Play::GameObject& obj_score_bkgr = Play::GetGameObjectByType(TYPE_SCORE_BACKGROUND);
	Play::DrawObject(obj_score_bkgr);

	// display the player's current game score
	Play2::DrawFontTextWithScale(
		"72px",
		"SCORE: " + std::to_string(int(gameState.score)),
		{ obj_score_bkgr.pos.x + 32,obj_score_bkgr.pos.y },
		Play::RIGHT,
		0.25
	);
}

bool Interface::ShouldDepleteHealth()
{
	// if the bunny is running, deplete health every frame
	Play::GameObject& obj_bunny = Play::GetGameObjectByType(TYPE_BUNNY);
	std::string spriteName = Play::GetSpriteName(obj_bunny.spriteId);
	if (spriteName.find("RUN") != std::string::npos)
	{
		return true;
	}

	// if the bunny is walking, only deplete health every 2 frames
	if (spriteName.find("WALK") != std::string::npos
		&& Play::frameCount % 2 == 0)
	{
		return true;
	}

	// if the bunny is not moving, only deplete health every 4 frames
	if (obj_bunny.velocity == Play::Vector2D{ 0, 0 }
		&& Play::frameCount % 4 == 0)
	{
		return true;
	}

	// don't deplete health this frame
	return false;
}
