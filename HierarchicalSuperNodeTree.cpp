#include "HierarchicalSuperNodeTree.h"
#include <algorithm>
#include <queue>


void HierarchicalSuperNodeTree::trim(string& s) {
    if (s.empty()) {
        return;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

vector<string> HierarchicalSuperNodeTree::split(const string& s, const string& delim) {
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

vector<vector<int>> HierarchicalSuperNodeTree::loadData(const string& filePath) {
    ifstream dataFile;
    dataFile.open(filePath);
    vector<vector<int>> allMaximalClique;
    vector<int> maximalClique;

    if (dataFile.is_open()) {
        string line;
        while (getline(dataFile, line)) {
            vector<string> maximalClique_s;
            trim(line);
            maximalClique_s = split(line, " ");
            maximalClique = {};

            for (unsigned int i = 0; i < maximalClique_s.size(); i++) {
                int vertex;
                stringstream ss1;
                ss1 << maximalClique_s[i];
                ss1 >> vertex; //string -> int
                maximalClique.push_back(vertex);
            }
            allMaximalClique.push_back(maximalClique);
        }
        dataFile.close();
    }
    else {
        cout << "Open File Error!" << endl;
    }
    return allMaximalClique;
}

void HierarchicalSuperNodeTree::execute(vector<vector<int>> allMaximalClique) {
    for (unsigned int num = 0; num < allMaximalClique.size(); num++) { // 遍历所有的极大团
        vector<int> mc = allMaximalClique[num];
        vector<vector<int>> temp = splitMaximalClique(mc);
        
        for (unsigned int i = 0; i < temp.size(); i++) {
            cout << endl;
            cout << "the maximalClique added is : " << endl;
            for (unsigned int k = 0; k < temp[i].size(); k++) {
                cout << temp[i][k] << "  ";
            }
            cout << endl;

            insertClique(temp[i], {}, {}, {}, 0);
        }
        printParameters();
    }
}

void HierarchicalSuperNodeTree::insertClique(vector<int> maximalClique, map<int, int> increDegreeMap, set<int> newVertex, set<int> oldVertex, int flag) {
    // 插入极大团
    set<int> newVertexSet, oldVertexSet; // 新插入极大团中的新节点，公共节点
    map<int, int> incrementalDegreeMap;

    if (flag == 0) {
        incrementalDegreeMap = graphGrow(maximalClique, newVertexSet, oldVertexSet);
    }
    else {
        newVertexSet = newVertex;
        oldVertexSet = oldVertex;
        incrementalDegreeMap = increDegreeMap;
    }
    
    cout << "[incrementalDegree]: ";
    for (map<int, int>::iterator idit = incrementalDegreeMap.begin(); idit != incrementalDegreeMap.end(); idit++) {
        cout << (*idit).first << "-" << (*idit).second << "  ";
    }
    cout << endl;
    cout << "[newVertexSet]:";
    for (set<int>::iterator newit = newVertexSet.begin(); newit != newVertexSet.end(); newit++) {
        cout << " " << *newit;
    }
    cout << endl;
    cout << "[oldVertexSet]:";
    for (set<int>::iterator oldit = oldVertexSet.begin(); oldit != oldVertexSet.end(); oldit++) {
        cout << " " << *oldit;
    }
    cout << endl;

    //[1]group by HSNode and incrementalDegree
    map<HSNode*, map<int, set<int>>> updateNodeMap;
    for (set<int>::iterator oldit = oldVertexSet.begin(); oldit != oldVertexSet.end(); oldit++) {
        // 遍历公共节点
        int vertex = *oldit;
        if (vertex_node_index.find(vertex) != vertex_node_index.end()) {
            HSNode* node = vertex_node_index[vertex];
            map<int, set<int>> vertexSetMap;
            if (updateNodeMap.find(node) != updateNodeMap.end()) {
                vertexSetMap = updateNodeMap[node];
            }
            int incrementalDegree = incrementalDegreeMap[vertex];
            set<int> vertexSet;
            if (vertexSetMap.find(incrementalDegree) != vertexSetMap.end()) {
                vertexSet = vertexSetMap[incrementalDegree];
            }
            vertexSet.insert(vertex);
            vertexSetMap[incrementalDegree] = vertexSet;
            updateNodeMap[node] = vertexSetMap;
        }
        else {
            cout << "[ERROR 1] : " << vertex << " is not in the vertex_node_index." << endl;
        }
    }

    //[2]find their ancestors, group by ancestors
    map<HSNode*, map<HSNode*, map<int, set<int>>>> ancestorMap;
    for (map<HSNode*, map<int, set<int>>>::iterator unmit = updateNodeMap.begin(); unmit != updateNodeMap.end(); unmit++) {
        HSNode* updateNode = (*unmit).first;
        HSNode* ancestorNode = updateNode;
        while (ancestorNode->parent != NULL) {
            ancestorNode = ancestorNode->parent;
        }
        map<HSNode*, map<int, set<int>>> descendantSet;
        if (ancestorMap.find(ancestorNode) != ancestorMap.end()) {
            descendantSet = ancestorMap[ancestorNode];
        }
        descendantSet[updateNode] = (*unmit).second;
        ancestorMap[ancestorNode] = descendantSet;
    }

    //[3]update each ancestor branch
    int newVertexNodeDegree = maximalClique.size() - 1; // 当前极大团的度数
    if (ancestorMap.size() == 0) {
        // 没有公共节点
        if (!newVertexSet.empty()) {
            HSNode* newTreeNode = new HSNode();
            newTreeNode->degree = newVertexNodeDegree;
            newTreeNode->initDegree = newVertexNodeDegree + 1;
            newTreeNode->hierarchical = 0;
            newTreeNode->vertexSet = newVertexSet;
            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                vertex_node_index[*nvsit] = newTreeNode;
            }
        }
        else {
            cout << "[ERROR 2] : newVertexSet is empty." << endl;
        }
    }
    else if (ancestorMap.size() == 1) {
        // 公共节点在一个连通图中
        // 注意：存在公共节点度数增长为0的情况，比如{{1，2，3，4}，{3，4，5，6}，{1，3，4，6}}中节点3，4
        map<HSNode*, map<HSNode*, map<int, set<int>>>>::iterator amit = ancestorMap.begin();
        map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
        HSNode* updateNode = batchUpdateTreeNode(maximalClique, treeNodeMap, incrementalDegreeMap,newVertexSet, newVertexNodeDegree, oldVertexSet);
    }
    else {
        // 公共节点在多个连通图中
        HSNode* newNode = new HSNode();
        newNode->degree = maximalClique.size() - 1;
        newNode->initDegree = maximalClique.size();
        if (!newVertexSet.empty()) {
            newNode->vertexSet = newVertexSet;
        }
        for (set<int>::iterator nit = newVertexSet.begin(); nit != newVertexSet.end(); nit++) {
            vertex_node_index[*nit] = newNode;
        }

        // 取出第一棵树作为parent方向
        map<HSNode*, map<HSNode*, map<int, set<int>>>>::iterator amit = ancestorMap.begin();
        map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
        HSNode* parentNode = batchUpdateTreeNode(maximalClique, treeNodeMap, incrementalDegreeMap, newVertexSet, newVertexNodeDegree, oldVertexSet);
        newNode->parent = parentNode; newNode->hierarchical = parentNode->hierarchical + 1;
        parentNode->children.insert(make_pair(newNode, 0)); /// 注意：势

        // 遍历剩下的树，依次作为child方向重构hierarchical
        for (amit++; amit != ancestorMap.end(); amit++) {
            map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
            HSNode* curNode = batchUpdateTreeNode(maximalClique, treeNodeMap, incrementalDegreeMap, newVertexSet, newVertexNodeDegree, oldVertexSet);
            HSNode* pa = curNode->parent; curNode->parent = newNode; newNode->children.insert(make_pair(curNode, 0)); /// 注意：势
            curNode->hierarchical = newNode->hierarchical + 1;
            while (pa != NULL) {
                HSNode* tmp = pa->parent;
                pa->parent = curNode;
                curNode->children.insert(make_pair(pa, pa->children[curNode])); pa->children.erase(curNode);
                update(curNode, pa);
                curNode = pa; pa = tmp;
            }
            update(curNode, pa);
        }
    }
}

void HierarchicalSuperNodeTree::update(HSNode* curNode, HSNode* parentNode) {
    if (curNode->children.size() == 0) {
        return;
    }
    for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
        (*cit).first->hierarchical = curNode->hierarchical + 1;
        if ((*cit).first != parentNode) {
            update((*cit).first, parentNode);
        }
    }
}

void HierarchicalSuperNodeTree::connectNewVertexSet(map<int, set<int>> updateMap, HSNode* node, HSNode* newNode, set<int> newVertexSet, int newVertexNodeDegree) {
    if (!newVertexSet.empty()) {//connect
        HSNode* newTreeNode = new HSNode();
        newTreeNode->degree = newVertexNodeDegree;
        newTreeNode->initDegree = newVertexNodeDegree + 1;
        newTreeNode->hierarchical = newNode->hierarchical + 1;
        newTreeNode->parent = newNode;
        newTreeNode->vertexSet = newVertexSet;
        for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
            vertex_node_index[*nvsit] = newTreeNode;
        }
        newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
    }
}

