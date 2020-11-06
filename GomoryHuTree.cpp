#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include "fin.h"


void GHT::createG() { // 采用邻接表存储表示，构造有向图G
	int i = 0; // 循环变量
	EdgeNode* p; // 用来构造边链表的边结点指针
	int v1, v2, w; // 无向边的两个顶点
	g.vexNum = g.edgeNum = 0;
	scanf_s("%d%d", &g.vexNum, &g.edgeNum); // 首先输入顶点个数和边数

	for (i = 0; i < g.vexNum; i++) { // 初始化表头指针为空0
		g.vertexs[i].head = NULL;
	}

	for (i = 0; i < g.edgeNum; i++) {
		scanf_s("%d%d%d", &v1, &v2, &w); // 输入一条边的起点和终点
		v1--; v2--;
		p = new EdgeNode; // 假定有足够空间
		p->to = v2;
		p->capacity = w;
		p->flow = 0;
		p->next = g.vertexs[v1].head; // 插入链表(头部)
		g.vertexs[v1].head = p;
	}
}

void GHT::trim(string& s) {
	if (s.empty()) {
		return;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
}

vector<string> GHT::split(const string& s, const string& delim) {
	vector<string> tokens;;
	tokens.clear();
	size_t lastPos = s.find_first_not_of(delim, 0);
	size_t pos = s.find(delim, lastPos);
	while (lastPos != string::npos) {
		tokens.emplace_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delim, pos);
		pos = s.find(delim, lastPos);
	}
	return tokens;
}

void GHT::loadData(const string& filePath) {
	EdgeNode* p;
	ifstream dataFile;
	dataFile.open(filePath);
	if (dataFile.is_open()) {
		string line;
		string n_s = "";
		int n = 0;
		while (getline(dataFile, line)) {
			vector<string> nodes;
			trim(line);
			if (line.at(0) == '#') {
				int i = 0;
				while (!(line[i] >= '0' && line[i] <= '9')) {
					i++;
				}
				while (line[i] >= '0' && line[i] <= '9') {
					n_s.push_back(line[i]);
					i++;
				}
				stringstream ss0;
				ss0 << n_s;
				ss0 >> n; //string -> int
				g.vexNum = n; tree.vexNum = n;
				for (int j = 0; j < n; j++) { // 初始化表头指针为空0
					g.vertexs[j].head = NULL;
				}
				continue;
			}
			nodes = split(line, "	");

			if (nodes.size() == 2) {
				int neighbor = 0;
				int currentVertex = 0;
				stringstream ss;
				ss << nodes[0];
				ss >> currentVertex; //string -> int
				vector<string> vertices;
				vertices = split(nodes[1], " ");

				for (unsigned int i = 0; i < vertices.size(); i++) {
					stringstream ss1;
					ss1 << vertices[i];
					ss1 >> neighbor; //string -> int

					p = new EdgeNode; // 假定有足够空间
					p->to = neighbor - 1;
					p->capacity = 1;
					p->flow = 0;
					p->next = g.vertexs[currentVertex - 1].head; // 插入链表(头部)
					g.vertexs[currentVertex - 1].head = p;
				}
			}
			else {
				cout << "Exception of data format: " << line << endl;
				break;
			}
		}
		dataFile.close();
	}
	else {
		cout << "Open File Error!" << endl;
	}
}

void GHT::loadData_addWeight(const string& filePath) { // 无向图的边只需要输入一次
	EdgeNode* p1,*p2;
	ifstream dataFile;
	dataFile.open(filePath);
	if (dataFile.is_open()) {
		string line;
		string n_s = "";
		int n = 0;
		while (getline(dataFile, line)) {
			vector<string> nodes;
			trim(line);
			if (line.at(0) == '#') {
				int i = 0;
				while (!(line[i] >= '0' && line[i] <= '9')) {
					i++;
				}
				while (line[i] >= '0' && line[i] <= '9') {
					n_s.push_back(line[i]);
					i++;
				}
				stringstream ss0;
				ss0 << n_s;
				ss0 >> n; //string -> int
				g.vexNum = n; tree.vexNum = n;
				for (int j = 0; j < n; j++) { // 初始化表头指针为空0
					g.vertexs[j].head = NULL;
				}
				continue;
			}

			nodes = split(line, " ");

			int v1 = 0, v2 = 0, w = 0;
			stringstream ss1,ss2,ss;
			ss1 << nodes[0];
			ss1 >> v1; //string -> int
			ss2 << nodes[1];
			ss2 >> v2; //string -> int
			ss << nodes[2];
			ss >> w; //string -> int

			p1 = new EdgeNode; // 假定有足够空间
			p1->to = v2 - 1;
			p1->capacity = w;
			p1->flow = 0;
			p1->next = g.vertexs[v1 - 1].head; // 插入链表(头部)
			g.vertexs[v1 - 1].head = p1;

			p2 = new EdgeNode; // 假定有足够空间
			p2->to = v1 - 1;
			p2->capacity = w;
			p2->flow = 0;
			p2->next = g.vertexs[v2 - 1].head; // 插入链表(头部)
			g.vertexs[v2 - 1].head = p2;
		}
		dataFile.close();
	}
	else {
		cout << "Open File Error!" << endl;
	}
}

