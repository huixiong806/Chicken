/*
Copyright 2017 orangebird.
Publish under GNU General Public License v3.0 Licence.
*/
#include"game.hpp"
#include "fann\\doublefann.h"
#include"ai.hpp"
#include<iostream>
#include<time.h>
#include<fstream>
#include<algorithm>
#include<thread>
#include <cstdlib>
#include "const.h"
#include <string>
bool new_game_1 = true;
bool new_game_2 = true;
std::ofstream network_train(".\\train.tmp");
#ifdef _DEBUG
#pragma comment(lib,"..\\lib\\fanndoubled.lib")
#else
#pragma comment(lib,"..\\lib\\fanndouble.lib")
#endif
using namespace std;
int32_t count_move=0;
const int siz =15;
int main()
{
	/*struct fann *ann;
	ann=fann_create_standard(num_layers, 675 ,64,64,64,64,64,225);
	fann_randomize_weights(ann, -1, 1);
	fann_save(ann, ".\\chicken.net");
	*/
	int count_train=0;
	bool outputStone = false;
	ifstream config("config.ini");
	if (config.is_open())
	{
		config >> outputStone;
		config.close();
	}
	cout << "Chicken K_in_A_Line test ver 4" << endl;
	cout << "Input your color(1=Black,-1=White,0=two computer,2=Train The Network):" << endl;
	int32_t myColor;
	cin >> myColor;
	bool train=false;
	if(myColor==2){
		train=true;
		myColor=0;
	}
	int32_t playout;	
	if(!train){
		cout << "Input MCTS playout(>1000):" << endl;
		cin >> playout;
	}else{
		playout=train_playout;
	}
	/*
	streambuf * backup;
	ofstream fout;
	fout.open("result.txt");
	backup = cout.rdbuf();
	cout.rdbuf(fout.rdbuf());
	*/
	do{
		new_game_1 = true;
		new_game_2 = true;
		count_move = 0;
		srand(time(0));
		Game<siz, siz> game;
		if (outputStone) game.outputStone(); else game.output();
		MonteCarloTreeSearchAI<siz, siz> ai;
		int32_t color = 1;
		int32_t winner = 0;
		while (!game.gameOver())
		{
			count_move = 0;
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
			else {
				index=ai.genMove(game, (Color)color, playout);// index = siz * siz + 2; siz * siz + 2 for fast play
				if(index!=siz*siz+1) count_move++;
			}
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
		if (train){
			string tstr;
			network_train.close();
			std::ifstream inf(".\\train.tmp");
			std::ofstream outf(".\\train.txt");
			outf<<count_move<<" "<<675<<" "<<225<<endl;
			while (!inf.eof()){
				getline(inf,tstr);
				outf<<tstr<<endl;
			}
			outf.close();
			inf.close();
			struct fann *ann;
			ann = fann_create_from_file(".\\chicken.net");
			fann_set_training_algorithm(ann,FANN_TRAIN_BATCH);
			cout << "Training" << endl;
			fann_train_on_file(ann, ".\\train.txt", max_epochs,epochs_between_reports, desired_error);
			cout << "Saving" <<endl;
			fann_save(ann, ".\\chicken.net");
			cout<<"Have trained:"<<++count_train<<endl;
		}
	}while(train);
	cout << "press q to quit" << endl;
	while (getchar() != 'q');
	return 0;
	
}