HSNode* HierarchicalSuperNodeTree::batchUpdateTreeNode(vector<int> maximalClique, map<HSNode*, map<int, set<int>>> treeNodeMap, map<int, int> increDegreeMap, set<int> newVertexSet, int newVertexNodeDegree, set<int> oldVertexSet) {
    // 对同一个连通图中的节点进行更新，分为 公共节点在同一个结点中 以及 在多个结点中 两种情况
    // 公共节点在同一个结点中
    // 注意：map<int, set<int>> 中set的size为1的情况需要单独考虑
    if (treeNodeMap.size() == 1) {
        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();
        HSNode* node = (*tnmit).first; map<int, set<int>> updateMap = (*tnmit).second;
        map<int, set<int>>::iterator updateit = updateMap.begin();
        int newDegree = node->degree + (*updateit).first; int flag0 = 0;

        // attention
        if ((*updateit).first == 0) {
            return node;
        }

        // 若需更新的公共节点个数为1，则新创建结点与当前结点相连
        if ((*updateit).second.size() == 1) {
            flag0 = 1;

            HSNode* newNode = new HSNode; 
            newNode->degree = newDegree;
            newNode->initDegree = node->initDegree;
            newNode->vertexSet = (*updateit).second;
            newNode->parent = node; newNode->hierarchical = node->hierarchical + 1;
            node->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newNode->vertexSet))); // actually, potential must be 0

            // 将newNode中的节点与newNode对应起来存到vertex_node_index中
            for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                node->vertexSet.erase(*nnit);
                vertex_node_index[*nnit] = newNode;
            }

            // 连接newVertexSet
            connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

            return newNode;
        }
        // 若需更新的公共节点个数大于等于2
        // 若父结点存在且度数大于当前结点
        else if (node->parent != nullptr && node->parent->degree > node->degree) {
            // 根据该边的势的大小进行分类处理
            // 情况1：势为0
            int potential = node->parent->children[node];

            // 势大于0
            if (potential > 0) {
                flag0 = 1;
                int degreeGap = node->parent->degree - node->degree;
                // 根据势与degreeGap的大小关系分类讨论
                // 分为 势=degreeGap 与 势<degreeGap 两种情况
                // potential = degreeGap的情况
                if (potential == degreeGap) {
                    // 若新结点的度数大于node的父节点的度数
                    if (newDegree > node->parent->degree) {
                        HSNode* newNode = new HSNode; newNode->degree = newDegree;
                        newNode->initDegree = node->initDegree;
                        newNode->vertexSet = (*updateit).second;
                        newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, 0)); /// 注意：势的大小
                        newNode->hierarchical = node->parent->hierarchical + 1;

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        // 连接newVertexSet
                        connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                        return newNode;
                    }
                    else if (newDegree <= node->parent->degree) { // 若新结点的度数小于等于node的父节点的度数
                        HSNode* newNode = new HSNode; newNode->degree = newDegree;
                        newNode->initDegree = node->initDegree;
                        newNode->vertexSet = (*updateit).second;
                        newNode->parent = node->parent; newNode->hierarchical = node->parent->hierarchical + 1;
                        node->parent->children.erase(node);
                        node->parent = newNode;
                        newNode->children.insert(make_pair(node, newNode->degree - node->degree));
                        node->parent->children.insert(make_pair(newNode, node->parent->degree - newNode->degree));
                        setHierarchical(node, newNode->hierarchical + 1);

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = (*updateit).second.begin(); nnit != (*updateit).second.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        // 连接newVertexSet
                        connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                        return newNode;
                    }
                }
                // potential < degreeGap的情况
                else if (potential < degreeGap) {
                    // 分为增加的度数 <= potential 与 > potential两种情况
                    // 情况1：增加的度数 <= potential
                    if ((*updateit).first <= potential) {
                        HSNode* newNode = new HSNode; newNode->parent = node->parent;
                        node->parent->children.insert(make_pair(newNode, potential - getPotentialNum(node, updateMap, newVertexSet))); node->parent->children.erase(node);
                        newNode->degree = newDegree; newNode->initDegree = node->initDegree;
                        newNode->vertexSet = (*updateit).second; newNode->hierarchical = node->hierarchical;
                        node->parent = newNode; newNode->children.insert(make_pair(node, getPotentialNum(node, updateMap, newVertexSet)));
                        setHierarchical(node, newNode->hierarchical + 1);
                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        // 连接newVertexSet
                        connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                        return newNode;
                    }
                    // 情况2：增加的度数 > potential
                    else if ((*updateit).first > potential) {
                        HSNode* nullNode = new HSNode; nullNode->parent = node->parent;
                        node->parent->children.insert(make_pair(nullNode, 0)); node->parent->children.erase(node);
                        nullNode->degree = newDegree; nullNode->initDegree = newDegree; nullNode->vertexSet = {}; nullNode->hierarchical = node->hierarchical;
                        node->parent = nullNode; nullNode->children.insert(make_pair(node, potential));
                        setHierarchical(node, nullNode->hierarchical + 1);

                        HSNode* newNode = new HSNode; newNode->parent = nullNode;
                        newNode->degree = newDegree; newNode->initDegree = node->initDegree;
                        nullNode->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newVertexSet) - potential));
                        newNode->vertexSet = (*updateit).second; newNode->hierarchical = nullNode->hierarchical + 1;

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        // 连接newVertexSet
                        connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                        return newNode;
                    }
                }
            }
        }
        // 若node存在子结点，遍历并更新
        else if (node->children.size() > 0) {
            for (map<HSNode*, int>::iterator childit = node->children.begin(); childit != node->children.end(); childit++) {
                HSNode* childNode = (*childit).first;
                // 若子结点度数大于node的度数
                if (childNode->degree > node->degree) {
                    int potential = (*childit).second;
                    // 若该边的势为0
                    if (potential == 0) {
                        continue;
                    }
                    // 若该边的势大于0
                    else if (potential < 10000) {
                        int degreeGap = childNode->degree - node->degree;
                        // 根据势与degreeGap的大小关系分类讨论
                        // 分为 势=degreeGap 与 势<degreeGap 两种情况
                        // potential = degreeGap的情况
                        if (potential == degreeGap) {
                            flag0 = 1;

                            // 若新结点的度数大于node的子节点的度数
                            if (newDegree > childNode->degree) {
                                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->initDegree = node->initDegree;
                                newNode->vertexSet = (*updateit).second;
                                newNode->parent = childNode; childNode->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newVertexSet) - potential)); /// 注意：势的大小
                                newNode->hierarchical = childNode->hierarchical + 1;

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                // 连接newVertexSet
                                connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                                return newNode;
                            }
                            // 若新结点的度数小于等于node的子节点的度数
                            else if (newDegree <= childNode->degree) {
                                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->initDegree = node->initDegree;
                                newNode->vertexSet = (*updateit).second;
                                newNode->parent = node; node->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newVertexSet))); node->children.erase(childNode);
                                newNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = newNode; newNode->children.insert(make_pair(childNode, potential - getPotentialNum(node, updateMap, newVertexSet)));
                                setHierarchical(childNode, newNode->hierarchical + 1);

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                // 连接newVertexSet
                                connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                                return newNode;
                            }
                        }
                        // potential < degreeGap的情况
                        else if (potential < degreeGap) {
                            // 分为增加的度数 <= potential 与 > potential两种情况
                            // 情况1：增加的度数 <= potential
                            if ((*updateit).first <= potential) {
                                HSNode* newNode = new HSNode; newNode->parent = node; node->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newVertexSet)));
                                newNode->degree = newDegree; newNode->initDegree = node->initDegree;
                                newNode->vertexSet = (*updateit).second; newNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = newNode; newNode->children.insert(make_pair(node, potential - getPotentialNum(node, updateMap, newVertexSet))); node->children.erase(childNode);
                                setHierarchical(childNode, newNode->hierarchical + 1);
                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                // 连接newVertexSet
                                connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                                return newNode;
                            }
                            // 情况2：增加的度数 > potential
                            else if ((*updateit).first > potential) {
                                HSNode* nullNode = new HSNode; nullNode->parent = node;
                                node->children.insert(make_pair(nullNode, potential)); node->children.erase(childNode);
                                nullNode->degree = newDegree; nullNode->vertexSet = {}; nullNode->initDegree = newDegree;
                                nullNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = nullNode; nullNode->children.insert(make_pair(childNode, 0));
                                setHierarchical(childNode, nullNode->hierarchical + 1);

                                HSNode* newNode = new HSNode; newNode->parent = nullNode; nullNode->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newVertexSet) - potential));
                                newNode->initDegree = node->initDegree;
                                newNode->vertexSet = (*updateit).second; newNode->hierarchical = nullNode->hierarchical + 1;

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                // 连接newVertexSet
                                connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

                                return newNode;
                            }
                        }
                    }
                }
            }
        }
        if (flag0 == 0) {
            HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->initDegree = node->initDegree;
            newNode->vertexSet = (*updateit).second;
            // 从node中移除度数增加的节点,将其指向新结点
            for (set<int>::iterator sit = (*updateit).second.begin(); sit != (*updateit).second.end(); sit++) {
                node->vertexSet.erase(*sit);
                vertex_node_index[*sit] = newNode;
            }
            newNode->hierarchical = node->hierarchical + 1;
            newNode->parent = node; node->children[newNode] = getPotentialNum(node, updateMap, newVertexSet);

            // 将newNode中的节点与newNode对应起来存到vertex_node_index中
            for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                node->vertexSet.erase(*nnit);
                vertex_node_index[*nnit] = newNode;
            }

            // 连接newVertexSet
            connectNewVertexSet(updateMap, node, newNode, newVertexSet, newVertexNodeDegree);

            return newNode;
        }
    }
    else if (treeNodeMap.size() > 1) {
        // 公共节点在多个结点中
        // 注意：存在 map<int, set<int>> 中 int 值为0的情况，或者 set 的size为1的情况

        // 将公共节点所在的结点间的路径存入road中
        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();

        // 峰结点需要下降的度数
        // int descendantDegree = treeNodeMap.begin()->second.begin()->first;
        // int descendantDegree = (tnmit->second.begin()->first - newVertexSet.size()) * (tnmit->second.begin()->second.size());
        // int descendantDegree = maximalClique.size() - 1;
        int descendantDegree = treeNodeMap.begin()->second.begin()->first * treeNodeMap.begin()->second.begin()->second.size();
        cout << "descendantDegree is : " << descendantDegree << endl;


        map<HSNode*, int> road1; // 存储初始时结点间的路径
        map<HSNode*, int> road2; // 存储公共结点下降后结点间的路径
        set<HSNode*> newUpdateNode;

        // 查找公共结点间的路径
        HSNode* node1; HSNode* node2;
        for (tnmit++; tnmit != treeNodeMap.end(); tnmit++) {
            node1 = (*treeNodeMap.begin()).first;
            node2 = (*tnmit).first;
            road1 = searchRoad(node1, node2);
        }

        // 输出路径road1
        cout << "road1 is : " << endl;
        for (map<HSNode*, int>::iterator rit = road1.begin(); rit != road1.end(); rit++) {
            cout << rit->first->degree << " ";
        }
        cout << endl;

        // 判断峰结点的度数
        // 若小于descendantDegree则需要进行重切割
        int minDegree1 = 1000; HSNode* recutNode1 = NULL; set<HSNode*> childSet1 = {}; 
        int minDegree2 = 1000; HSNode* recutNode2 = NULL; set<HSNode*> childSet2 = {};

        for (map<HSNode*, int>::iterator roadit = road1.begin(); roadit != road1.end(); roadit++) {
            HSNode* curNode = (*roadit).first; set<HSNode*> tempChildSet = {};
            int count_parent = 0, count_child = 0; // 记录路径上curNode父子结点的个数

            // 计算路径上curNode父结点的个数
            if (road1.find(curNode->parent) != road1.end() && curNode->parent->degree >= curNode->degree) {
                count_parent++;
            }

            // 计算路径上curNode子结点的个数
            if (curNode->children.size() > 0) {
                for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
                    if (road1.find((*cit).first) != road1.end() && (*cit).first->degree >= curNode->degree) {
                        count_child++; tempChildSet.insert((*cit).first);
                    }
                }
            }

            // curNode为峰结点
            if ((count_parent + count_child) >= 2) {
                if (curNode->degree <= minDegree1) {
                    minDegree1 = curNode->degree; recutNode1 = curNode; childSet1 = tempChildSet;
                }
            }

            // curNode为端结点
            if ((count_parent + count_child) == 1) {
                if (curNode->degree <= minDegree2) {
                    minDegree2 = curNode->degree; recutNode2 = curNode; childSet2 = tempChildSet;
                }
            }
        }

        // 判断度数最小的点是峰结点还是端结点
        int tap = 0; // tap=0标记recutNode在中间， tap=1标记recutNode在两端
        int minDegree = 1000; HSNode* recutNode = NULL; set<HSNode*> childSet = {};
        if (minDegree1 <= minDegree2) {
            minDegree = minDegree1;
            tap = 0; recutNode = recutNode1; childSet = childSet1;
        }
        else {
            minDegree = minDegree2;
            tap = 1; recutNode = recutNode2; childSet = childSet2;
        }

        cout << endl;
        cout << "peakNode and endNode 's littlest degree is : " << minDegree << endl;

        if (minDegree < descendantDegree) {
            cout << endl;
            cout << "need to recut!" << endl;
            cout << endl;

            HSNode* nullNode = new HSNode;
            nullNode->degree = recutNode->degree; nullNode->initDegree = recutNode->initDegree;
            nullNode->vertexSet = {};

            if (tap == 0) { // recutNode为峰结点
                // 将nullNode设置为重切割后一棵树的root
                HSNode* childNode = *childSet.begin();
                childNode->parent = nullNode; nullNode->children.insert(make_pair(childNode, recutNode->children[childNode]));
                recutNode->children.erase(childNode);
                setHierarchical(nullNode, 0);
            }
            else { // recutNode为端结点

                // 端结点的父结点在路径上
                // 将nullNode设置为重切割后一棵树的尾部
                if (childSet.size() == 0) { 
                    nullNode->parent = recutNode->parent; recutNode->parent->children.insert(make_pair(nullNode, recutNode->parent->children[recutNode]));
                    nullNode->hierarchical = recutNode->hierarchical;
                    nullNode->parent->children.erase(recutNode); recutNode->parent = NULL;
                    setHierarchical(recutNode, 0);
                }
                // 端结点的子结点在路径上
                // 将nullNode设置为重切割后一棵树的root
                else {
                    // 将nullNode设置为重切割后一棵树的root
                    HSNode* childNode = *childSet.begin();
                    childNode->parent = nullNode; nullNode->children.insert(make_pair(childNode, recutNode->children[childNode]));
                    recutNode->children.erase(childNode);
                    setHierarchical(nullNode, 0);
                }
            }

            // 重新加入当前极大团
            insertClique(maximalClique, increDegreeMap, newVertexSet, oldVertexSet, 1);

            // 将重切割时切断的边重新加上去
            map<HSNode*, int> road = searchRoad(recutNode, nullNode);
            updatePeakNode(road, minDegree);

            return NULL;
        }


        // 更新公共节点所在的结点
        for (map<HSNode*, map<int, set<int>>>::iterator tit = treeNodeMap.begin(); tit != treeNodeMap.end(); tit++) {
            HSNode* endNode = (*tit).first; map<int, set<int>> updateEndNodeMap = (*tit).second;
            int flag = 0; // 标记结点更新是否成功
            int newDegree = endNode->degree + updateEndNodeMap.begin()->first; // 新结点的度数

            // 若端结点endNode的父节点存在且在路径上
            if (endNode->parent != nullptr && road1.find(endNode->parent) != road1.end()) {
                // 若端结点endNode的父节点的度数比endNode大
                if (endNode->parent->degree > endNode->degree) {
                    flag = 1;

                    if (newDegree > endNode->parent->degree) { // 若新结点的度数大于endNode的父节点的度数
                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->initDegree = endNode->initDegree;
                        newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                        newEndNode->parent = endNode->parent; endNode->parent->children.insert(make_pair(newEndNode, 0));
                        newEndNode->hierarchical = endNode->parent->hierarchical + 1;

                        // 将新结点加入newUpdateNode中
                        newUpdateNode.insert(newEndNode);

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                            endNode->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newEndNode;
                        }
                    }
                    else if (newDegree <= endNode->parent->degree) { // 若新结点的度数小于等于endNode的父节点的度数
                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->initDegree = endNode->initDegree;
                        newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                        newEndNode->parent = endNode->parent;
                        endNode->parent->children.insert(make_pair(newEndNode, endNode->parent->degree - newEndNode->degree)); // fixme
                        newEndNode->hierarchical = endNode->parent->hierarchical + 1;
                        endNode->parent->children.erase(endNode);
                        endNode->parent = newEndNode;
                        newEndNode->children.insert(make_pair(endNode, newEndNode->degree - endNode->degree)); // fixme
                        setHierarchical(endNode, newEndNode->hierarchical + 1);

                        // 将新结点加入newUpdateNode中
                        newUpdateNode.insert(newEndNode);

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                            endNode->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newEndNode;
                        }
                    }
                }
            }
            else { // 若端结点endNode的父节点不存在，或者不在路径上
                // 若endNode存在子结点
                if (endNode->children.size() > 0) {
                    for (map<HSNode*, int>::iterator chit = endNode->children.begin(); chit != endNode->children.end(); chit++) {
                        HSNode* childNode = (*chit).first;
                        // 若该子结点在路径上
                        if (road1.find(childNode) != road1.end()) {
                            if (childNode->degree > endNode->degree) {
                                flag = 1;
                                if (newDegree > childNode->degree) { // 若新结点的度数大于endNode的子节点的度数
                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->initDegree = endNode->initDegree;
                                    newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                                    newEndNode->parent = childNode; childNode->children.insert(make_pair(newEndNode, 0));
                                    newEndNode->hierarchical = childNode->hierarchical + 1;

                                    // 将新结点加入newUpdateNode中
                                    newUpdateNode.insert(newEndNode);

                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
                                    for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                                        endNode->vertexSet.erase(*nnit);
                                        vertex_node_index[*nnit] = newEndNode;
                                    }
                                }
                                else if (newDegree <= childNode->degree) { // 若新结点的度数小于等于endNode的子节点的度数
                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->initDegree = endNode->initDegree;
                                    newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                                    newEndNode->parent = endNode; endNode->children.insert(make_pair(newEndNode, newDegree - endNode->degree)); // fixme
                                    endNode->children.erase(childNode);
                                    childNode->parent = newEndNode; newEndNode->children.insert(make_pair(childNode, childNode->degree - newEndNode->degree)); // fixme
                                    newEndNode->hierarchical = endNode->hierarchical + 1;
                                    setHierarchical(childNode, newEndNode->hierarchical + 1);

                                    // 将新结点加入newUpdateNode中
                                    newUpdateNode.insert(newEndNode);

                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
                                    for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                                        endNode->vertexSet.erase(*nnit);
                                        vertex_node_index[*nnit] = newEndNode;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
            // 若结点未更新成功，即endNode的父(子)结点不在road中，或者在road中但是度数均比endNode低
            if (flag == 0) {
                HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->initDegree = endNode->initDegree;
                newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                newEndNode->parent = endNode; endNode->children.insert(make_pair(newEndNode, 0));
                newEndNode->hierarchical = endNode->hierarchical + 1;

                // 将新结点加入newUpdateNode中
                newUpdateNode.insert(newEndNode);

                // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
                for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                    endNode->vertexSet.erase(*nnit);
                    vertex_node_index[*nnit] = newEndNode;
                }
            }
        }

        // 更新公共结点后，重新查找新创建结点间的路径
        set<HSNode*>::iterator nunit = newUpdateNode.begin();

        for (nunit++; nunit != newUpdateNode.end(); nunit++) {
            HSNode* newNode1 = *newUpdateNode.begin();
            HSNode* newNode2 = *nunit;
            road2 = searchRoad(newNode1, newNode2);
        }

        // 输出路径road2
        cout << "road2 is : " << endl;
        for (map<HSNode*, int>::iterator rit = road2.begin(); rit != road2.end(); rit++) {
            cout << rit->first->degree << " ";
        }
        cout << endl;

        // 更新峰结点
        updatePeakNode(road2, descendantDegree);
        
        HSNode* newNode = *(newUpdateNode.begin());
        if (!newVertexSet.empty()) {//connect
            HSNode* newTreeNode = new HSNode();
            newTreeNode->degree = newVertexNodeDegree;
            newTreeNode->initDegree = newVertexNodeDegree + 1;
            newTreeNode->hierarchical = newNode->hierarchical + 1;
            newTreeNode->parent = newNode;
            newTreeNode->vertexSet = newVertexSet;
            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                vertex_node_index[*nvsit] = newTreeNode;
            }
            newNode->children.insert(make_pair(newTreeNode, 0)); // 注意：势的确定 fixme
        }

        return *(newUpdateNode.begin());
    }

    cout << "[error]" << endl;
    return NULL;
}