int GHT::min(int x, int y) { // 返回x，y中较小的值
	return x < y ? x : y;
}

int GHT::bfs(int start, int target) { // start-target广度优先遍历
	int u, v;

	for (int i = 0; i < g.vexNum; i++) {
		color[i] = WHITE;
	}
	queue<int> q;
	q.push(start);
	color[start] = GRAY;
	augmentingPathArray[start] = -1;
	while (!q.empty())
	{
		u = q.front();
		color[u] = BLACK;
		q.pop();
		EdgeNode* p = g.vertexs[u].head;

		while (p != NULL) {
			v = p->to;
			if (color[v] == WHITE && ((p->capacity) - (p->flow)) > 0) {
				q.push(v);
				color[v] = GRAY;
				augmentingPathArray[v] = u;
			}
			p = p->next;
		}
	}

	// 若target顶点color为BLACK
	return color[target] == BLACK;
}

int GHT::max_flow(int source, int sink) { // Ford-Fulkerson算法计算最大流
	int u;
	int max_flow = 0;

	EdgeNode* p;
	for (int i = 0; i < g.vexNum; i++) {
		p = g.vertexs[i].head;
		while (p != NULL) {
			p->flow = 0;
			p = p->next;
		}
	}

	// 若增广路存在，更新改进量increment
	while (bfs(source, sink)) {
		// 计算increment
		int increment = INF;
		for (u = sink; augmentingPathArray[u] != (-1); u = augmentingPathArray[u]) {
			p = g.vertexs[augmentingPathArray[u]].head;
			while (p->to != u) {
				p = p->next;
			}
			increment = min(increment, p->capacity - p->flow);
		}

		// 更新流量网络
		for (u = sink; augmentingPathArray[u] != (-1); u = augmentingPathArray[u]) {
			p = g.vertexs[augmentingPathArray[u]].head;
			while (p->to != u) {
				p = p->next;
			}
			p->flow += increment;
		}
		for (u = sink; augmentingPathArray[u] != (-1); u = augmentingPathArray[u]) {
			p = g.vertexs[u].head;
			while (p->to != augmentingPathArray[u]) {
				p = p->next;
			}
			p->flow -= increment;
		}

		max_flow += increment;
	}

	return max_flow;
}

int GHT::isNear(int i, int j) { // 判断在tree中i，j是否相邻
	EdgeNode* p = tree.vertexs[i].head;
	while (p != NULL) {
		if (p->to == j) {
			return true;
		}
		p = p->next;
	}
	return false;
}

void GHT::addEdge(int i, int j) { // 在tree中连接i，j，权重设置为1
	if (!isNear(i, j)) {
		EdgeNode* p = new EdgeNode; // 假定有足够空间
		p->to = j;
		p->capacity = 1;
		p->flow = 0;
		p->next = tree.vertexs[i].head; // 插入链表(头部)
		tree.vertexs[i].head = p;
	}
}

void GHT::delectEdge(int i, int j) { // 在tree中删除边i，j（假设i，j相邻，即边i，j存在）
	if (tree.vertexs[i].head->to == j) {
		tree.vertexs[i].head = tree.vertexs[i].head->next;
	}
	else {
		EdgeNode* p = tree.vertexs[i].head;
		while (p->next != NULL) {
			if (p->next->to == j) {
				p->next = p->next->next;
			}
			else {
				p = p->next;
			}
		}
	}
}

void GHT::assign(int i, int j, int value) { // 给i，j结点的capacity赋值value
	EdgeNode* p; // 假定有足够空间	
	if (isNear(i, j)) {
		p = tree.vertexs[i].head;
		while (p != NULL) {
			if (p->to == j) {
				p->capacity = value;
				break;
			}
			p = p->next;
		}
	}
	else {
		p = new EdgeNode;
		p->to = j;
		p->capacity = value;
		p->flow = 0;
		p->next = tree.vertexs[i].head; // 插入链表(头部)
		tree.vertexs[i].head = p;
	}
}

void GHT::build(int l, int r) {
	if (l >= r) return;
	int x = node[l], y = node[l + 1];
	int cut = max_flow(x, y);
	assign(x, y, cut); assign(y, x, cut);

	int top1 = 0, top2 = 0;
	for (int i = l; i <= r; i++)
	{
		//最后一次bfs后有深度,说明和x相连接,放在左集合temp1 
		if (bfs(x, node[i])) { temp1[++top1] = node[i]; }
		else { temp2[++top2] = node[i]; }
	}
	for (int i = l; i <= l + top1 - 1; i++)    node[i] = temp1[i - l + 1];
	for (int i = l + top1; i <= r; i++)  node[i] = temp2[i - top1 - l + 1];

	build(l, l + top1 - 1); build(l + top1, r);//两个集合分别建图
}

