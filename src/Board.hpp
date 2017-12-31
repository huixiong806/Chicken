#ifndef BOARD_HPP
#define BOARD_HPP
#include<string>
enum class Stone:int32_t
{
	BLACK=1,
	EMPTY=0,
	WHITE=-1
};
using Color = Stone;
template<size_t row,size_t col>
class Board
{
private:
	Stone gird[row][col];
public:
	Stone* operator[](size_t index)
	{
		return gird[index];
	}
	const Stone& getGirdC(size_t x, size_t y)const
	{
		return gird[x][y];
	}
	const Stone& getGirdC(size_t index)const
	{
		return gird[index/col][index%col];
	}
	Board()
	{
		for (int i = 0; i < row; ++i)
			for (int j = 0; j < col; ++j)
				gird[i][j] = Stone::EMPTY;
	}
	bool operator<(const Board& rhs)const
	{
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < col; ++j)
				if (gird[i][j] < rhs.getGirdC(i, j))
					return true;
		}
		return false;
	}
};
#endif