void HierarchicalSuperNodeTree::updatePeakNode(map<HSNode*, int> road, int descendantDegree) {
    if (descendantDegree == 0) {
        return;
    }

    // 判断是否为峰结点并存储在peakNodeSet中
    set<HSNode*> peakNodeSet; // 存储峰结点

    for (map<HSNode*, int>::iterator roadit = road.begin(); roadit != road.end(); roadit++) {
        HSNode* curNode = (*roadit).first;
        int count_parent = 0, count_child = 0; // 记录路径上curNode父子结点的个数

        // 计算路径上curNode父结点的个数
        if (road.find(curNode->parent) != road.end() && curNode->parent->degree >= curNode->degree) {
            count_parent++;
        }

        // 计算路径上curNode子结点的个数
        if (curNode->children.size() > 0) {
            for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
                if (road.find((*cit).first) != road.end() && (*cit).first->degree >= curNode->degree) {
                    count_child++;
                }
            }
        }

        //curNode为峰结点
        if ((count_parent + count_child) >= 2) {
            peakNodeSet.insert(curNode);
        }
    }

    // 更新峰结点
    for (set<HSNode*>::iterator pnit = peakNodeSet.begin(); pnit != peakNodeSet.end(); pnit++) {
        HSNode* peakNode = *pnit;
        HSNode* newNode = new HSNode;
        newNode->degree = peakNode->degree + descendantDegree;
        newNode->initDegree = peakNode->initDegree;
        newNode->vertexSet = {};
        set<HSNode*> childrenSet; // 保存路径上peakNode的子结点

        for (map<HSNode*, int>::iterator cit = peakNode->children.begin(); cit != peakNode->children.end(); cit++) {
            if (road.find((*cit).first) != road.end() && (*cit).first->degree >= peakNode->degree) {
                childrenSet.insert((*cit).first);
            }
        }

        // 将peakNode在路径上的子结点转移到newNode上（考虑了在多个结点的情况）
        for (set<HSNode*>::iterator csit = childrenSet.begin(); csit != childrenSet.end(); csit++) {
            (*csit)->parent = newNode;
            newNode->children.insert(make_pair(*csit, peakNode->children[*csit]));
            peakNode->children.erase(*csit);
        }

        // peakNode的父节点在路径中
        if (road.find(peakNode->parent) != road.end() && peakNode->parent->degree >= peakNode->degree) {
            // 更新层次信息
            // 注意：势的关系的继承
            newNode->parent = peakNode->parent;
            peakNode->parent->children.insert(make_pair(newNode, 0));
            newNode->hierarchical = peakNode->parent->hierarchical + 1;
            peakNode->parent->children.erase(peakNode);
            peakNode->parent = newNode;
            newNode->children.insert(make_pair(peakNode, 0));
            setHierarchical(newNode, newNode->hierarchical);
        }
        // curNode的父节点不在路径中
        else {
            // 更新层次信息
            // 注意：势的关系的继承
            newNode->parent = peakNode; peakNode->children.insert(make_pair(newNode, 0));
            setHierarchical(newNode, peakNode->hierarchical + 1);
        }

        road.erase(peakNode); road.insert(make_pair(newNode, 0));
    }
}

