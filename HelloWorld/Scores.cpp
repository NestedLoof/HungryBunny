#include "Scores.h"

// save the score to the high scores list if it is one of the top 5
void Scores::SaveScore()
{
	// don't save a score of 0
	if (gameState.score == 0)
	{
		return;
	}

	// check if the score is a new high score so we can display this on the game over screen
	std::vector<int> highScores = GetHighScores();
	if (highScores.size() != 0
		&& gameState.score > highScores[0])
	{
		IS_HIGH_SCORE = true;
	}

	// add the new score and sort scores in descending order
	highScores.push_back(gameState.score);
	sort(highScores.begin(), highScores.end(), std::greater<int>());

	// write the top scores to file
	WriteScores(highScores);
}

std::string Scores::GetFileName()
{
	switch (gameState.difficulty)
	{
	case DIFFICULTY_EASY:
		return "easyscores.txt";
	case DIFFICULTY_MEDIUM:
		return "mediumscores.txt";
	case DIFFICULTY_HARD:
		return "hardscores.txt";
	case DIFFICULTY_HUNGRY:
		return "hungryscores.txt";
	}
}

// Get a list of high scores in descending order
std::vector<int> Scores::GetHighScores()
{
	std::vector<int> scores;

	try
	{
		// open the file to read from
		std::ifstream infile(GetFileName());
		if (!infile.is_open())
		{
			std::cout << "Error opening " << GetFileName() << std::endl;
			return scores;
		}

		// loop through each score in the file and add to the scores vector
		while (infile.good())
		{
			std::string s;
			if (!getline(infile, s, ','))
			{
				break;
			}

			int score = std::stoi(s);
			scores.push_back(score);
		}
		infile.close();

		// sort scores in descending order before returning
		sort(scores.begin(), scores.end(), std::greater<int>());
	}
	catch (...)
	{
		std::cout << "Error getting scores" << std::endl;
	}

	return scores;
}

// Write scores to file as a comma separated list
void Scores::WriteScores(std::vector<int> scores)
{
	try
	{
		// open the file to write to
		std::ofstream outfile(GetFileName());
		if (!outfile.is_open())
		{
			std::cout << "Error opening " << GetFileName() << std::endl;
			return;
		}

		// write each score
		int numScores = scores.size();
		for (int i = 0; i < std::min(MAX_NUM_HIGH_SCORES, numScores); i++)
		{
			outfile << scores[i];

			// append a comma if not the last score
			if (i < std::min(MAX_NUM_HIGH_SCORES, numScores) - 1)
			{
				outfile << ",";
			}
		}
		outfile.close();
	}
	catch (...)
	{
		std::cout << "Error saving score" << std::endl;
	}
}