#pragma once
#ifndef GHT_H
#define GHT_H
#define maxn 100
#include <string>
#include <vector>
using namespace std;

struct EdgeNode { // 边结点
	int to; // 无向边的另一个邻接点的序号
	int capacity; // 边的权重
	int flow; // 边的流量
	EdgeNode* next; // 指向下一个边结点的指针
};

struct VertexNode { // 顶点
	int data; // 顶点信息
	EdgeNode* head; // 表头指针
};

struct Graph { // 无向图的邻接表存储结构
	VertexNode vertexs[maxn]; // 顶点数组
	int vexNum, edgeNum; // 顶点数和边数
};

class GHT {
public:
	const int INF = 0x3f3f3f;
	const int WHITE = 0;
	const int GRAY = 1;
	const int BLACK = 2;

	int augmentingPathArray[maxn];  // 列表存储增广路
	int road[maxn];
	int q[maxn + 2];
	int color[maxn]; // bfs标志
	int temp1[maxn], temp2[maxn], node[maxn];

	Graph g;
	Graph tree;
	void createG(); // 控制台手动输入创建图
	void loadData(const string& filePath); // 从邻接表形式存储的文件中读取图
	void loadData_addWeight(const string& filePath); // 从带权重的文件中读取图
	vector<string> split(const string& str, const string& delim); // 分割字符串
	void trim(string& s); // 去除字符串首尾空格
	int min(int x, int y); // 返回x，y中较小值
	int bfs(int start, int target); // 广度优先遍历
	int bfs_2(int start, int target); // 广度优先遍历
	int max_flow(int source, int sink); // Ford-Fulkerson算法计算最大流
	int isNear(int i, int j); // 判断结点i,j在tree中是否相邻
	void addEdge(int i, int j); // 在tree中连接i，j，权重设置为1
	void delectEdge(int i, int j); // 在tree中删除边i，j（假设i，j相邻，即边i，j存在）
	void assign(int i, int j, int value); // 给i，j结点的capacity赋值value
	void assign_2(int i, int j, int value); // 给i，j结点的flow赋值value
	void build(int l, int r);
	void build_GH_tree(); 
	void build_GH_tree_EQ(); // EQ法生成GH树
	void build_GH_tree_MGH(); // MGH法生成GH树
	int mincut(int s, int t); // 根据最小割树计算s，t的最小割
	void showInfo(); // 控制台输出tree
	void outTree(const string& filePath); // 将生成的最小割树输出到文件中
	void outCutGragh(const string& filePath); // 将任意两点的割存储到矩阵中输出
};
#endif // !GHT_H

#pragma once