#ifndef AI_HPP
#define AI_HPP
#include"game.hpp"
#include<vector>
#include<string>
#include<math.h>
#include<Windows.h>
const double selectConst = 0.3;
/*
 double timecount;
 double avgsteps=0;
 */

template<size_t row,size_t col>
class TreeNode
{
private:
	bool isLeaf()
	{
		return children.empty();
	}
	void expand(Game<row, col>& board)
	{	
		// 扩展出所有子节点
		std::vector<int> indices;
		// Critical points test
		Color opposite = color == Color::BLACK ? Color::WHITE : Color::BLACK;
		std::pair<int, std::vector<int>> c = board.getCriticalPoints(color, true), co = board.getCriticalPoints(opposite, false);
		if (c.first == 4 && c.second.size() == 1)
			indices = c.second;
		else if (co.first == 4 && co.second.size() == 1)
			indices = co.second;
		else if (c.first == 3)
			indices = c.second;
		else if (co.first == 3)
			indices = co.second;
		else
			indices = board.getNearPositions(board.getWink() / 2);
		// indices.push_back(row * col + 1);
		for (auto i: indices)
		{
			children.push_back(std::make_pair(nullptr, i));
			children.rbegin()->first = new TreeNode();
			children.rbegin()->first->color = (Color)-(int32_t)color;
		}
	}
	std::pair<TreeNode*, int32_t> select()
	{
		double maxScore = -99999999.0;
		std::pair<TreeNode*, int32_t> choice = std::make_pair(nullptr, -1);
		for (auto&& child: children)
		{
			// 存在还未扩展的子节点，则扩展它
			if (child.first->total == 0)
				return child;
			double nowScore = (child.first->score / child.first->total) + selectConst * sqrt(log(this->total) / child.first->total) + (rand() % 10)*0.0001;
			if (nowScore > maxScore)
			{
				maxScore = nowScore;
				choice = child;
			}
		}
		return choice;
	}
	void updateStats(double value)
	{
		score += value;
		total += 1;
	}
	int32_t rollOut(const Game<row, col>& board)
	{
		Game<row,col>temp = board;
		int32_t curColor = (int32_t)color;
		for (int i = 0; i < row*col / 4; i++)
		{
			temp.fastPlay((Color)curColor);
			//temp.output();
			if (temp.gameOver()) break;
			curColor = -curColor;
		}
		if (temp.gameOver()) return (int32_t)temp.getResult();
		return 0;
	}
public:
	void deleteChildren()
	{
		for (auto&& child : children)
		{
			child.first->deleteChildren();
			delete child.first;
		}
	}
	std::vector<std::pair<TreeNode*, int32_t>> children;
	double score, total;
	Color color;
	TreeNode()
	{
		score = total = 0.0;
		color = Color::EMPTY;
	}
	void selectAction(Game<row, col>& board)
	{
		std::vector<TreeNode*> visited;
		TreeNode* cur = this;
		visited.push_back(this);
		double value;
		while (!cur->isLeaf())
		{
			std::pair<TreeNode*, int32_t> next = cur->select();
			board.play(next.second, cur->color);
			cur = next.first;
			visited.push_back(cur);
		}
		if (!board.gameOver())
		{
			cur->expand(board);	
			std::pair<TreeNode*, int32_t> next = cur->select();
			board.play(next.second, cur->color);
			TreeNode* newNode = next.first;
			visited.push_back(newNode);
			int32_t result = newNode->rollOut(board);
			value = (double)result*0.5+0.5;
		}else value = ((double)board.getResult()*0.5+0.5);
		for (TreeNode* node : visited)
			// 若此节点下一步该黑棋走，那么白棋选择此节点的得分该为黑棋获胜状况取反。
			node->updateStats((node->color==Color::BLACK)?1.0-value:value);
				
	}
};
template<size_t row, size_t col>
bool cmp(const std::pair<TreeNode<row, col>*, int32_t>& u, const std::pair<TreeNode<row, col>*, int32_t>& v)
{
	return u.first->total > v.first->total;
}
template<size_t row, size_t col>
class MonteCarloTreeSearchAI
{
private:
	TreeNode<row,col>* root;
	Game<row, col> myBoard;
public:
	MonteCarloTreeSearchAI() {}
	size_t genMove(const Game<row, col>& board, Color color,int32_t playout)
	{
		LARGE_INTEGER cpuFreq;
		LARGE_INTEGER startTime;
		LARGE_INTEGER endTime;
		root = new TreeNode<row, col>();
		root->color = color;
		double runTime = 0;
		QueryPerformanceFrequency(&cpuFreq);
		QueryPerformanceCounter(&startTime);
		for(size_t t=1;t<=playout;++t)
		{
			myBoard = board;
			root->selectAction(myBoard);
		}	
		QueryPerformanceCounter(&endTime);
		runTime = (((endTime.QuadPart - startTime.QuadPart)*1.0) / cpuFreq.QuadPart);
		std::cout << "speed: " << ((double)playout / (double)runTime) << "n/s" << std::endl;
		int32_t maxTimes = -1;
		size_t choice = 0;
		double winRate=0;
		for (auto&& child : root->children)
		{
			if (child.first->total > maxTimes)
			{
				maxTimes = child.first->total;
				choice = child.second;
				winRate = (child.first->score)/(child.first->total);
			}
		}
		if (winRate < 0.1)return row*col + 1; // 认输
		sort(root->children.begin(), root->children.end(), cmp<row, col>);
		std::cout << "result: " << std::endl;
		for (int32_t i = 0; i<=5&&i < root->children.size(); ++i)
		{
			auto c = root->children[i].first;
			auto d = root->children[i].second;
			std::cout  << (char)('A' + (d % col)) << (row - d / row) << " " << c->total << " " << (c->score) / (c->total) * 100 << "%" << std::endl;
		}
		root->deleteChildren();
		delete root;
		return choice;
	}
};
#endif
