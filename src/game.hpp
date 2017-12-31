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
	int32_t winK;
	bool blackPass, whitePass;
protected:
public:
	Game()
	{
		winK = 5;
		blackPass = false;
		whitePass = false;
	}
	Game(const int32_t winK_) :winK(winK_)
	{
		blackPass = false;
		whitePass = false;
	}
	bool canPlay(size_t index)
	{
		if (index == row * col)return true;
		return board.getGirdC(index) == Stone::EMPTY;
	}
	bool inside(int r, int c)
	{
		return r >= 0 && r < row && c >= 0 && c < col;
	}
	bool canPlay(int r, int c)
	{
		if (!inside(r, c)) return false;
		return canPlay(r*col + c);
	}
	std::vector<int> getNearPositions(int dist)
	{
		std::queue<pair<int, int> > q;
		std::unique_ptr<int> dptr(new int[row*col]);
		int* d = dptr.get();

		memset(d, -1, sizeof(int)*row*col);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				if (board.getGirdC(i, j) != Stone::EMPTY)
				{
					q.push(make_pair(i, j));
					d[i*col + j] = 0;
				}

		std::vector<int> res;

		while (!q.empty()) {
			pair<int, int> p = q.front(); q.pop();
			int dd = d[p.first*col + p.second];
			if (dd >= dist) break;

			if (dd > 0) res.push_back(p.first*col + p.second);

			const int rd[8] = { 1,0,-1,0,1,1,-1,-1 }, cd[8] = { 0,1,0,-1,1,-1,1,-1 };
			for (int k = 0; k < 8; k++) {
				int rr = p.first + rd[k], cc = p.second + cd[k];
				if (inside(rr, cc) && d[rr*col + cc] == -1) {
					q.push(make_pair(rr, cc));
					d[rr*col + cc] = dd + 1;
				}
			}
		}

		// Center position as beginning
		if (res.empty()) res.push_back(row / 2 * col + col / 2);

		return res;
	}
	bool explicitPlay(Color color)
	{
		Color opposite = color == Stone::BLACK ? Stone::WHITE : Stone::BLACK;
		const int rd[4] = { 1,0,1,1 };
		const int cd[4] = { 0,1,1,-1 };
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				for (int k = 0; k < 4; k++)
				{
					int ir = i - rd[k], ic = j - cd[k];
					if (inside(ir, ic) && board.getGirdC(ir, ic) == opposite) continue;
					int r = i, c = j, cnt = 0;
					while (cnt < winK && inside(r, c) && board.getGirdC(r, c) == opposite)
						r += rd[k], c += cd[k], cnt++;
					if (cnt == winK - 1)
					{
						if (canPlay(r, c))
						{
							play(r*col + c, color);
							return true;
						}
						else if (canPlay(ir, ic))
						{
							play(ir*col + ic, color);
							return true;
						}
						continue;
					}
					if (cnt == winK - 2 && canPlay(r, c) && canPlay(ir, ic))
					{
						if (rand() % 2 == 1)
						{
							play(r*col + c, color);
							return true;
						}
						else
						{
							play(ir*col + ic, color);
							return true;
						}
					}
				}
		return false;
	}
	void fastPlay(Color color)
	{
		if (explicitPlay(color))
			return;
		std::vector<int> pool = getNearPositions(2);
		play(pool[rand() % pool.size()], color);
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
		if (canPlay(index))
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
					std::cout << "  ";
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
};
#endif