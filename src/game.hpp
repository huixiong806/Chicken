/*
Copyright 2017 orangebird.
Publish under GNU General Public License v3.0 Licence.
*/
#ifndef GAME_HPP
#define GAME_HPP
#include "Board.hpp"
#include "const.h"
#include <set>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cassert>
#include "hash.hpp"
const double value[2][4] =
{ {0.3,0.5,3.0,10.0},
  {0.2,0.4,2.0,20.0}
};
enum class PutStoneResult :bool
{
	NOTALLOWED,
	ACCEPTED,
};
double calc_in[675];
double *calc_out = nullptr;
template<size_t row, size_t col>
class Game
{
public:
	Board<row, col> board;
private:
	int32_t winK, fastPlayRange;
	bool blackPass, whitePass;
	int32_t* distField;
	int32_t* estimateValue;
protected:
public:
	int32_t getWink()
	{
		return winK;
	}
	Game()
	{
		winK = 5;
		fastPlayRange = 2;
		blackPass = false;
		whitePass = false;
		distField = new int32_t[row*col];
		estimateValue = new int32_t[405];
	}
	Game(int32_t* estimateValue_)
	{
		winK = 5;
		fastPlayRange = 2;
		blackPass = false;
		whitePass = false;
		distField = new int32_t[row*col];
		estimateValue = new int32_t[405];
		copy(estimateValue_, estimateValue_ + 405, estimateValue);
	}
	Game(const Game& r) :board(r.board), winK(r.winK), fastPlayRange(r.fastPlayRange), blackPass(r.blackPass), whitePass(r.whitePass)
	{
		distField = new int32_t[row*col];
		estimateValue = new int32_t[405];
		copy(r.distField, r.distField + row * col, distField);
		copy(r.estimateValue, r.estimateValue + row * col, estimateValue);
	}
	~Game()
	{
		delete[] distField;
		delete[] estimateValue;
	}
	bool isEmpty(size_t index)
	{
		if (index == row * col) return true;
		return board.getGridColor(index) == Stone::EMPTY;
	}
	bool inside(int r, int c)
	{
		return r >= 0 && r < row && c >= 0 && c < col;
	}
	bool canPlay(int r, int c)
	{
		if (!inside(r, c)) return false;
		return isEmpty(r*col + c);
	}
	std::vector<int> getNearPositions(int dist)
	{
		std::queue<size_t> q;
		int* d = distField;

		memset(d, -1, sizeof(int)*row*col);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				if (board.getGridColor(i, j) != Stone::EMPTY)
				{
					q.push(i*col+j);
					d[i*col + j] = 0;
				}

		std::vector<int> res;

		while (!q.empty()) {
			size_t p = q.front(); q.pop();
			int dd = d[p];
			if (dd >= dist) break;

			if (dd > 0) res.push_back(p);

			const int rd[8] = { 1,0,-1,0,1,1,-1,-1 }, cd[8] = { 0,1,0,-1,1,-1,1,-1 };
			for (int k = 0; k < 8; k++) {
				int rr = p/col + rd[k], cc = p%col + cd[k];
				if (inside(rr, cc) && d[rr*col + cc] == -1) {
					q.push(rr*col+cc);
					d[rr*col + cc] = dd + 1;
				}
			}
		}

		// Center position as beginning
		if (res.empty()) res.push_back(row / 2 * col + col / 2);

		return res;
	}
	// {length, points}
	// isNextMove: is it the color's turn?
	std::pair<int, std::vector<int>> getCriticalPoints(Color color, bool isNextMove)
	{
		// "x" for color `color`, "o" for opposite color
		// List in the same direction as the `while` loop
		const int rd[4] = { 1,0,1,1 };
		const int cd[4] = { 0,1,1,-1 };
		int length = -1;
		std::vector<int> res;
		for (int i = 0; i < row && length == -1; i++)
			for (int j = 0; j < col && length == -1; j++)
				for (int k = 0; k < 4 && length == -1; k++)
				{
					int ir = i - rd[k], ic = j - cd[k];
					if (inside(ir, ic) && board.getGridColor(ir, ic) == color) continue;
					int r = i, c = j, cnt = 0;
					while (cnt < winK && inside(r, c) && board.getGridColor(r, c) == color)
						r += rd[k], c += cd[k], cnt++;
					if (cnt == winK - 1) // xxxx
					{
						if (canPlay(r, c)) // xxxx.
						{
							length = 4;
							res.push_back(r*col + c);
						}
						else if (canPlay(ir, ic)) // .xxxx
						{
							length = 4;
							res.push_back(ir*col + ic);
						}
						continue;
					}
					if (cnt == winK - 2) // xxx
					{
						int r2 = r + rd[k], c2 = c + cd[k];
						int ir2 = ir - rd[k], ic2 = ic - cd[k];
						bool f = canPlay(r, c), f2 = canPlay(r2, c2);
						bool i = canPlay(ir, ic), i2 = canPlay(ir2, ic2);
						if (f && i) {
							if (!f2 && !i2) continue;
							length = 3;
							if (f2 && i2) // ..xxx..
							{
								res.push_back(ir*col + ic);
								res.push_back(r*col + c);
							}
							else if (f2 && !i2 && isNextMove) // o.xxx..
							{
								res.push_back(r*col + c);
							}
							else if (f2 && !i2 && !isNextMove) // o.xxx..
							{
								res.push_back(ir*col + ic);
								res.push_back(r*col + c);
								res.push_back(r2*col + c2);
							}
							else if (!f2 && i2 && isNextMove) // ..xxx.o
							{
								res.push_back(ir*col + ic);
							}
							else if (!f2 && i2 && !isNextMove) // ..xxx.o
							{
								res.push_back(ir2*col + ic2);
								res.push_back(ir*col + ic);
								res.push_back(r*col + c);
							}
						}
					}
				}
		return std::make_pair(length, res);
	}
	// {opposite, self}
	/*int32_t*/
	void calcNetwork(Color color) {
		static struct fann *ann=fann_create_from_file(".\\Chicken.net");
		static int count = 0;
		//cout << count<<endl;
		count++;
		for (int i = 0; i < 225; i++) {
			if (board.getGridColor(i) == Stone::BLACK) calc_in[i] = 1;
			else calc_in[i] = 0;
		}
		for (int i = 0; i < 225; i++) {
			if (board.getGridColor(i) == Stone::WHITE) calc_in[225 + i] = 1;
			else calc_in[225 + i] = 0;
		}
		for (int i = 0; i < 225; i++) {
			if (color == Stone::BLACK) calc_in[225 * 2 + i] = 1;
			else calc_in[225 * 2 + i] = 1;
		}
		calc_out = fann_run(ann, calc_in);
	}
	double calcScore(Color color, int r, int c, const int32_t* estimateValue_=0)
	{
		/*assert(winK <= row && winK <= col);
		const int rd[4] = { 1,0,1,1 };
		const int cd[4] = { 0,1,1,-1 };
		//const int selfScores[6] = { 0,300,500,3000,10000,0 } ;
		//const int oppositeScores[6] = { 0,200,400,2000,20000,0 };
		const int pow3[5] = { 1,3,9,27,81 };
		Color opposite = color == Color::BLACK ? Color::WHITE : Color::BLACK;
		//pair<int, int> res;
		int32_t res=0;
		for (int k = 0; k < 4; k++)
		{
			/*
			  state为一个"5连"的状态压缩形式
			  空位的位置*81+第1个非空位置的棋子颜色*27+...+第4个非空位置的棋子颜色
			  0表示白棋，1表示空，2表示黑棋
			
			size_t state = 0;
			int cnt = 0, cnto = 0;
			//printf("> k = %d\n", k);
			int rr = r - rd[k] * (winK - 1), cc = c - cd[k] * (winK - 1); // Current position
			int r1 = r + rd[k] * winK, c1 = c + cd[k] * winK;
			while (!inside(rr, cc)) rr += rd[k], cc += cd[k];
			bool insideFlag = true;
			int32_t target=5;
			for (int i = 0; i < winK - 1; i++)
			{
				if (rr == r&&cc == c)//正好是所在空位
					target = i+1;
				else state = (state/81*81)+((state%81)*3+(((int32_t)board.getGridColor(rr, cc)*(int32_t)color) + 1))%81;
				//printf(" %d %d [pre]\n", rr, cc);
				rr += rd[k], cc += cd[k];
				if (!inside(rr, cc))
				{
					insideFlag = false;
					break;
				}
			}
			if (!insideFlag)continue;
			do
			{
				target--;
				state = (target * 81) + ((state % 81) * 3 + (((int32_t)board.getGridColor(rr, cc)*(int32_t)color) + 1)) % 81;
				res+= estimateValue_[state];
				//printf(" %d %d - %d %d [sum] %d %d\n", rr - (winK - 1) * rd[k], cc - (winK - 1) * cd[k], rr, cc, cnto, cnt);
				rr += rd[k], cc += cd[k];
			} while (inside(rr, cc) && (rr != r1 || cc != c1));
		}
		return res;*/

		return calc_out[r*col + c];
	}
	int fastDecision(Color color,const int32_t* estimateValue_=0)
	{
		// Critical points test
		/*
		Color opposite = color == Color::BLACK ? Color::WHITE : Color::BLACK;
		std::pair<int, std::vector<int>> c = getCriticalPoints(color, true), co = getCriticalPoints(opposite, false);
		if (c.first == 4 && c.second.size() == 1)
			return c.second[0];
		else if (co.first == 4 && co.second.size() == 1)
			return co.second[0];
		else if (c.first == 3)
			return c.second[0];
		else if (co.first == 3)
			return co.second[rand() % co.second.size()]; // I just randomized here...
		*/
		// Select by score
		int index = row * col + 1;
		double maxScore = -1;
		calcNetwork(color);
		std::vector<int32_t> indices = getNearPositions(fastPlayRange);//, pool;
		for (auto i : indices)
		{
			double score = calcScore(color, i / col, i % col, estimateValue_);
			//if (score.first < score.second) std::swap(score.first, score.second);
			if (score > maxScore)
			{
				//cout << i / col << " " << i%col << endl;
				maxScore = score;
				index = i;
				//pool.clear();
				//pool.push_back(i);
			}
			//else if (score == maxScore)
				//pool.push_back(i);
		}
		//if (!pool.empty()) index = pool[rand() % pool.size()];
		/*
		if (index != row * col + 1)
		{
		pair<int, int> score = calcScore((Stone)color, index / col, index % col);
		printf("Score@(%d, %d) [opposite / self]: %d / %d\n", index / col, index % col, score.first, score.second);
		}
		*/
		return index;
	}
	void fastPlay(Color color)
	{
		play(fastDecision(color, estimateValue), color);
	}
	void fastPlay(Color color,const int32_t* estimateValue_)
	{
		play(fastDecision(color, estimateValue_), color);
	}
	bool gameOver()
	{
		if (blackPass && whitePass) return true;
		if (getResult() != Color::EMPTY) return true;
		for (size_t index = 0; index < row*col; ++index)
			if (board.getGridColor(index) == Stone::EMPTY)
				return false;
		return true;
	}
	bool isWin(Color color)
	{
		const int rd[4] = { 1,0,1,1 };
		const int cd[4] = { 0,1,1,-1 };
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				for (int k = 0; k < 4; k++)
				{
					int ir = i - rd[k], ic = j - cd[k];
					if (inside(ir, ic) && board.getGridColor(ir, ic) == color) continue;
					int r = i, c = j, cnt = 0;
					while (cnt < winK && inside(r, c) && board.getGridColor(r, c) == color)
						r += rd[k], c += cd[k], cnt++;
					if (cnt == winK) return true;
				}
		return false;
	}
	// 获取胜利的一方(0表示没有人胜利)
	Color getResult()
	{
		if (isWin(Color::BLACK))
			return Color::BLACK;
		if (isWin(Color::WHITE))
			return Color::WHITE;
		return Color::EMPTY;
	}
	// 落子
	PutStoneResult play(size_t index, Color color)
	{
		if (index == row * col)
		{
			if (color == Color::WHITE)
				whitePass = true;
			else if (color == Color::BLACK)
				blackPass = true;
			return PutStoneResult::ACCEPTED;
		}
		if (isEmpty(index))
		{
			board[index / col][index%col] = color;
			whitePass = blackPass = false;
			return PutStoneResult::ACCEPTED;
		}
		return PutStoneResult::NOTALLOWED;
	}
	Stone getStone(size_t x, size_t y)
	{
		return board[x][y];
	}
	Board<row, col>& getBoard()
	{
		return board;
	}
	void output()
	{
		std::cout << "  ";
		for (int j = 0; j < col; ++j)
		{
			std::cout << " " << (char)(j + 'A');
		}
		std::cout << std::endl;
		for (int i = 0; i < row; ++i)
		{
			if ((row - i) < 10)
				std::cout << " " << (row - i);
			else std::cout << (row - i);
			for (int j = 0; j < col; ++j)
			{
				if (board[i][j] == Stone::EMPTY)
				{
					std::cout << " .";
				}
				else if (board[i][j] == Stone::BLACK)
				{
					std::cout << " x";
				}
				else std::cout << " o";
			}
			if ((row - i) < 10)
				std::cout << " " << (row - i);
			else std::cout << (row - i);
			std::cout << std::endl;
		}
		std::cout << "  ";
		for (int j = 0; j < col; ++j)
		{
			std::cout << " " << (char)(j + 'A');
		}
		std::cout << std::endl;
	}
	void outputStone()
	{
		std::cout << " ";
		for (int j = 0; j < col; ++j)
		{
			std::cout << " " << (char)(j + 'A');
		}
		std::cout << endl;
		for (int i = 0; i < row; ++i)
		{
			if ((row - i) < 10)
				std::cout << " " << (row - i);
			else std::cout << (row - i);
			for (int j = 0; j < col; ++j)
			{
				if (board[i][j] == Stone::EMPTY)
				{
					//std::cout << " .";
					if (i == 0 && j == 0)
						std::cout << "┏";
					else if (i == row - 1 && j == 0)
						std::cout << "┗";
					else if (i == 0 && j == col - 1)
						std::cout << "┓";
					else if (i == row - 1 && j == col - 1)
						std::cout << "┛";
					else if (i == 0)
						std::cout << "┯";
					else if (i == row - 1)
						std::cout << "┷";
					else if (j == 0)
						std::cout << "┠";
					else if (j == col - 1)
						std::cout << "┨";
					else std::cout << "┼";
				}
				else if (board[i][j] == Stone::BLACK)
				{
					//std::cout << "●";
					std::cout << "○";
				}
				//std::cout << "○";
				else std::cout << "●";
			}
			if ((row - i) < 10)
				std::cout << " " << (row - i);
			else std::cout << (row - i);
			std::cout << std::endl;
		}
		std::cout << " ";
		for (int j = 0; j < col; ++j)
		{
			std::cout << " " << (char)(j + 'A');
		}
		std::cout << endl;
	}
};
#endif