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
enum class PutStoneResult:bool
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
	Game(const int32_t winK_):winK(winK_)
	{
		blackPass = false;
		whitePass = false;
	}
	bool canPlay(size_t index)
	{
		if (index == row*col)return true;
		return board.getGirdC(index) == Stone::EMPTY;
	}
	void randomPlay(Color color)
	{
		std::vector<int32_t> pool;
		for (int i = 0; i <= row*col; ++i)
			if (board.getGirdC(i)==Color::EMPTY)
				pool.push_back(i);
		play(pool[rand()%pool.size()], color);
	}
	bool gameOver()
	{
		if (blackPass&&whitePass)return true;
		if (getResult() != Color::EMPTY)return true;
		for (size_t index = 0; index < row*col; ++index)
			if (board.getGirdC(index) == Stone::EMPTY)
				return false;
		return true;
	}
	//获取胜利的一方(0表示没有人胜利)
	Color getResult()
	{
		//判断横向k子连
		for (size_t i = 0; i < row; ++i)
		{
			size_t blackCount=0,whiteCount=0;
			for (size_t j = 0; j < col; ++j)
			{
				
				if (board.getGirdC(i,j) == Stone::WHITE)whiteCount++;
				else whiteCount = 0;
				if (board.getGirdC(i, j) == Stone::BLACK)blackCount++;
				else blackCount = 0;
				if (blackCount >= winK || whiteCount >= winK)
					return blackCount >= winK ? Color::BLACK : Color::WHITE;
			}
		}
		//判断纵向k子连
		for (size_t i = 0; i < col; ++i)
		{
			size_t blackCount = 0, whiteCount = 0;
			for (size_t j = 0; j < row; ++j)
			{
				if (board.getGirdC(j, i) == Stone::WHITE)whiteCount++;
				else whiteCount = 0;
				if (board.getGirdC(j, i) == Stone::BLACK)blackCount++;
				else blackCount = 0;
				if (blackCount >= winK || whiteCount >= winK)
					return blackCount >= winK ? Color::BLACK : Color::WHITE;
			}
		}
		//判断斜向K子连(左下-右上)
		for (size_t sum = winK-1; sum < row+col- winK; ++sum)
		{
			size_t blackCount = 0, whiteCount = 0;
			for (size_t r = std::max(0,(int32_t)sum-(int32_t)col+1); r < row&&sum-r>=0; ++r)
			{
				int32_t c = sum - r;
				if (board.getGirdC(r, c) == Stone::WHITE)whiteCount++;
				else whiteCount = 0;
				if (board.getGirdC(r, c) == Stone::BLACK)blackCount++;
				else blackCount = 0;
				if (blackCount >= winK || whiteCount >= winK)
					return blackCount >= winK ? Color::BLACK : Color::WHITE;
			}
		}
		//判断斜向K子连(右下-左上)
		for (size_t sum = winK-1; sum < row + col - winK; ++sum)
		{
			size_t blackCount = 0, whiteCount = 0;
			for (size_t r = std::max(0, (int32_t)sum - (int32_t)col + 1); r < row&&sum - r >= 0; ++r)
			{
				int32_t c = sum - r;
				if (board.getGirdC(r, col - c - 1) == Stone::WHITE)whiteCount++;
				else whiteCount = 0;
				if (board.getGirdC(r, col - c - 1) == Stone::BLACK)blackCount++;
				else blackCount = 0;
				if (blackCount >= winK || whiteCount >= winK)
					return blackCount >= winK ? Color::BLACK : Color::WHITE;
			}
		}
		return Color::EMPTY;
	}
	//落子
	PutStoneResult play(size_t index, Color color)
	{
		if (index == row*col)
		{
			if (color == Color::WHITE)whitePass = true;
			else if (color == Color::BLACK)blackPass = true;
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
	Board<row,col>& getBoard()
	{
		return board;
	}
	void output()
	{
		std::cout << " ";
		for (int j = 0; j < col; ++j)
		{
			std::cout<<" " << (char)(j+'A');
		}
		std::cout << endl;
		for (int i = 0; i < row; ++i)
		{
			if ((row-i) < 10)
				std::cout <<" "<< (row - i);
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