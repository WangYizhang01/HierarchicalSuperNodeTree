#ifndef MINCUTTREE_HIERARCHICALSUPERNODETREE_H
#define MINCUTTREE_HIERARCHICALSUPERNODETREE_H

#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

struct HSNode {
    int degree = 0;
    int initDegree = 0; // ��ʼ�����ŵĴ�С�����ڼ�����
    int hierarchical = 0;
    set<int> vertexSet = {};
    HSNode* parent = NULL;
    map<HSNode*, int> children = {};

    void print() {
        int num = 0;
        cout << "[";
        for (set<int>::iterator vsit = vertexSet.begin(); vsit != vertexSet.end(); vsit++) {
            if (num > 0) {
                cout << " ";
            }
            cout << *vsit;
            num++;
        }
        cout << "](L" << hierarchical << "|" << "D" << degree << ")";
        if (children.size() > 0) {
            int i = 0;
            cout << "{";
            for (map<HSNode*, int>::iterator cit = children.begin(); cit != children.end(); cit++) {
                if (i > 0) {
                    cout << ",";
                }
                HSNode* child = (*cit).first;
                child->print();
                if ((*cit).second != 0) {
                    cout << "$" << (*cit).second << "$";
                }
                i++;
            }
            cout << "}";
        }
    }
};


class HierarchicalSuperNodeTree {
public:
    HierarchicalSuperNodeTree() {};

    ~HierarchicalSuperNodeTree() {
        //construct_graph
        //forest_set

    };//release space

    void execute(vector<vector<int> > allMaximalClique);
    void insertClique(vector<int> maximalClique, map<int, int> increDegreeMap, set<int> newVertex, set<int> oldVertex, int flag = 0);
    HSNode* batchUpdateTreeNode(vector<int> maximalClique, map<HSNode*, map<int, set<int>>> treeNodeMap, map<int, int> increDegreeMap, set<int> newVertexSet, int newVertexNodeDegree, set<int> oldVertexSet);
    void setHierarchical(HSNode* updateNode, int hierarchical);
    int getPotentialNum(HSNode* node, map<int, set<int>> updateNodeMap, set<int> newVertexSet, int flag = 0);
    int computeMincut(int m, int n); // �����ɵ����м�������������֮�����С��
    void outCutGragh(const string& filePath); // ���������������С����ɵľ���
    void update(HSNode* curNode, HSNode* parentNode);
    void connectNewVertexSet(map<int, set<int>> updateMap, HSNode* node, HSNode* newNode, set<int> newVertexSet, int newVertexNodeDegree);
    void trim(string& s); // ȥ���ַ�����β�ո�
    vector<string> split(const string& str, const string& delim); // �ָ��ַ���
    vector<vector<int>> loadData(const string& filePath); // ��ȡ������
    vector<vector<int>> splitMaximalClique(vector<int> maximalClique);
    map<HSNode*, int> searchRoad(HSNode* node1, HSNode* node2); // ����ͬһ���������������·��
    void sequenceTraversal(); // ͼ�Ĳ������
    void updatePeakNode(map<HSNode*, int> road, int descendantDegree); // ���·���
    map<int, int> graphGrow(vector<int> maximalClique, set<int>& newVertexSet, set<int>& oldVertexSet);
    void printParameters();

private:
    map<int, HSNode*> vertex_node_index;
    map<int, map<int, int>*> construct_graph;
};

#endif //MINCUTTREE_HIERARCHICALSUPERNODETREE_H
#pragma once