map<HSNode*, int> HierarchicalSuperNodeTree::searchRoad(HSNode* node1, HSNode* node2) {
    map<HSNode*, int> road;
    road[node1] = 1; road[node2] = 1;
    while (node1->hierarchical > 0 && node2->hierarchical > 0) {
        if (node1 == node2) {
            break;
        }
        else if (node1->hierarchical <= node2->hierarchical) {
            node2 = node2->parent; road[node2] = 1;
        }
        else if (node1->hierarchical > node2->hierarchical) {
            node1 = node1->parent; road[node1] = 1;
        }
    }
    if (node2->hierarchical == 0) {
        while (node1->hierarchical > 0) {
            node1 = node1->parent; road[node1] = 1;
        }
    }
    if (node1->hierarchical == 0) {
        while (node2->hierarchical > 0) {
            node2 = node2->parent; road[node2] = 1;
        }
    }
    return road;
}

void HierarchicalSuperNodeTree::setHierarchical(HSNode* updateNode, int hierarchical) {
    updateNode->hierarchical = hierarchical;
    for (map<HSNode*, int>::iterator ucit = updateNode->children.begin(); ucit != updateNode->children.end(); ucit++) {
        HSNode* child = (*ucit).first;
        setHierarchical(child, hierarchical + 1);
    }
}

