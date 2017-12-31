#ifndef HASH_HPP
#define HASH_HPP
#include<string>
#include"Board.hpp"
uint64_t randu64()
{
	return ((uint64_t)rand() << 49) | ((uint64_t)rand() << 34) | ((uint64_t)rand() << 19) | ((uint64_t)rand() << 4) | ((uint64_t)rand()&15);
}
uint64_t hashTable[19][19][3];
template<size_t row,size_t col>
class ZobristHash
{
public:
	static void reInit()
	{
		for(int k=0;k<3;++k)
			for(int i=0;i<row;++i)
				for (int j = 0; j < col; ++j)
				{
					hashTable[i][j][k]= randu64();
				}
	}
	static uint64_t getHash(Board<row,col>board)
	{
		uint64_t result = 0;
		for (int i = 0; i<row; ++i)
			for (int j = 0; j < col; ++j)
			{
				result ^= hashTable[i][j][(int32_t)board[i][j] + 1];
			}
		return result;
	}
};
#endif