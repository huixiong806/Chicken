#ifndef VALUE_TRAINNER_HPP
#define VALUE_TRAINNER_HPP
#include "game.hpp"
#include "hash.hpp"
#include <fstream>
//#define DNApopulation 1000//DNA种群数100 
int32_t DNApopulation;
#define mutationRate 0.001//突变率
class DNA
{
public:
	int32_t gene[405];
	//float fitness;
	/*
	void set_fitness()
	{
		float score = 0;
		for (int i = 0; i<gene_length; i++)
		{
			if (gene[i] == target[i])
			{
				score = score + 1;
			}
		}
		fitness = score / gene_length;
	}*/
	DNA()
	{
		for (int i = 0; i<405; i++)
		{
			gene[i] = (int32_t)pow(2.0, ((double)(randu64() % 20000) / 1000.0));//随机生成价值
		}
	}
	DNA crossover(DNA partner)
	{
		DNA child = DNA();
		for (int i = 0; i<405; i++)
		{
			if (rand() % 100 + 1 >= 50)child.gene[i] = gene[i];
			else child.gene[i] = partner.gene[i];
		}
		return child;
	}
	void mutate(float mutation_rate)
	{
		for (int i = 0; i<405; i++)
		{
			if (rand() % (10000) / (float)(10000) <= mutation_rate)
				gene[i] = (int32_t)pow(2.0, ((double)(randu64() % 20000) / 1000.0));
		}
	}
	/*
	string getPhrase()
	{
		string s = "";
		for (int i = 0; i<gene_length; i++)
		{
			s += genes[i];
		}
		return s;
	}*/
};

class ValueTrainner
{
public:
	std::vector<DNA> population;
	//a胜过b
	~ValueTrainner()
	{
	}
	bool beat(const DNA& a,const DNA& b)
	{
		Game<15, 15>game;
		int32_t curColor = (int32_t)1;
		const DNA *black,*white;
		bool blackIsA=0;
		if (rand() % 2)blackIsA =1,black = &a,white=&b;
		else white = &a, black = &b;
		for (int i = 0; i < 15*15 / 4; i++)
		{
			game.fastPlay((Color)curColor,curColor==1?black->gene:white->gene);
			//temp.output();
			if (game.gameOver()) break;
			curColor = -curColor;
		}
		if (!game.gameOver()) return rand() % 2;
		int32_t result = (int32_t)game.getResult();
		if (result == 1)
			return blackIsA;
		else if (result == -1)
			return blackIsA ^ 1;
	}
	DNA getBest()
	{
		std::vector<int> v;
		DNA* res=nullptr;
		v.resize(DNApopulation);
		for(int i=0;i<DNApopulation;++i)
			for (int j = i + 1; j < DNApopulation; ++j)
			{
				if (beat(population[i], population[j]))
				{
					v[i]++;
				}
				else v[j]++;
			}
		int maxScore = -1;
		int best = -1;
		for (int i = 0; i < DNApopulation; ++i)
		{
			if (v[i] > maxScore)
			{
				maxScore = v[i];
				best = i;
			}
		}
		return population[best];
	}
	void setup()//构造DNA种群 
	{
		population.resize(DNApopulation);
	}
	void draw()
	{
		/*
		//获取个体适应度 
		for (int i = 0; i<DNApopulation; i++)
		{
			population[i].set_fitness();
		}*/
		//选择 
		std::vector<DNA> child;
		for (int i = 0; i < DNApopulation; i++)
		{
			int a = randu64() % (DNApopulation);
			int b = randu64() % (DNApopulation);
			int c = randu64() % (DNApopulation);
			int d = randu64() % (DNApopulation);
			DNA parentA = beat(population[a],population[b])? population[a]: population[b];
			DNA parentB = beat(population[c],population[d])? population[c]: population[d];
			DNA Child = parentA.crossover(parentB);
			Child.mutate(mutationRate);
			child.push_back(Child);
		}
		population = child;
	}
	void train()
	{

		std::cout << "参数训练开始" << std::endl;
		std::cout << "请输入种群数量n" << std::endl;
		std::cin >> DNApopulation;
		setup();
		while (1)
		{
			std::cout << "请输入操作，k(>=10)继续进化k代 O(k*n),1=当前最优解自对弈,并将权值输出到value.txt O(n^2)" << std::endl;
			int32_t choice;
			std::cin >> choice;
			if(choice>=10)
			{
				int k = choice;
				while (k--)
				{
					if (k % 10 == 0)std::cout << "当前进化任务剩余" << k << "代" << std::endl;
					draw();
				}
			}
			else if (choice == 9)
			{
				std::ofstream fs("value.txt");
				DNA best = getBest();
				for (int i = 0; i < 405; ++i)
					fs << best.gene[i]<<" ";
			}
			else if(choice==1)
			{
				DNA best = getBest();
				Game<15, 15>temp(best.gene);
				int32_t curColor = 1;
				while(!temp.gameOver())
				{
					temp.fastPlay((Color)curColor);
					temp.outputStone();
					curColor = -curColor;
				}
				Color result = temp.getResult();
				std::cout << (result == Color::BLACK ? "Black Win" : result == Color::WHITE ? "White Win" : "Tie") << std::endl;
				std::ofstream fs("value.txt");
				for (int i = 0; i < 405; ++i)
					fs << best.gene[i] << " ";
			}
		}
	}
};
#endif