int HierarchicalSuperNodeTree::getPotentialNum(HSNode* node, map<int, set<int>> updateNodeMap, set<int> newVertexSet, int flag) {
    // 计算势的大小，其中flag=0表示计算的是node与公共节点的势，flag=1表示计算的是公共节点与新节点的势
    int updateVertexNum = updateNodeMap.begin()->second.size();
    int newVertexNum = newVertexSet.size();

    //cout << "getPotentialNum: " << endl;
    //cout << "updateVertexNum -- node->initDegree -- newVertexNum" << endl;
    //cout << updateVertexNum << " -- " << node->initDegree << " -- " << newVertexNum << endl;

    if (flag == 0) {
        return min((node->initDegree - updateVertexNum) * updateVertexNum - node->initDegree + 1, newVertexNum);
    }
    else if (flag == 1) {
        return min(newVertexNum * updateVertexNum - (updateVertexNum + newVertexNum - 1), node->initDegree - updateVertexNum);
    }
    else {
        cout << "[error]:flag can only equal to 0 or 1." << endl;
        return 0;
    }
}

// Update the graph and return the incremental degree of vertices.
map<int, int> HierarchicalSuperNodeTree::graphGrow(vector<int> maximalClique, set<int>& newVertexSet, set<int>& oldVertexSet) {
    map<int, int> incrementalDegree;
    for (vector<int>::iterator mcit = maximalClique.begin(); mcit != maximalClique.end(); mcit++) {
        int vertex = *mcit;
        int degree = 0;
        if (construct_graph.find(vertex) != construct_graph.end()) {
            map<int, int>* neighbor = construct_graph[vertex];
            for (vector<int>::iterator mcit1 = maximalClique.begin(); mcit1 != maximalClique.end(); mcit1++) {
                if (neighbor->find(*mcit1) == neighbor->end() && vertex != *mcit1) {
                    degree++;
                    neighbor->insert(make_pair(*mcit1, 1));
                }
            }
            oldVertexSet.insert(vertex);
        }
        else {
            map<int, int>* neighbor = new map<int, int>();
            for (vector<int>::iterator mcit2 = maximalClique.begin(); mcit2 != maximalClique.end(); mcit2++) {
                if (vertex != *mcit2) {
                    degree++;
                    neighbor->insert(make_pair(*mcit2, 1));
                }
            }
            construct_graph[vertex] = neighbor;
            newVertexSet.insert(vertex);
        }
        incrementalDegree.insert(make_pair(vertex, degree));
    }

    return incrementalDegree;
}

