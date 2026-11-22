#include <iostream>
#include "bowling_frame.h"
#include <random>
#include <string>

using namespace std;

std::random_device dev_main;
std::mt19937 rng_main(dev_main());

int current_game_total(Bowling_Frame * first_frame)
{
	Bowling_Frame * temp = first_frame;
	int game_total = 0;
	int temp_total = 0;
	while(temp)
	{
		game_total += temp->frame_score();
		temp = temp->next_frame();
	}
	return game_total;	
}

int main(int argc, char * argv[])
{
	//check for the number of players argument
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <num_players>" << endl;
		return 1;
	}

	string arg = argv[1];
	for (unsigned int i = 0; i < arg.length(); i++)
	{
		if (arg[i] < '0' || arg[i] > '9')
		{
			cout << "Error: number of players must be a positive integer." << endl;
			cout << "Usage: " << argv[0] << " <num_players>" << endl;
			return 1;
		}
	}

	int num_players = stoi(arg);
	if (num_players < 1)
	{
		cout << "Error: number of players must be at least 1." << endl;
		return 1;
	}

	cout << "Player count is: " << num_players << endl;

	//make an array with one Bowling_Frame pointer per player
	Bowling_Frame ** players = new Bowling_Frame*[num_players];

	//make frames and link frames together
	for (int p = 0; p < num_players; p++)
	{
		players[p] = new Bowling_Frame(1);
		Bowling_Frame * prev = players[p];

		for (int f = 2; f <= 10; f++)
		{
			Bowling_Frame * frame = new Bowling_Frame(f);
			prev->next_frame(frame);
			frame->prev_frame(prev);
			prev = frame;
		}
	}

	//set frame scores: each call is one roll, and a game has at most 21 rolls
	for (int p = 0; p < num_players; p++)
	{
		for (int r = 0; r < 21; r++)
			players[p]->update_scores();
	}

	//iterate through linked list and print values
	for (int p = 0; p < num_players; p++)
	{
		Bowling_Frame * traverse = players[p];
		
		while(traverse)
		{
			cout << "Frame " << traverse->frame_num() << ": " << endl
			 	 << " Score 1: " << traverse->score1() << endl
			 	 << " Score 2: " << traverse->score2() << endl;

			if (traverse->bonus_count() >= 1)
				cout << " Bonus Roll 1: " << traverse->bonus_roll1() << endl;
			if (traverse->bonus_count() >= 2)
				cout << " Bonus Roll 2: " << traverse->bonus_roll2() << endl;

			cout << " Frame Score: " << traverse->frame_score() << endl;

			traverse = traverse->next_frame();
		}

		cout << endl << "Total Game Score for player " << p + 1 << ": "
			 << current_game_total(players[p]) << endl << endl;
	}

	//delete frames
	for (int p = 0; p < num_players; p++)
	{
		Bowling_Frame * temp = players[p];
		while (temp)
		{
			Bowling_Frame * next = temp->next_frame();
			delete temp;
			temp = next;
		}
	}
	delete [] players;

}