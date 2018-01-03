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
template<size_t row, size_t col>
class Game
{
private:
	Board<row, col> board;
	int32_t winK, randomPlayRange;
	bool blackPass, whitePass;
protected:
public:
	int32_t getWink()
	{
		return winK;
	}
	Game()
	{
		winK = 5;
		randomPlayRange = 2;
		blackPass = false;
		whitePass = false;
	}
	/*
	Game(const int32_t winK_) :winK(winK_)
	{
		blackPass = false;
		whitePass = false;
	}
	*/
	bool isEmpty(size_t index)
	{
		if (index == row * col) return true;
		return board.getGirdC(index) == Stone::EMPTY;
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
		std::unique_ptr<int> dptr(new int[row*col]);
		int* d = dptr.get();

		memset(d, -1, sizeof(int)*row*col);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				if (board.getGirdC(i, j) != Stone::EMPTY)
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
	int getCriticalPoint(Color color)
	{
		const int rd[4] = { 1,0,1,1 };
		const int cd[4] = { 0,1,1,-1 };
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				for (int k = 0; k < 4; k++)
				{
					int ir = i - rd[k], ic = j - cd[k];
					if (inside(ir, ic) && board.getGirdC(ir, ic) == color) continue;
					int r = i, c = j, cnt = 0;
					while (cnt < winK && inside(r, c) && board.getGirdC(r, c) == color)
						r += rd[k], c += cd[k], cnt++;
					if (cnt == winK - 1)
					{
						if (canPlay(r, c))
							return r*col + c;
						else if (canPlay(ir, ic))
							return ir*col + ic;
						continue;
					}
				}
		return -1;
	}
	void fastPlay(Color color)
	{
		int index;
		Color opposite = color == Color::BLACK ? Color::WHITE : Color::BLACK;
		int c = getCriticalPoint(color), oc = getCriticalPoint(opposite);
		// Fast decisions
		if (c != -1)
			index = c;
		else if (oc != -1)
			index = oc;
		else
		{
			// Random play
			std::vector<int32_t> pool = getNearPositions(randomPlayRange);
			index = pool[rand() % pool.size()];
		}
		play(index, color);
	}
	bool gameOver()
	{
		if (blackPass&&whitePass)return true;
		if (getResult() != Color::EMPTY) return true;
		for (size_t index = 0; index < row*col; ++index)
			if (board.getGirdC(index) == Stone::EMPTY)
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
					if (inside(ir, ic) && board.getGirdC(ir, ic) == color) continue;
					int r = i, c = j, cnt = 0;
					while (cnt < winK && inside(r, c) && board.getGirdC(r, c) == color)
						r += rd[k], c += cd[k], cnt++;
					if (cnt == winK) return true;
				}
		return false;
	}
	//获取胜利的一方(0表示没有人胜利)
	Color getResult()
	{
		if (isWin(Color::BLACK))
			return Color::BLACK;
		if (isWin(Color::WHITE))
			return Color::WHITE;
		return Color::EMPTY;
	}
	//落子
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