vector<vector<int>> HierarchicalSuperNodeTree::splitMaximalClique(vector<int> maximalClique) {
    map<HSNode*, vector<int>> tempMap = {}; vector<int> newVertex = {};
    for (vector<int>::iterator mit = maximalClique.begin(); mit != maximalClique.end(); mit++) {
        if (vertex_node_index.find(*mit) != vertex_node_index.end()) {
            if (tempMap.find(vertex_node_index[*mit]) == tempMap.end()) {
                vector<int> vertexSet = {};
                vertexSet.push_back(*mit);
                tempMap[vertex_node_index[*mit]] = vertexSet;
            }
            else {
                vector<int> vertexSet = tempMap[vertex_node_index[*mit]];
                vertexSet.push_back(*mit);
                tempMap[vertex_node_index[*mit]] = vertexSet;
            }
        }
        else {
            newVertex.push_back(*mit);
        }
    }

    cout << "newVertex is : " << endl;
    for (unsigned int k = 0; k < newVertex.size(); k++) {
        cout << newVertex[k] << "  ";
    }
    cout << endl;

    vector<vector<int>> temp;
    if (newVertex.size() != 0) {
        temp.push_back(newVertex);
    }
    if (tempMap.size() != 0) {
        for (map<HSNode*, vector<int>>::iterator tmit = tempMap.begin(); tmit != tempMap.end(); tmit++) {
            temp.push_back((*tmit).second);
        }
    }

    map<HSNode*, set<HSNode*>> ancestorMap;
    for (map<HSNode*, vector<int>>::iterator tmit = tempMap.begin(); tmit != tempMap.end(); tmit++) {
        HSNode* node = (*tmit).first;
        while (node->parent != NULL) {
            node = node->parent;
        }
        if (ancestorMap.find(node) != ancestorMap.end()) {
            set<HSNode*> nodeSet = ancestorMap[node];
            nodeSet.insert((*tmit).first);
            ancestorMap[node] = nodeSet;
        }
        else {
            set<HSNode*> nodeSet = {};
            nodeSet.insert(node);
            ancestorMap[node] = nodeSet;
        }
    }

    int flag = 0;
    for (map<HSNode*, set<HSNode*>>::iterator amit = ancestorMap.begin(); amit != ancestorMap.end(); amit++) {
        if ((*amit).second.size() >= 3) {
            flag = 1;
        }
    }

    vector<vector<int>> newAllMaximalClique;
    if (flag == 1) {
        if (temp.size() == 1) {
            newAllMaximalClique.push_back(temp[0]);
            return newAllMaximalClique;
        }

        for (unsigned int i = 0; i < temp.size() - 1; i++) {
            for (unsigned int j = i + 1; j < temp.size(); j++) {
                vector<int> tempVec = {}; tempVec.resize(temp[i].size() + temp[j].size());
                merge(temp[i].begin(), temp[i].end(), temp[j].begin(), temp[j].end(), tempVec.begin());
                newAllMaximalClique.push_back(tempVec);
            }
        }

        return newAllMaximalClique;
    }
    else {
        newAllMaximalClique.push_back(maximalClique);
        return newAllMaximalClique;
    }
}

