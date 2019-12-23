#include <iostream>
#include "MultiTree.h"
#include <cstdlib>
#include <ctime>

using namespace std;

clock_t start;
clock_t ending;
enum direction { NONE, UP, DOWN, LEFT, RIGHT };

//void test()
//{
//	MultiTree<int> myTree = MultiTree<int>();
//	TreeNode<int>* node[4];
//
//	node[0] = myTree.insertAsRoot(1);
//	node[1] = myTree.insertAsYC(node[0], 2);
//	node[2] = myTree.insertAsYC(node[0], 3);
//	node[3] = myTree.insertAsYC(node[1], 4);
//
//	myTree.travPre([](int & n) { cout << n << " "; });
//}

//输入区
const int ROW = 4;  //MAXROW
const int COL = 7;  //MAXCOL
const int SROW = 0;  //Started Row
const int SCOL = 6;  //Started Column
const int TROW = 1;  //Teminateded Row
const int TCOL = 5;  //Teminateded Column
const int INIT[ROW][COL] = { {-1, -0, -0, -0, -1, -1, +1},
							 {-0, -1, -1, -1, -0, -0, -1},
							 {-1, -0, -1, -0, -0, -0, -1},
							 {-0, -0, -0, -0, -1, -0, -0} };  //起始点置1，终点置0，障碍节点置-1，途径节点置0

//二维数据点
class point
{
public:
	int x;
	int y;
	direction dir;

	point(int a = 0, int b = 0, direction d = NONE) :x(a), y(b), dir(d) {};
};


//位移算子
point* up(TreeNode<point>* cur, int map[ROW][COL], bool isLast)
{
	int xx = cur->data.x;
	while (xx > -1 && map[xx][cur->data.y] != 0) //若xx不合法，则--
	{
		xx--;
	}
	if (xx == -1 || (!isLast && (xx == TROW && cur->data.y == TCOL)))
	{
		return NULL;
	}
	else
	{
		return new point(xx, cur->data.y, UP);
	}
}

point* down(TreeNode<point>* cur, int map[ROW][COL], bool isLast)
{
	int xx = cur->data.x;
	while (xx < ROW && map[xx][cur->data.y] != 0) //若xx不合法，则++
	{
		xx++;
	}
	if (xx == ROW || (!isLast && (xx == TROW && cur->data.y == TCOL)))
	{
		return NULL;
	}
	else
	{
		return new point(xx, cur->data.y, DOWN);
	}
}

point* left(TreeNode<point>* cur, int map[ROW][COL], bool isLast)
{
	int yy = cur->data.y;
	while (yy > -1 && map[cur->data.x][yy] != 0) //若yy不合法，则--
	{
		yy--;
	}
	if (yy == -1 || !isLast && (cur->data.x == TROW && yy == TCOL))
	{
		return NULL;
	}
	else
	{
		return new point(cur->data.x, yy, LEFT);
	}
}

point* right(TreeNode<point>* cur, int map[ROW][COL], bool isLast)
{
	int yy = cur->data.y;
	while (yy < COL && map[cur->data.x][yy] != 0) //若yy不合法，则++
	{
		yy++;
	}
	if (yy == COL || (!isLast && (cur->data.x == TROW && yy == TCOL)))
	{
		return NULL;
	}
	else
	{
		return new point(cur->data.x, yy, RIGHT);
	}
}

//将 map 的状态演化到搜索树的节点 cur 处
int evolution(TreeNode<point>* cur, int map[ROW][COL])
{
	int height = 0;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] = INIT[i][j];
		}
	}
	TreeNode<point>* temp = cur;
	while (temp)
	{
		height++;
		map[temp->data.x][temp->data.y] = 1;
		temp = temp->parent();
	}
	return height;
}

//主程序入口
int main()
{
	start = clock();  //计时开始
	int map[ROW][COL];  //记录状态
	MultiTree<point> searchTree = MultiTree<point>();  //定义搜索树
	Stack<TreeNode<point>*> S;  //定义辅助栈
	TreeNode<point>* visit = NULL;  //当前激活的节点
	int depth = 0;  //当前激活节点所处的深度
	bool find_last = false;

	int LENGTH = 1;  //定义并计算目标搜索深度
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (INIT[i][j] == 0)
			{
				LENGTH++;
			}
		}
	}

	S.push(searchTree.insertAsRoot(point(SROW, SCOL, NONE)));  //根节点入栈
	while (!S.empty() && evolution(visit, map) < LENGTH)  //若未抵达目标深度且栈未空之前，则反复循环
	{
		visit = S.pop(); //弹出当前节点
		if (evolution(visit, map) == LENGTH - 1) // 尝试前往目标深度
		{
			find_last = true;
		}
		point* pos;  //非空孩子的入栈次序为先右后左
		if ((visit->data.dir != DOWN) && (pos = up(visit, map, find_last)))  //不走回头路，且该位移是可行的
		{
			S.push(searchTree.insertAsYC(visit, *pos));  //向下扩展搜索树，压栈
		}
		if ((visit->data.dir != UP) && (pos = down(visit, map, find_last)))
		{
			S.push(searchTree.insertAsYC(visit, *pos));
		}
		if ((visit->data.dir != RIGHT) && (pos = left(visit, map, find_last)))
		{
			S.push(searchTree.insertAsYC(visit, *pos));
		}
		if ((visit->data.dir != LEFT) && (pos = right(visit, map, find_last)))
		{
			S.push(searchTree.insertAsYC(visit, *pos));
		}
		find_last = false; //前往目标深度失败
	}

	ending = clock();  //计时结束
	double endtime = (double)(ending - start) / CLOCKS_PER_SEC;
	std::cout << "Total time:" << endtime * 1000 << "ms" << endl;	//输出总耗时，ms为单位


	if (S.empty())
	{
		std::cout << "Not Found!";
	}
	else
	{
		while (visit)  //反向输出路径
		{
			std::cout << "(" << visit->data.x + 1 << "," << visit->data.y + 1 << ")\n";
			visit = visit->parent();
		}
	}

	system("pause");
	return 0;
}
