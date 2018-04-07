/*
Copyright 2017 orangebird.
Publish under GNU General Public License v3.0 Licence.
*/
#include"game.hpp"
#include "doublefann.h"
#include"ai.hpp"
#include<iostream>
#include<time.h>
#include<fstream>
#include<algorithm>
#include<thread>
#include <cstdlib>
#include "const.h"
std::ofstream network_train("K:\\编程\\Chicken\\Debug\\train.txt");
#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\fanndoubled.lib")
#else
#pragma comment(lib,"..\\lib\\fanndouble.lib")
#endif
using namespace std;
const int siz =15;
int FANN_API test_callback(struct fann *ann, struct fann_train_data *train,
	unsigned int max_epochs, unsigned int epochs_between_reports,
	float desired_error, unsigned int epochs)
{
	printf("Epochs     %8d. MSE: %.5f. Desired-MSE: %.5f\n", epochs, fann_get_MSE(ann), desired_error);
	return 0;
}
int main()
{
	/*struct fann *ann;
	ann=fann_create_standard(num_layers, 675, 64, 64, 64, 64, 64, 225);
	fann_randomize_weights(ann, -1, 1);
	fann_save(ann, "L:\\编程\\Chicken\\Debug\\chicken.net");
	*/
	//trainner.train();
	/*bool outputStone = false;
	ifstream config("config.ini");
	if (config.is_open())
	{
		config >> outputStone;
		config.close();
	}
	cout << "Chicken K_in_A_Line test ver 3" << endl;
	cout << "Input your color(1=Black,-1=White,0=two computer):" << endl;
	int32_t myColor;
	cin >> myColor;
	cout << "Input MCTS playout(>1000):" << endl;
	int32_t playout;
	cin >> playout;
	/*
	streambuf * backup;
	ofstream fout;
	fout.open("result.txt");
	backup = cout.rdbuf();
	cout.rdbuf(fout.rdbuf());
	
	srand(time(0));
	Game<siz, siz> game;
	if (outputStone) game.outputStone(); else game.output();
	MonteCarloTreeSearchAI<siz, siz> ai;
	int32_t color = 1;
	int32_t winner = 0;
	while (!game.gameOver())
	{
		cout << (color > 0 ? "Black's turn" : "White's turn") << endl;
		size_t index;
		if (color == myColor)
		{
			int p; char q;
			cin >> q >> p;
			p = siz - p;
			q = toupper(q) - 'A';
			index = p * siz + q;
		}
		else index=ai.genMove(game, (Color)color, playout);// index = siz * siz + 2; siz * siz + 2 for fast play
		if (index == siz * siz + 1)
		{
			cout << (color > 0 ? "Black" : "White") << " resigned." << endl;
			winner = -color;
			break;
		}
		else if (index == siz * siz + 2)
			game.fastPlay((Stone)color); // For debugging
		else
			game.play(index, (Stone)color);
		if (outputStone)
			game.outputStone();
		else
			game.output();
		cout << endl;
		color = -color;
	}
	Color result = game.getResult();
	if (winner == 0)
		cout << (result == Color::BLACK ? "Black Win" : result == Color::WHITE ? "White Win" : "Tie") << endl;
	else
		cout << (winner > 0 ? "Black Win" : "White Win") << endl;
	network_train.close();
	cout << "press q to quit" << endl;
	while (getchar() != 'q');
	return 0;*/
	struct fann *ann;
	ann = fann_create_from_file(".\\chicken.net");
	cout << "Training" << endl;
	fann_train_on_file(ann, ".\\train.txt", max_epochs,epochs_between_reports, desired_error);
	fann_save(ann, ".\\chicken.net");
	return 0;
	
}