void HierarchicalSuperNodeTree::printParameters() {
    //[1] map<int, map<int,int>*> construct_graph;
    cout << "[construct_graph]:" << endl;
    for (map<int, map<int, int>*>::iterator cgit = construct_graph.begin(); cgit != construct_graph.end(); cgit++) {
        cout << " " << (*cgit).first << " :";
        map<int, int>* neighbor = (*cgit).second;
        for (map<int, int>::iterator nit = neighbor->begin(); nit != neighbor->end(); nit++) {
            cout << " " << (*nit).first;
        }
        cout << endl;
    }
    cout << endl;

    cout << "[vertex_node_index]\nvertex: hierarchical   degree" << endl;
    for (map<int, HSNode*>::iterator vnit = vertex_node_index.begin(); vnit != vertex_node_index.end(); vnit++) {
        HSNode* node = (*vnit).second;
        cout << "  " << (*vnit).first << "   :      " << node->hierarchical << "            " << node->degree << endl;
    }
}

int HierarchicalSuperNodeTree::computeMincut(int m, int n) {
    HSNode* Node1 = vertex_node_index[m]; HSNode* Node2 = vertex_node_index[n];

    int minCut = min(Node1->degree, Node2->degree);
    while (Node1->hierarchical > 0 && Node2->hierarchical > 0) {
        if (Node1 == Node2) {
            return minCut;
        }
        else if (Node1->hierarchical <= Node2->hierarchical) {
            Node2 = Node2->parent;
            minCut = min(minCut, Node2->degree);
        }
        else if (Node1->hierarchical > Node2->hierarchical) {
            Node1 = Node1->parent;
            minCut = min(minCut, Node1->degree);
        }
    }

    while (Node1->hierarchical > 0) {
        Node1 = Node1->parent;
        minCut = min(minCut, Node1->degree);
    }
    while (Node2->hierarchical > 0) {
        Node2 = Node2->parent;
        minCut = min(minCut, Node2->degree);
    }

    return minCut;
}

