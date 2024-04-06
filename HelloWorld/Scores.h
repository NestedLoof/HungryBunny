#pragma once
#include "Play.h"
#include "Utilities.h"
#include <iostream>
#include <fstream>

// reads and writes high scores to the corresponding files
class Scores
{
public:
	//! @brief Saves the score to the list of high scores (if top 5) and sets IS_HIGH_SCORE to true if necessary.
	static void SaveScore();
	//! @brief Gets the high scores from file.
	static std::vector<int> GetHighScores();
	//! @brief Write scores to file.
	static void WriteScores(std::vector<int> scores);
	//! @brief Gets the name of the file based on the difficulty.
	static std::string GetFileName();

	// how many high scores to keep
	inline static int MAX_NUM_HIGH_SCORES = 5;
};