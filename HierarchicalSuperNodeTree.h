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

struct HSNode{
    int degree;
    int initDegree; // 初始极大团的大小，用于计算势
    int flag; // 标记结点中的vertex属于哪个极大团
    int hierarchical;
    set<int> vertexSet;
    HSNode *parent;
    map<HSNode*,int> children;

    void print(){
        int num = 0;
        cout<<"[";
        for(set<int>::iterator vsit=vertexSet.begin();vsit!=vertexSet.end();vsit++){
            if(num>0){
                cout<<" ";
            }
            cout<<*vsit;
            num++;
        }
        cout<<"](L"<<hierarchical<<"|"<<"D"<<degree<<")";
        if(children.size()>0){
            int i = 0;
            cout<<"{";
            for(map<HSNode*,int>::iterator cit=children.begin();cit!=children.end();cit++){
                if(i>0){
                    cout<<",";
                }
                HSNode* child = (*cit).first;
                child->print();
                if((*cit).second != 0){
                    cout<<"$"<<(*cit).second<<"$";
                }
                i++;
            }
            cout<<"}";
        }
    }
};


class HierarchicalSuperNodeTree {
public:
    HierarchicalSuperNodeTree(){};

    ~HierarchicalSuperNodeTree(){
        //construct_graph
        //forest_set

    };//release space

    void execute(vector<vector<int> > allMaximalClique);
    void insertClique(vector<int> maximalClique, int num);
    HSNode* batchUpdateTreeNode(map<HSNode*, map<int, set<int>>> treeNodeMap,set<int> newVertexSet,int newVertexNodeDegree, int num);
    void setHierarchical(HSNode *updateNode,int hierarchical);
    int getPotentialNum(HSNode* node, map<int, set<int>> updateNodeMap, set<int> newVertexSet, int flag=0);
    int computeMincut(int m, int n); // 在生成的树中计算任意两个点之间的最小割
    void outCutGragh(const string& filePath); // 输出任意两点间的最小割组成的矩阵
    void update(HSNode* curNode, HSNode* parentNode);
    void connectNewVertexSet(map<int, set<int>> updateMap, HSNode* node, HSNode* newNode, set<int> newVertexSet, int newVertexNodeDegree, int num); // 连接新节点所在的结点
    HSNode* updateTwoNodes(map<HSNode*, map<int, set<int>>> treeNodeMap, HSNode* node1, HSNode* node2);
    HSNode* updateEndNode(map<HSNode*, map<int, set<int>>> treeNodeMap, HSNode* endNode, map<HSNode*, int> road);
    void trim(string& s); // 去除字符串首尾空格
    vector<string> split(const string& str, const string& delim); // 分割字符串
    vector<vector<int>> loadData(const string& filePath); // 读取极大团

    map<int, HSNode *> vertex_node_index;
    map<int, map<int,int>*> construct_graph;
    // set<HSNode *> forest_set;

    map<int,int> graphGrow(vector<int> maximalClique, set<int> &newVertexSet, set<int> &oldVertexSet);
    void printParameters();

};

#endif //MINCUTTREE_HIERARCHICALSUPERNODETREE_H