void GHT::build_GH_tree() {
	tree.vexNum = g.vexNum;
	for (int i = 0; i < tree.vexNum; i++) { // 初始化表头指针为空0
		tree.vertexs[i].head = NULL;
	}
	for (int i = 0; i < g.vexNum; i++) { node[i] = i; }
	build(0, g.vexNum - 1);
}

void GHT::build_GH_tree_EQ() { // GHT法生成GH树
	// 初始化最小割树
	tree.vexNum = g.vexNum;
	for (int i = 0; i < tree.vexNum; i++) { // 初始化表头指针为空0
		tree.vertexs[i].head = NULL;
	}

	for (int i = 1; i < tree.vexNum; i++) {
		EdgeNode* p1 = new EdgeNode; // 假定有足够空间
		p1->to = i;
		p1->capacity = 1;
		p1->flow = 0;
		p1->next = tree.vertexs[0].head; // 插入链表(头部)
		tree.vertexs[0].head = p1;

		EdgeNode* p2 = new EdgeNode;
		p2->to = 0;
		p2->capacity = 1;
		p2->flow = 0;
		p2->next = tree.vertexs[i].head; // 插入链表(头部)
		tree.vertexs[i].head = p2;
	}

	for (int s = 1; s < tree.vexNum; s++) {
		EdgeNode* p;
		p = tree.vertexs[s].head;
		int t = p->to;
		int MaxFlow = max_flow(s, t); // 计算最大流,得到最小割
		p->capacity = MaxFlow;
		assign(t, s, MaxFlow);

		for (int i = s + 1; i < tree.vexNum; i++) {
			if (isNear(t, i) && bfs(s, i)) {
				delectEdge(t, i);
				delectEdge(i, t);
				addEdge(i, s);
				addEdge(s, i);
			}
		}
	}
}

void GHT::assign_2(int i, int j, int value) { // 给i，j结点的flow赋值value
	EdgeNode* p; // 假定有足够空间

	if (isNear(i, j)) {
		p = tree.vertexs[i].head;
		while (p != NULL) {
			if (p->to == j) {
				p->flow = value;
				break;
			}
			p = p->next;
		}
	}
}

void GHT::build_GH_tree_MGH() {
	int i, s, p[maxn], t, f1[maxn], q[maxn], minCut;
	int m = 0;

	for (int i = 0; i < tree.vexNum; i++) { // 初始化表头指针为空0
		tree.vertexs[i].head = NULL;
	}

	for (i = 0; i < tree.vexNum; i++) {
		p[i] = 0;
		f1[i] = 0;
		q[i] = 0;
	}

	for (s = 1; s < tree.vexNum; s++) {
		t = p[s];
		minCut = max_flow(s, t);
		f1[s] = minCut;

		for (i = 0; i < tree.vexNum; i++) {
			q[i] = 0;
		}

		for (int i = 0; i < tree.vexNum; i++) { // 初始化表头指针为空0
			tree.vertexs[i].head = NULL;
		}

		for (i = 0; i < tree.vexNum; i++) {
			if (color[i] == BLACK) {
				q[i] = BLACK;
			}
		}

		for (i = 0; i < tree.vexNum; i++) {
			if (i != s && p[i] == t && q[i] == BLACK) {
				p[i] = s;
			}
		}

		if (q[p[t]] == BLACK) {
			p[s] = p[t];
			p[t] = s;
			f1[s] = f1[t];
			f1[t] = minCut;
		}

		assign_2(s, t, minCut);
		assign_2(t, s, minCut);

		for (int i = 0; i < s; i++)
			if (i != t) {
				EdgeNode* p3 = tree.vertexs[t].head;
				while (p3 != NULL) {
					if (p3->to == i) {
						m = min(minCut, p3->flow);
						break;
					}
					p3 = p3->next;
				}
				assign_2(s, i, m);
				assign_2(i, s, m);
			}

		for (i = 1; i <= s; i++) {
			assign(i, p[i], f1[i]);
			assign(p[i], i, f1[i]);
		}
	}
}

int GHT::bfs_2(int start, int target) { // start-target广度优先遍历
	int u, v;

	for (int i = 0; i < g.vexNum; i++) {
		color[i] = WHITE;
	}
	queue<int> q;
	q.push(start);
	color[start] = GRAY;
	road[start] = -1;
	while (!q.empty())
	{
		u = q.front();
		color[u] = BLACK;
		q.pop();
		EdgeNode* p = tree.vertexs[u].head;

		while (p != NULL) {
			v = p->to;
			if (color[v] == WHITE) {
				q.push(v);
				color[v] = GRAY;
				road[v] = u;
			}
			p = p->next;
		}
	}

	// 若target顶点color为BLACK
	return color[target] == BLACK;
}

