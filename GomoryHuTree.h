#pragma once
#ifndef GHT_H
#define GHT_H
#define maxn 100
#include <string>
#include <vector>
using namespace std;

struct EdgeNode { // �߽��
	int to; // ����ߵ���һ���ڽӵ�����
	int capacity; // �ߵ�Ȩ��
	int flow; // �ߵ�����
	EdgeNode* next; // ָ����һ���߽���ָ��
};

struct VertexNode { // ����
	int data; // ������Ϣ
	EdgeNode* head; // ��ͷָ��
};

struct Graph { // ����ͼ���ڽӱ�洢�ṹ
	VertexNode vertexs[maxn]; // ��������
	int vexNum, edgeNum; // �������ͱ���
};

class GHT {
public:
	const int INF = 0x3f3f3f;
	const int WHITE = 0;
	const int GRAY = 1;
	const int BLACK = 2;

	int augmentingPathArray[maxn];  // �б�洢����·
	int road[maxn];
	int q[maxn + 2];
	int color[maxn]; // bfs��־
	int temp1[maxn], temp2[maxn], node[maxn];

	Graph g;
	Graph tree;
	void createG(); // ����̨�ֶ����봴��ͼ
	void loadData(const string& filePath); // ���ڽӱ���ʽ�洢���ļ��ж�ȡͼ
	void loadData_addWeight(const string& filePath); // �Ӵ�Ȩ�ص��ļ��ж�ȡͼ
	vector<string> split(const string& str, const string& delim); // �ָ��ַ���
	void trim(string& s); // ȥ���ַ�����β�ո�
	int min(int x, int y); // ����x��y�н�Сֵ
	int bfs(int start, int target); // ������ȱ���
	int bfs_2(int start, int target); // ������ȱ���
	int max_flow(int source, int sink); // Ford-Fulkerson�㷨���������
	int isNear(int i, int j); // �жϽ��i,j��tree���Ƿ�����
	void addEdge(int i, int j); // ��tree������i��j��Ȩ������Ϊ1
	void delectEdge(int i, int j); // ��tree��ɾ����i��j������i��j���ڣ�����i��j���ڣ�
	void assign(int i, int j, int value); // ��i��j����capacity��ֵvalue
	void assign_2(int i, int j, int value); // ��i��j����flow��ֵvalue
	void build(int l, int r);
	void build_GH_tree(); 
	void build_GH_tree_EQ(); // EQ������GH��
	void build_GH_tree_MGH(); // MGH������GH��
	int mincut(int s, int t); // ������С��������s��t����С��
	void showInfo(); // ����̨���tree
	void outTree(const string& filePath); // �����ɵ���С����������ļ���
	void outCutGragh(const string& filePath); // ����������ĸ�洢�����������
};
#endif // !GHT_H

#pragma once