void HierarchicalSuperNodeTree::outCutGragh(const string& filePath) {
    ofstream outputFile;
    outputFile.open(filePath);
    int vertexNumber = vertex_node_index.size();
    if (outputFile.is_open()) {
        for (int i = 1; i <= vertexNumber; i++) {
            for (int j = 1; j <= vertexNumber; j++) {
                if (i == j) {
                    outputFile << 0 << "    ";
                }
                else {
                    outputFile << computeMincut(i, j) << "    ";
                }
            }
            outputFile << endl;
        }
    }
    outputFile.close();
}

void HierarchicalSuperNodeTree::sequenceTraversal() {
    // 层序遍历输出图
    if (vertex_node_index.size() == 0) {
        cout << "The gragh is empty." << endl;
        return;
    }
    HSNode* node = vertex_node_index[1];
    while (node->parent != NULL) {
        node = node->parent;
    }
    int hierarchical = 0;
    cout << "h" << hierarchical << endl;

    if (node->hierarchical == 0) {
        
        queue<HSNode*> q; q.push(node); queue<HSNode*> newQ;
        while (!q.empty()) {
            HSNode* curNode = q.front(); q.pop();
            
            cout << "degree: " << curNode->degree << endl;
            cout << "vertexSet is : " << "  ";
            for (set<int>::iterator it = curNode->vertexSet.begin(); it != curNode->vertexSet.end(); it++) {
                cout << *it << "  ";
            }
            cout << endl; cout << endl;

            if (curNode->children.size() > 0) {
                for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
                    newQ.push((*cit).first);
                }
            }
            if (q.empty() && !newQ.empty()) {
                cout << endl;
                q = newQ;
                newQ = {};
                hierarchical++;
                cout << "h" << hierarchical << endl;
            }
        }
    }
    else {
        cout << "[error]:the ancestor's hierarchical must be 0." << endl;
    }
}


int main() {
    /*int num = 14;

    for (int i = 0; i < num; i++) {
        stringstream ss; string i_s;
        ss << i; ss >> i_s;
        
        string filePath = "C://Users//Administrator//Desktop//example//HSNT//maximalClique" + i_s + ".txt";
        string outFilePath = "C://Users//Administrator//Desktop//result//HSNT//HSNT" + i_s + ".txt";

        HierarchicalSuperNodeTree tree;
        vector<vector<int>> allMaximalClique;
        allMaximalClique = tree.loadData(filePath);

        tree.execute(allMaximalClique);
        tree.outCutGragh(outFilePath);
    }*/

    int num = 4;
    stringstream ss; string i_s;
    ss << num; ss >> i_s;

    string filePath = "C://Users//Administrator//Desktop//example//HSNT//maximalClique" + i_s + ".txt";
    string outFilePath = "C://Users//Administrator//Desktop//result//HSNT//HSNT" + i_s + ".txt";

    HierarchicalSuperNodeTree tree;
    vector<vector<int>> allMaximalClique;
    allMaximalClique = tree.loadData(filePath);

    tree.execute(allMaximalClique);
    tree.outCutGragh(outFilePath);
}