int GHT::mincut(int s, int t) { // 根据GH树计算s，t的最小割
	int m = INF;

	while (bfs_2(s, t)) {
		for (int u = t; road[u] != (-1); u = road[u]) {
			EdgeNode* p = tree.vertexs[road[u]].head;
			while (p != NULL) {
				if (p->to == u) {
					m = min(m, p->capacity);
					break;
				}
				p = p->next;
			}
		}
		break;
	}
	return m;
}

void GHT::showInfo() {
	EdgeNode* p;
	for (int i = 0; i < g.vexNum; i++) {
		p = g.vertexs[i].head;
		cout << i+1 << "      ";
		while (p != NULL) {
			cout << p->to+1 << " " << p->capacity << "    ";
			p = p->next;
		}
		cout << endl;
	}
}

void GHT::outTree(const string& filePath) {
	EdgeNode* p;
	ofstream outputFile;
	outputFile.open(filePath);
	if (outputFile.is_open()) {
		for (int i = 0; i < tree.vexNum; i++) {
			p = tree.vertexs[i].head;
			outputFile << i+1 << "      ";
			while (p != NULL) {
				outputFile << p->to+1 << " " << p->capacity << "    ";
				p = p->next;
			}
			outputFile << endl;
		}
	}
	outputFile.close();
}

void GHT::outCutGragh(const string& filePath) {
	ofstream outputFile;
	outputFile.open(filePath);
	if (outputFile.is_open()) {
		for (int i = 0; i < tree.vexNum; i++) {
			for (int j = 0; j < tree.vexNum; j++) {
				if (i == j) {
					outputFile << 0 << "    ";
				}
				else {
					outputFile << mincut(i,j) << "    ";
				}
			}
			outputFile << endl;
		}
	}
	outputFile.close();
}


int main() {
	//int num = 12;

	//for (int i = 0; i < num; i++) {
	//	stringstream ss; string i_s;
	//	ss << i; ss >> i_s;

	//	// 输入文件
	//	string inputFile = "C:\\Users\\Administrator\\Desktop\\example\\GHT\\input" + i_s + ".txt";
	//	// string inputFile_addWeight = "C:\\Users\\Administrator\\Desktop\\example\\GHT\\input_add.txt";

	//	// 输出文件
	//	// string outputFile = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A0.txt";
	//	// string outputFile_EQ = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A1.txt";
	//	// string outputFile_MGH = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A2.txt";
	//	string outputFile_CutGraph = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B" + i_s + ".txt";
	//	// string outputFile_CutGraph_EQ = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B1.txt";
	//	// string outputFile_CutGraph_MGH = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B2.txt";

	//	GHT t, t1, t2;
	//	t.loadData(inputFile);// t1.loadData(inputFile); t2.loadData(inputFile);

	//	t.build_GH_tree();// t1.build_GH_tree_EQ(); t2.build_GH_tree_MGH();
	//	// t.outTree(outputFile); t1.outTree(outputFile_EQ); t2.outTree(outputFile_MGH);
	//	t.outCutGragh(outputFile_CutGraph);// t1.outCutGragh(outputFile_CutGraph_EQ); t2.outCutGragh(outputFile_CutGraph_MGH);
	//}

	int num = 14;
	stringstream ss; string i_s;
	ss << num; ss >> i_s;
	
	// 输入文件
	string inputFile = "C:\\Users\\Administrator\\Desktop\\example\\GHT\\input" + i_s + ".txt";
	// string inputFile_addWeight = "C:\\Users\\Administrator\\Desktop\\example\\GHT\\input_add.txt";

	// 输出文件
	// string outputFile = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A0.txt";
	// string outputFile_EQ = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A1.txt";
	// string outputFile_MGH = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\A2.txt";
	string outputFile_CutGraph = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B" + i_s + ".txt";
	// string outputFile_CutGraph_EQ = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B1.txt";
	// string outputFile_CutGraph_MGH = "C:\\Users\\Administrator\\Desktop\\result\\GHT\\B2.txt";

	GHT t, t1, t2;
	t.loadData(inputFile);// t1.loadData(inputFile); t2.loadData(inputFile);

	t.build_GH_tree();// t1.build_GH_tree_EQ(); t2.build_GH_tree_MGH();
	// t.outTree(outputFile); t1.outTree(outputFile_EQ); t2.outTree(outputFile_MGH);
	t.outCutGragh(outputFile_CutGraph);// t1.outCutGragh(outputFile_CutGraph_EQ); t2.outCutGragh(outputFile_CutGraph_MGH);
}