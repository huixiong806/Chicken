/*
Copyright 2017 orangebird.
Publish under GNU General Public License v3.0 Licence.
*/
#include"game.hpp"
#include"ai.hpp"
#include<iostream>
#include<time.h>
#include<fstream>
#include<algorithm>
#include<thread>
using namespace std;
const int siz =15;
int main()
{
	bool outputStone = true;
	ifstream config("config.ini");
	if (config.is_open())
	{
		config >> outputStone;
		config.close();
	}
	cout << "Chicken K_in_A_Line test ver 2" << endl;
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
	*/
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
		else index=ai.genMove(game,(Color)color,playout);
		if (index == siz * siz + 1)
		{
			cout << (color > 0 ? "Black" : "White") << " resigned." << endl;
			winner = -color;
			break;
		}
		if (index == siz * siz + 2) game.fastPlay((Stone)color); // For debugging
		else game.play(index, (Stone)color);
		if (outputStone) game.outputStone(); else game.output();
		cout << endl;
		color = -color;
	}
	Color result = game.getResult();
	if(winner==0)
	cout << (result==Color::BLACK ? "Black Win" : result == Color::WHITE?"White Win":"Tie") << endl;
	else cout <<(winner > 0 ? "Black Win" : "White Win") << endl;
	cout << "press q to quit" << endl;
	while (getchar()!='q');
	return 0;
}