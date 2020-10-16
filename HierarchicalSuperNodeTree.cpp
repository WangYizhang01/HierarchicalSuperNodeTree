#include "HierarchicalSuperNodeTree.h"

void HierarchicalSuperNodeTree::execute(vector<vector<int>> allMaximalClique){
    for(int num=0;num<allMaximalClique.size();num++){ // 遍历所有的极大团
        vector<int> mc = allMaximalClique[num];
        insertClique(mc); // 插入极大团mc
         printParameters();
    }
}

void HierarchicalSuperNodeTree::insertClique(vector<int> maximalClique){
    // 插入极大团
    set<int> newVertexSet, oldVertexSet; // 新插入极大团中的新节点，公共节点
    map<int, int> incrementalDegreeMap = graphGrow(maximalClique, newVertexSet, oldVertexSet);
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
            HSNode *node = vertex_node_index[vertex];
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
        } else {
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
    if(ancestorMap.size()==0){
        // 没有公共节点
        if(!newVertexSet.empty()){
            HSNode *newTreeNode = new HSNode();
            newTreeNode->degree = newVertexNodeDegree;
            newTreeNode->hierarchical = 0;
            newTreeNode->vertexSet = newVertexSet;
            for(set<int>::iterator nvsit=newVertexSet.begin();nvsit!=newVertexSet.end();nvsit++){
                vertex_node_index[*nvsit] = newTreeNode;
            }
        }else{
            cout << "[ERROR 2] : newVertexSet is empty." << endl;
        }
    }else if(ancestorMap.size()==1){
        // 公共节点在一个连通图中
        // 注意：存在公共节点度数增长为0的情况，比如{{1，2，3，4}，{3，4，5，6}，{1，3，4，6}}中节点3，4
        map<HSNode*, map<HSNode*, map<int, set<int>>>>::iterator amit = ancestorMap.begin();
        map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
        HSNode* updateNode = batchUpdateTreeNode(treeNodeMap,newVertexSet,newVertexNodeDegree);
    }else{
        // 公共节点在多个连通图中
        HSNode *newNode = new HSNode();
        newNode->degree = maximalClique.size() - 1;
        if(!newVertexSet.empty()){
            newNode->vertexSet = newVertexSet;
        }
        for (set<int>::iterator nit = newVertexSet.begin(); nit != newVertexSet.end(); nit++) {
            vertex_node_index[*nit] = newNode;
        }

        // 取出第一棵树作为parent方向
        map<HSNode*, map<HSNode*, map<int, set<int>>>>::iterator amit = ancestorMap.begin();
        map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
        HSNode* parentNode = batchUpdateTreeNode(treeNodeMap,newVertexSet,newVertexNodeDegree);
        newNode->parent = parentNode; newNode->hierarchical = parentNode->hierarchical + 1; 
        parentNode->children.insert(make_pair(newNode, 0)); /// 注意：势

        // 遍历剩下的树，依次作为child方向重构hierarchical
        for (amit++; amit != ancestorMap.end(); amit++) {
            map<HSNode*, map<int, set<int>>> treeNodeMap = (*amit).second;
            HSNode* curNode = batchUpdateTreeNode(treeNodeMap,newVertexSet,newVertexNodeDegree);
            HSNode* pa = curNode->parent; curNode->parent = newNode; newNode->children.insert(make_pair(curNode, 0)); /// 注意：势
            curNode->hierarchical = newNode->hierarchical + 1;
            while (pa != NULL) {
                HSNode* tmp = pa->parent;
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

//HSNode* HierarchicalSuperNodeTree::batchUpdateTreeNode(map<HSNode*, map<int, set<int>>> treeNodeMap, set<int> newVertexSet) {
//    // 对同一个连通图中的节点进行更新，分为 公共节点在同一个结点中 以及 在多个结点中 两种情况
//    // 公共节点在同一个结点中
//    if (treeNodeMap.size() == 1) {
//        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();
//        HSNode* node = (*tnmit).first; map<int, set<int>> updateMap = (*tnmit).second;
//        map<int, set<int>>::iterator updateit = updateMap.begin();
//        int newDegree = node->degree + (*updateit).first; int flag0 = 0;
//
//        if (node->parent != nullptr && node->parent->degree > node->degree) {
//            flag0 = 1;
//
//            // 若新结点的度数大于node的父节点的度数
//            if (newDegree > node->parent->degree) {
//                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
//                newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, 0)); // 注意：势的大小
//                newNode->hierarchical = node->parent->hierarchical + 1;
//
//                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
//                    node->vertexSet.erase(*nnit);
//                    vertex_node_index[*nnit] = newNode;
//                }
//
//                return newNode;
//            }
//            else if (newDegree < node->parent->degree) { // 若新结点的度数小于node的父节点的度数
//                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
//                newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, 0)); node->parent->children.erase(node);
//                newNode->hierarchical = node->hierarchical;
//                node->parent = newNode; newNode->children.insert(make_pair(node, 0));
//                setHierarchical(node, newNode->hierarchical + 1);
//
//                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
//                    node->vertexSet.erase(*nnit);
//                    vertex_node_index[*nnit] = newNode;
//                }
//
//                return newNode;
//            }
//            else { // 若新结点的度数等于node的父节点的度数
//                node->parent->vertexSet.insert((*updateit).second.begin(), (*updateit).second.end());
//
//                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                for (set<int>::iterator nnit = (*updateit).second.begin(); nnit != (*updateit).second.end(); nnit++) {
//                    node->vertexSet.erase(*nnit);
//                    vertex_node_index[*nnit] = node->parent;
//                }
//
//                return node->parent;
//            }
//        }
//        else if (node->children.size() > 0) {
//            for (map<HSNode*, int>::iterator childit = node->children.begin(); childit != node->children.end(); childit++) {
//                if ((*childit).second == 0) {
//                    continue;
//                }
//                else if ((*childit).second < 10000) {
//                    flag0 = 1;
//                    if ((*childit).second > (*updateit).first) {
//                        HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
//                        newNode->hierarchical = node->hierarchical + 1;
//                        newNode->parent = node; node->children[newNode] = (*updateit).first;
//                        newNode->children[(*childit).first] = (*childit).second - (*updateit).first;
//                        (*childit).first->parent = newNode;
//                        setHierarchical((*childit).first, newNode->hierarchical + 1);
//                        node->children.erase(childit);
//
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
//                            node->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = newNode;
//                        }
//
//                        return newNode;
//                    }
//                    else if ((*childit).second < (*updateit).first) {
//                        HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
//                        newNode->hierarchical = (*childit).first->hierarchical + 1;
//                        newNode->parent = (*childit).first; (*childit).first->children[newNode] = (*updateit).first - (*childit).second;
//
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
//                            node->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = newNode;
//                        }
//
//                        return newNode;
//                    }
//                    else {
//                        (*childit).first->vertexSet.insert((*updateit).second.begin(), (*updateit).second.end()); // 注意：结点是否合并
//
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = (*updateit).second.begin(); nnit != (*updateit).second.end(); nnit++) {
//                            node->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = node->parent;
//                        }
//
//                        return (*childit).first;
//                    }
//                }
//            }
//        }
//        if (flag0 == 0) {
//            HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
//            // 从node中移除度数增加的节点,将其指向新结点
//            for (set<int>::iterator sit = (*updateit).second.begin(); sit != (*updateit).second.end(); sit++) {
//                node->vertexSet.erase(*sit);
//                vertex_node_index[*sit] = newNode;
//            }
//            newNode->hierarchical = node->hierarchical + 1;
//            newNode->parent = node; node->children[newNode] = (*updateit).first;
//            return newNode;
//        }
//    }
//    else if (treeNodeMap.size() > 1) {
//        // 公共节点在多个结点中
//        map<HSNode*, int> road; // 存储结点间的路径
//        set<HSNode*> newUpdateNode;
//        // 将公共节点所在的结点间的路径存入road中
//        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();
//        for (tnmit++; tnmit != treeNodeMap.end(); tnmit++) {
//            HSNode* node1 = (*treeNodeMap.begin()).first; road[node1] = 1;
//            HSNode* node2 = (*tnmit).first; road[node2] = 1;
//            while (node1->hierarchical > 0 && node2->hierarchical > 0) {
//                if (node1 == node2) {
//                    break;
//                }
//                else if (node1->hierarchical <= node2->hierarchical) {
//                    node2 = node2->parent; road[node2] = 1;
//                }
//                else if (node1->hierarchical > node2->hierarchical) {
//                    node1 = node1->parent; road[node1] = 1;
//                }
//            }
//            if (node2->hierarchical == 0) {
//                while (node1->hierarchical > 0) {
//                    node1 = node1->parent; road[node1] = 1;
//                }
//            }
//            if (node1->hierarchical == 0) {
//                while (node2->hierarchical > 0) {
//                    node2 = node2->parent; road[node2] = 1;
//                }
//            }
//        }
//
//        // 遍历路径，将峰结点下降
//        for (map<HSNode*, int>::iterator roadit = road.begin(); roadit != road.end(); roadit++) {
//            int descendantDegree = treeNodeMap.begin()->second.begin()->first; // 峰结点需要下降的度数
//
//            HSNode* curNode = (*roadit).first;
//            if (road.find(curNode->parent) != road.end() && curNode->parent->degree < curNode->degree) {
//                for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
//                    if (road.find((*cit).first) != road.end() && (*cit).first->degree < curNode->degree) {
//                        // 峰结点
//                        HSNode* newNode = new HSNode;
//                        newNode->degree = curNode->degree + descendantDegree;
//                        newNode->children = curNode->children; curNode->children.clear(); // 注意
//                        newNode->parent = curNode->parent; curNode->parent = newNode;
//                        // 更新层次信息
//                        // 注意：势的关系的继承
//                        newNode->hierarchical = curNode->hierarchical; curNode->hierarchical = newNode->hierarchical + 1;
//                    }
//                }
//            }
//        }
//
//        // 更新公共节点所在的结点
//        for (map<HSNode*, map<int, set<int>>>::iterator tit = treeNodeMap.begin(); tit != treeNodeMap.end(); tit++) {
//            HSNode* endNode = (*tit).first; map<int, set<int>> updateEndNodeMap = (*tit).second;
//            int flag = 0; // 标记结点更新是否成功
//            int newDegree = endNode->degree + updateEndNodeMap.begin()->first; // 新结点的度数
//
//            // 若端结点endNode的父节点存在且在路径上
//            if (endNode->parent != nullptr && road.find(endNode->parent) != road.end()) {
//                // 若端结点endNode的父节点的度数比endNode大
//                if (endNode->parent->degree < endNode->degree) {
//                    flag = 1;
//
//                    // 若新结点的度数大于endNode的父节点的度数
//                    if (newDegree > endNode->parent->degree) {
//                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
//                        newEndNode->parent = endNode->parent; endNode->parent->children.insert(make_pair(newEndNode, 0));
//                        newEndNode->hierarchical = endNode->parent->hierarchical + 1;
//
//                        // 将新结点加入newUpdateNode中
//                        newUpdateNode.insert(newEndNode);
//
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
//                            endNode->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = newEndNode;
//                        }
//                    }
//                    else if (newDegree < endNode->parent->degree) { // 若新结点的度数小于endNode的父节点的度数
//                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
//                        newEndNode->parent = endNode->parent; endNode->parent->children.insert(make_pair(newEndNode, 0)); endNode->parent->children.erase(endNode);
//                        newEndNode->hierarchical = endNode->hierarchical;
//                        endNode->parent = newEndNode; newEndNode->children.insert(make_pair(endNode, 0));
//                        setHierarchical(endNode, newEndNode->hierarchical + 1);
//                        // 将新结点加入newUpdateNode中
//                        newUpdateNode.insert(newEndNode);
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
//                            endNode->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = newEndNode;
//                        }
//                    }
//                    else { // 若新结点的度数等于endNode的父节点的度数
//                        endNode->parent->vertexSet.insert(updateEndNodeMap.begin()->second.begin(), updateEndNodeMap.begin()->second.end());
//                        // 将新结点加入newUpdateNode中
//                        newUpdateNode.insert(endNode->parent);
//
//                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
//                        for (set<int>::iterator nnit = updateEndNodeMap.begin()->second.begin(); nnit != updateEndNodeMap.begin()->second.end(); nnit++) {
//                            endNode->vertexSet.erase(*nnit);
//                            vertex_node_index[*nnit] = endNode->parent;
//                        }
//                    }
//                }
//            }
//            else { // 若端结点endNode的父节点不存在，或者不在路径上
//                // 若endNode存在子结点
//                if (endNode->children.size() > 0) {
//                    for (map<HSNode*, int>::iterator chit = endNode->children.begin(); chit != endNode->children.end(); chit++) {
//                        HSNode* childNode = (*chit).first;
//                        // 若该子结点在路径上
//                        if (road.find(childNode) != road.end()) {
//                            if (childNode->degree > endNode->degree) {
//                                flag = 1;
//                                if (newDegree > childNode->degree) {
//                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
//                                    newEndNode->parent = childNode; childNode->children.insert(make_pair(newEndNode, 0));
//                                    newEndNode->hierarchical = childNode->hierarchical + 1;
//                                    // 将新结点加入newUpdateNode中
//                                    newUpdateNode.insert(newEndNode);
//                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
//                                    for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
//                                        endNode->vertexSet.erase(*nnit);
//                                        vertex_node_index[*nnit] = newEndNode;
//                                    }
//                                }
//                                else if (newDegree < childNode->degree) {
//                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
//                                    newEndNode->parent = endNode; endNode->children.insert(make_pair(newEndNode, 0)); endNode->children.erase(childNode);
//                                    newEndNode->hierarchical = endNode->hierarchical + 1;
//                                    setHierarchical(childNode, newEndNode->hierarchical + 1);
//                                    // 将新结点加入newUpdateNode中
//                                    newUpdateNode.insert(newEndNode);
//
//                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
//                                    for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
//                                        endNode->vertexSet.erase(*nnit);
//                                        vertex_node_index[*nnit] = newEndNode;
//                                    }
//                                }
//                                else {
//                                    childNode->vertexSet.insert(updateEndNodeMap.begin()->second.begin(), updateEndNodeMap.begin()->second.end());
//                                    // 将新结点加入newUpdateNode中
//                                    newUpdateNode.insert(childNode);
//                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
//                                    for (set<int>::iterator nnit = updateEndNodeMap.begin()->second.begin(); nnit != updateEndNodeMap.begin()->second.end(); nnit++) {
//                                        endNode->vertexSet.erase(*nnit);
//                                        vertex_node_index[*nnit] = childNode;
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            // 若结点未更新成功，即endNode的父(子)结点不在road中，或者在road中但是度数均比endNode低
//            if (flag == 0) {
//                HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
//                newEndNode->parent = endNode; endNode->children.insert(make_pair(newEndNode, 0));
//                newEndNode->hierarchical = endNode->hierarchical + 1;
//                // 将新结点加入newUpdateNode中
//                newUpdateNode.insert(newEndNode);
//
//                // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
//                for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
//                    endNode->vertexSet.erase(*nnit);
//                    vertex_node_index[*nnit] = newEndNode;
//                }
//            }
//        }
//        return *(newUpdateNode.begin());
//    }
//}

HSNode* HierarchicalSuperNodeTree::batchUpdateTreeNode(map<HSNode*, map<int, set<int>>> treeNodeMap, set<int> newVertexSet, int newVertexNodeDegree) {
    // 对同一个连通图中的节点进行更新，分为 公共节点在同一个结点中 以及 在多个结点中 两种情况
    // 公共节点在同一个结点中
    // 注意：map<int, set<int>> 中set的size为1的情况需要单独考虑
    if (treeNodeMap.size() == 1) {
        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();
        HSNode* node = (*tnmit).first; map<int, set<int>> updateMap = (*tnmit).second;
        map<int, set<int>>::iterator updateit = updateMap.begin();
        int newDegree = node->degree + (*updateit).first; int flag0 = 0;

        // 若需更新的公共节点个数为1，则新创建结点与当前结点相连
        if ((*updateit).second.size() == 1) {
            flag0 = 1;

            HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
            newNode->parent = node; newNode->hierarchical = node->hierarchical + 1;
            node->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newNode->vertexSet)));
            // 将newNode中的节点与newNode对应起来存到vertex_node_index中
            for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                node->vertexSet.erase(*nnit);
                vertex_node_index[*nnit] = newNode;
            }

            if (!newVertexSet.empty()) {//connect
                HSNode* newTreeNode = new HSNode();
                newTreeNode->degree = newVertexNodeDegree;
                newTreeNode->hierarchical = newNode->hierarchical + 1;
                newTreeNode->parent = newNode;
                newTreeNode->vertexSet = newVertexSet;
                for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                    vertex_node_index[*nvsit] = newTreeNode;
                }
                newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node,updateMap,newVertexSet,1))); // 注意：势的确定
            }

            return newNode;
        }
        // 若需更新的公共节点个数大于等于2
        // 若父结点存在且度数大于当前结点
        else if (node->parent != nullptr && node->parent->degree > node->degree) {
            // 根据该边的势的大小进行分类处理
            // 情况1：势为0
            int potential = node->parent->children[node];
            if (potential == 0) {
                flag0 = 1;

                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                newNode->parent = node; newNode->hierarchical = node->hierarchical + 1;
                node->children.insert(make_pair(newNode, getPotentialNum(node, updateMap, newNode->vertexSet)));
                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                    node->vertexSet.erase(*nnit);
                    vertex_node_index[*nnit] = newNode;
                }

                if (!newVertexSet.empty()) {//connect
                    HSNode* newTreeNode = new HSNode();
                    newTreeNode->degree = newVertexNodeDegree;
                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                    newTreeNode->parent = newNode;
                    newTreeNode->vertexSet = newVertexSet;
                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                        vertex_node_index[*nvsit] = newTreeNode;
                    }
                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                }
                return newNode;
            }
            // 情况2：势大于0
            else if (potential > 0) {
                int degreeGap = node->parent->degree - node->degree;
                // 根据势与degreeGap的大小关系分类讨论
                // 分为 势=degreeGap 与 势<degreeGap 两种情况
                // potential = degreeGap的情况
                if (potential == degreeGap) {
                    flag0 = 1;

                    // 若新结点的度数大于node的父节点的度数
                    if (newDegree > node->parent->degree) {
                        HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                        newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, 0)); /// 注意：势的大小
                        newNode->hierarchical = node->parent->hierarchical + 1;

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        if (!newVertexSet.empty()) {//connect
                            HSNode* newTreeNode = new HSNode();
                            newTreeNode->degree = newVertexNodeDegree;
                            newTreeNode->hierarchical = newNode->hierarchical + 1;
                            newTreeNode->parent = newNode;
                            newTreeNode->vertexSet = newVertexSet;
                            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                vertex_node_index[*nvsit] = newTreeNode;
                            }
                            newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                        }

                        return newNode;
                    }
                    else if (newDegree < node->parent->degree) { // 若新结点的度数小于node的父节点的度数
                        HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                        newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, node->parent->degree - newNode->degree)); node->parent->children.erase(node);
                        newNode->hierarchical = node->hierarchical;
                        node->parent = newNode; newNode->children.insert(make_pair(node, newNode->degree-node->degree));
                        setHierarchical(node, newNode->hierarchical + 1);

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        if (!newVertexSet.empty()) {//connect
                            HSNode* newTreeNode = new HSNode();
                            newTreeNode->degree = newVertexNodeDegree;
                            newTreeNode->hierarchical = newNode->hierarchical + 1;
                            newTreeNode->parent = newNode;
                            newTreeNode->vertexSet = newVertexSet;
                            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                vertex_node_index[*nvsit] = newTreeNode;
                            }
                            newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                        }

                        return newNode;
                    }
                    else { // 若新结点的度数等于node的父节点的度数
                        HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                        newNode->parent = node->parent; node->parent->children.insert(make_pair(newNode, 0)); newNode->hierarchical = node->parent->hierarchical + 1;

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = (*updateit).second.begin(); nnit != (*updateit).second.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = node->parent;
                        }

                        if (!newVertexSet.empty()) {//connect
                            HSNode* newTreeNode = new HSNode();
                            newTreeNode->degree = newVertexNodeDegree;
                            newTreeNode->hierarchical = newNode->hierarchical + 1;
                            newTreeNode->parent = newNode;
                            newTreeNode->vertexSet = newVertexSet;
                            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                vertex_node_index[*nvsit] = newTreeNode;
                            }
                            newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                        }

                        return node->parent;
                    }
                }
                // potential < degreeGap的情况
                else if (potential < degreeGap) {
                    // 分为增加的度数 <= potential 与 > potential两种情况
                    // 情况1：增加的度数 <= potential
                    if ((*updateit).first <= potential) {
                        HSNode* newNode = new HSNode; newNode->parent = node->parent; 
                        node->parent->children.insert(make_pair(newNode, potential - (*updateit).first)); node->parent->children.erase(node);
                        newNode->degree = newDegree; newNode->vertexSet = (*updateit).second; newNode->hierarchical = node->hierarchical;
                        node->parent = newNode; newNode->children.insert(make_pair(node, (*updateit).first));
                        setHierarchical(node, newNode->hierarchical + 1);
                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        if (!newVertexSet.empty()) {//connect
                            HSNode* newTreeNode = new HSNode();
                            newTreeNode->degree = newVertexNodeDegree;
                            newTreeNode->hierarchical = newNode->hierarchical + 1;
                            newTreeNode->parent = newNode;
                            newTreeNode->vertexSet = newVertexSet;
                            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                vertex_node_index[*nvsit] = newTreeNode;
                            }
                            newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                        }
                        return newNode;
                    }
                    // 情况2：增加的度数 > potential
                    else if ((*updateit).first > potential) {
                        HSNode* nullNode = new HSNode; nullNode->parent = node->parent; 
                        node->parent->children.insert(make_pair(nullNode, 0)); node->parent->children.erase(node);
                        nullNode->degree = newDegree; nullNode->vertexSet = {}; nullNode->hierarchical = node->hierarchical;
                        node->parent = nullNode; nullNode->children.insert(make_pair(node, potential));
                        setHierarchical(node, nullNode->hierarchical + 1);

                        HSNode* newNode = new HSNode; newNode->parent = nullNode; nullNode->children.insert(make_pair(newNode, 0));
                        newNode->vertexSet = (*updateit).second; newNode->hierarchical = nullNode->hierarchical + 1;

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                            node->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newNode;
                        }

                        if (!newVertexSet.empty()) {//connect
                            HSNode* newTreeNode = new HSNode();
                            newTreeNode->degree = newVertexNodeDegree;
                            newTreeNode->hierarchical = newNode->hierarchical + 1;
                            newTreeNode->parent = newNode;
                            newTreeNode->vertexSet = newVertexSet;
                            for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                vertex_node_index[*nvsit] = newTreeNode;
                            }
                            newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                        }
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
                                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                                newNode->parent = childNode; childNode->children.insert(make_pair(newNode, 0)); /// 注意：势的大小
                                newNode->hierarchical = childNode->hierarchical + 1;

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                if (!newVertexSet.empty()) {//connect
                                    HSNode* newTreeNode = new HSNode();
                                    newTreeNode->degree = newVertexNodeDegree;
                                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                                    newTreeNode->parent = newNode;
                                    newTreeNode->vertexSet = newVertexSet;
                                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                        vertex_node_index[*nvsit] = newTreeNode;
                                    }
                                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                                }

                                return newNode;
                            }
                            // 若新结点的度数小于node的子节点的度数
                            else if (newDegree < childNode->degree) { 
                                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                                newNode->parent = node; node->children.insert(make_pair(newNode, (*updateit).first)); node->children.erase(childNode);
                                newNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = newNode; newNode->children.insert(make_pair(childNode, potential - (*updateit).first));
                                setHierarchical(childNode, newNode->hierarchical + 1);

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                if (!newVertexSet.empty()) {//connect
                                    HSNode* newTreeNode = new HSNode();
                                    newTreeNode->degree = newVertexNodeDegree;
                                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                                    newTreeNode->parent = newNode;
                                    newTreeNode->vertexSet = newVertexSet;
                                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                        vertex_node_index[*nvsit] = newTreeNode;
                                    }
                                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                                }

                                return newNode;
                            }
                            else { // 若新结点的度数等于node的子节点的度数
                                HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
                                newNode->parent = childNode; childNode->children.insert(make_pair(newNode, 0)); newNode->hierarchical = childNode->hierarchical + 1;

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = (*updateit).second.begin(); nnit != (*updateit).second.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = node->parent;
                                }

                                if (!newVertexSet.empty()) {//connect
                                    HSNode* newTreeNode = new HSNode();
                                    newTreeNode->degree = newVertexNodeDegree;
                                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                                    newTreeNode->parent = newNode;
                                    newTreeNode->vertexSet = newVertexSet;
                                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                        vertex_node_index[*nvsit] = newTreeNode;
                                    }
                                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                                }

                                return node->parent;
                            }
                        }
                        // potential < degreeGap的情况
                        else if (potential < degreeGap) {
                            // 分为增加的度数 <= potential 与 > potential两种情况
                            // 情况1：增加的度数 <= potential
                            if ((*updateit).first <= potential) {
                                HSNode* newNode = new HSNode; newNode->parent = node; node->children.insert(make_pair(newNode, (*updateit).first));
                                newNode->degree = newDegree; newNode->vertexSet = (*updateit).second; newNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = newNode; newNode->children.insert(make_pair(node, potential - (*updateit).first)); node->children.erase(childNode);
                                setHierarchical(childNode, newNode->hierarchical + 1);
                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                if (!newVertexSet.empty()) {//connect
                                    HSNode* newTreeNode = new HSNode();
                                    newTreeNode->degree = newVertexNodeDegree;
                                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                                    newTreeNode->parent = newNode;
                                    newTreeNode->vertexSet = newVertexSet;
                                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                        vertex_node_index[*nvsit] = newTreeNode;
                                    }
                                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                                }
                                return newNode;
                            }
                            // 情况2：增加的度数 > potential
                            else if ((*updateit).first > potential) {
                                HSNode* nullNode = new HSNode; nullNode->parent = node; 
                                node->children.insert(make_pair(nullNode, potential)); node->children.erase(childNode);
                                nullNode->degree = newDegree; nullNode->vertexSet = {}; nullNode->hierarchical = node->hierarchical + 1;
                                childNode->parent = nullNode; nullNode->children.insert(make_pair(childNode, 0));
                                setHierarchical(childNode, nullNode->hierarchical + 1);

                                HSNode* newNode = new HSNode; newNode->parent = nullNode; nullNode->children.insert(make_pair(newNode, 0));
                                newNode->vertexSet = (*updateit).second; newNode->hierarchical = nullNode->hierarchical + 1;

                                // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                                for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                                    node->vertexSet.erase(*nnit);
                                    vertex_node_index[*nnit] = newNode;
                                }

                                if (!newVertexSet.empty()) {//connect
                                    HSNode* newTreeNode = new HSNode();
                                    newTreeNode->degree = newVertexNodeDegree;
                                    newTreeNode->hierarchical = newNode->hierarchical + 1;
                                    newTreeNode->parent = newNode;
                                    newTreeNode->vertexSet = newVertexSet;
                                    for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                                        vertex_node_index[*nvsit] = newTreeNode;
                                    }
                                    newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
                                }
                                return newNode;
                            }
                        }
                    }
                } 
            }
        }
        if (flag0 == 0) {
            HSNode* newNode = new HSNode; newNode->degree = newDegree; newNode->vertexSet = (*updateit).second;
            // 从node中移除度数增加的节点,将其指向新结点
            for (set<int>::iterator sit = (*updateit).second.begin(); sit != (*updateit).second.end(); sit++) {
                node->vertexSet.erase(*sit);
                vertex_node_index[*sit] = newNode;
            }
            newNode->hierarchical = node->hierarchical + 1;
            newNode->parent = node; node->children[newNode] = getPotentialNum(node, updateMap, newNode->vertexSet);
            // 将newNode中的节点与newNode对应起来存到vertex_node_index中
            for (set<int>::iterator nnit = newNode->vertexSet.begin(); nnit != newNode->vertexSet.end(); nnit++) {
                node->vertexSet.erase(*nnit);
                vertex_node_index[*nnit] = newNode;
            }

            if (!newVertexSet.empty()) {//connect
                HSNode* newTreeNode = new HSNode();
                newTreeNode->degree = newVertexNodeDegree;
                newTreeNode->hierarchical = newNode->hierarchical + 1;
                newTreeNode->parent = newNode;
                newTreeNode->vertexSet = newVertexSet;
                for (set<int>::iterator nvsit = newVertexSet.begin(); nvsit != newVertexSet.end(); nvsit++) {
                    vertex_node_index[*nvsit] = newTreeNode;
                }
                newNode->children.insert(make_pair(newTreeNode, getPotentialNum(node, updateMap, newVertexSet, 1))); // 注意：势的确定
            }

            return newNode;
        }
    }
    else if (treeNodeMap.size() > 1) {
        // 公共节点在多个结点中
        // 注意：存在 map<int, set<int>> 中 int 值为0的情况，或者 set 的size为1的情况
        map<HSNode*, int> road; // 存储结点间的路径
        set<HSNode*> newUpdateNode;
        // 将公共节点所在的结点间的路径存入road中
        map<HSNode*, map<int, set<int>>>::iterator tnmit = treeNodeMap.begin();
        for (tnmit++; tnmit != treeNodeMap.end(); tnmit++) {
            HSNode* node1 = (*treeNodeMap.begin()).first; road[node1] = 1;
            HSNode* node2 = (*tnmit).first; road[node2] = 1;
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
        }

        // 遍历路径，将峰结点下降
        for (map<HSNode*, int>::iterator roadit = road.begin(); roadit != road.end(); roadit++) {
            int descendantDegree = treeNodeMap.begin()->second.begin()->first; // 峰结点需要下降的度数

            HSNode* curNode = (*roadit).first;
            if (road.find(curNode->parent) != road.end() && curNode->parent->degree < curNode->degree) {
                for (map<HSNode*, int>::iterator cit = curNode->children.begin(); cit != curNode->children.end(); cit++) {
                    if (road.find((*cit).first) != road.end() && (*cit).first->degree < curNode->degree) {
                        // 峰结点
                        HSNode* newNode = new HSNode;
                        newNode->degree = curNode->degree + descendantDegree;
                        newNode->children = curNode->children; curNode->children.clear(); // 注意
                        newNode->parent = curNode->parent; curNode->parent = newNode;
                        // 更新层次信息
                        // 注意：势的关系的继承
                        newNode->hierarchical = curNode->hierarchical; curNode->hierarchical = newNode->hierarchical + 1;
                    }
                }
            }
        }

        ///////// need to updated
        // 更新公共节点所在的结点
        for (map<HSNode*, map<int, set<int>>>::iterator tit = treeNodeMap.begin(); tit != treeNodeMap.end(); tit++) {
            HSNode* endNode = (*tit).first; map<int, set<int>> updateEndNodeMap = (*tit).second;
            int flag = 0; // 标记结点更新是否成功
            int newDegree = endNode->degree + updateEndNodeMap.begin()->first; // 新结点的度数

            // 若端结点endNode的父节点存在且在路径上
            if (endNode->parent != nullptr && road.find(endNode->parent) != road.end()) {
                // 若端结点endNode的父节点的度数比endNode大
                if (endNode->parent->degree < endNode->degree) {
                    flag = 1;

                    // 若新结点的度数大于endNode的父节点的度数
                    if (newDegree > endNode->parent->degree) {
                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
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
                    else if (newDegree < endNode->parent->degree) { // 若新结点的度数小于endNode的父节点的度数
                        HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                        newEndNode->parent = endNode->parent; endNode->parent->children.insert(make_pair(newEndNode, 0)); endNode->parent->children.erase(endNode);
                        newEndNode->hierarchical = endNode->hierarchical;
                        endNode->parent = newEndNode; newEndNode->children.insert(make_pair(endNode, 0));
                        setHierarchical(endNode, newEndNode->hierarchical + 1);
                        // 将新结点加入newUpdateNode中
                        newUpdateNode.insert(newEndNode);
                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = newEndNode->vertexSet.begin(); nnit != newEndNode->vertexSet.end(); nnit++) {
                            endNode->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = newEndNode;
                        }
                    }
                    else { // 若新结点的度数等于endNode的父节点的度数
                        endNode->parent->vertexSet.insert(updateEndNodeMap.begin()->second.begin(), updateEndNodeMap.begin()->second.end());
                        // 将新结点加入newUpdateNode中
                        newUpdateNode.insert(endNode->parent);

                        // 将newNode中的节点与newNode对应起来存到vertex_node_index中
                        for (set<int>::iterator nnit = updateEndNodeMap.begin()->second.begin(); nnit != updateEndNodeMap.begin()->second.end(); nnit++) {
                            endNode->vertexSet.erase(*nnit);
                            vertex_node_index[*nnit] = endNode->parent;
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
                        if (road.find(childNode) != road.end()) {
                            if (childNode->degree > endNode->degree) {
                                flag = 1;
                                if (newDegree > childNode->degree) {
                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
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
                                else if (newDegree < childNode->degree) {
                                    HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
                                    newEndNode->parent = endNode; endNode->children.insert(make_pair(newEndNode, 0)); endNode->children.erase(childNode);
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
                                else {
                                    childNode->vertexSet.insert(updateEndNodeMap.begin()->second.begin(), updateEndNodeMap.begin()->second.end());
                                    // 将新结点加入newUpdateNode中
                                    newUpdateNode.insert(childNode);
                                    // 将newEndNode中的节点与newEndNode对应起来存到vertex_node_index中
                                    for (set<int>::iterator nnit = updateEndNodeMap.begin()->second.begin(); nnit != updateEndNodeMap.begin()->second.end(); nnit++) {
                                        endNode->vertexSet.erase(*nnit);
                                        vertex_node_index[*nnit] = childNode;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // 若结点未更新成功，即endNode的父(子)结点不在road中，或者在road中但是度数均比endNode低
            if (flag == 0) {
                HSNode* newEndNode = new HSNode; newEndNode->degree = newDegree; newEndNode->vertexSet = updateEndNodeMap.begin()->second;
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
        return *(newUpdateNode.begin());
    }
}

void HierarchicalSuperNodeTree::setHierarchical(HSNode *updateNode, int hierarchical){
    updateNode->hierarchical = hierarchical;
    for(map<HSNode*, int>::iterator ucit=updateNode->children.begin();ucit!=updateNode->children.end();ucit++){
        HSNode* child = (*ucit).first;
        setHierarchical(child,hierarchical+1);
    }
}

int HierarchicalSuperNodeTree::getPotentialNum(HSNode* node, map<int, set<int>> updateNodeMap, set<int> newVertexSet, int flag=0){
    // 计算势的大小，其中flag=0表示计算的是node与公共节点的势，flag=1表示计算的是公共节点与新节点的势
    int nodeVertexNum = node->vertexSet.size();
    int degree = node->degree;
    int updateVertexNum = updateNodeMap.begin()->second.size();
    int newVertexNum = newVertexSet.size();
    if (flag == 0) {
        return min((nodeVertexNum - updateVertexNum) * updateVertexNum - degree, newVertexNum);
    }
    else if (flag == 1) {
        return min(newVertexNum * updateVertexNum - (updateVertexNum + newVertexNum - 1), nodeVertexNum - updateVertexNum);
    }
}    

// Update the graph and return the incremental degree of vertices.
map<int,int> HierarchicalSuperNodeTree::graphGrow(vector<int> maximalClique, set<int> &newVertexSet, set<int> &oldVertexSet){
    map<int,int> incrementalDegree;
    for(vector<int>::iterator mcit=maximalClique.begin();mcit!=maximalClique.end();mcit++){
        int vertex = *mcit;
        int degree = 0;
        if(construct_graph.find(vertex)!=construct_graph.end()){
            map<int,int>* neighbor = construct_graph[vertex];
            for(vector<int>::iterator mcit1=maximalClique.begin();mcit1!=maximalClique.end();mcit1++){
                if(neighbor->find(*mcit1)==neighbor->end() && vertex!=*mcit1){
                    degree++;
                    neighbor->insert(make_pair(*mcit1,1));
                }
            }
            oldVertexSet.insert(vertex);
        }else{
            map<int,int>* neighbor = new map<int,int>();
            for(vector<int>::iterator mcit2=maximalClique.begin();mcit2!=maximalClique.end();mcit2++){
                if(vertex!=*mcit2){
                    degree++;
                    neighbor->insert(make_pair(*mcit2,1));
                }
            }
            construct_graph[vertex] = neighbor;
            newVertexSet.insert(vertex);
        }
        incrementalDegree.insert(make_pair(vertex,degree));
    }

    return incrementalDegree;
}

void HierarchicalSuperNodeTree::printParameters(){
    //[1] map<int, map<int,int>*> construct_graph;
    cout<<"[construct_graph]:"<<endl;
    for(map<int, map<int,int>*>::iterator cgit=construct_graph.begin();cgit!=construct_graph.end();cgit++){
        cout<<" "<<(*cgit).first<<" :";
        map<int,int>* neighbor = (*cgit).second;
        for(map<int,int>::iterator nit=neighbor->begin();nit!=neighbor->end();nit++){
            cout<<" "<<(*nit).first;
        }
        cout<<endl;
    }

    cout<<"[vertex_node_index] vertex:hierarchical:"<<endl;
    for(map<int, HSNode *>::iterator vnit=vertex_node_index.begin();vnit!=vertex_node_index.end();vnit++) {
        HSNode * node = (*vnit).second;
        cout<<(*vnit).first<<":"<< node->hierarchical <<endl;
    }

}

int HierarchicalSuperNodeTree::computeMincut(int m, int n) {
    HSNode* Node1 = vertex_node_index[m]; HSNode* Node2 = vertex_node_index[n];
    
    // while (Node1->hierarchical > 0 && Node2->hierarchical > 0) {
    //     if (Node1 == Node2) {
    //         return Node1->degree; 
    //     }else if (Node1->hierarchical <= Node2->hierarchical) { 
    //         if (Node2->parent->hierarchical == Node2->hierarchical-1){
    //             Node2 = Node2->parent; 
    //         }else{
    //             for (map<HSNode*,int>::iterator cit = Node2->children.begin();cit != Node2->children.end();cit++){
    //                 if ((*cit).first->hierarchical == Node2->hierarchical-1){
    //                     Node2 = (*cit).first;
    //                 }
    //             }
    //         } 
    //     }else if (Node1->hierarchical > Node2->hierarchical) {
    //         if (Node1->parent->hierarchical == Node1->hierarchical-1){
    //             Node1 = Node1->parent; 
    //         }else{
    //             for (map<HSNode*,int>::iterator cit = Node1->children.begin();cit != Node1->children.end();cit++){
    //                 if ((*cit).first->hierarchical == Node1->hierarchical-1){
    //                     Node1 = (*cit).first;
    //                 }
    //             }
    //         }
    //     }
    // }
    int minCut = min(Node1->degree, Node2->degree); 
    while (Node1->hierarchical > 0 && Node2->hierarchical > 0) {
        if (Node1 == Node2) {
            return minCut; 
        }else if (Node1->hierarchical <= Node2->hierarchical) { 
            Node2 = Node2->parent; 
            minCut = min(minCut, Node2->degree);
        }else if (Node1->hierarchical > Node2->hierarchical) {
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
        for (int i = 0; i < vertexNumber; i++) {
            for (int j = 0; j < vertexNumber; j++) {
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


int main() {
    //cout << "succeed!" << endl;
    HierarchicalSuperNodeTree tree;
    vector<vector<int>> allMaximalClique;
    vector<int> maximalClique1; vector<int> maximalClique2; vector<int> maximalClique3;
    for (int i = 1; i < 5; i++) {
        maximalClique1.push_back(i);
    }
    maximalClique2.push_back(3); maximalClique2.push_back(4); maximalClique2.push_back(5); maximalClique2.push_back(6); maximalClique3.push_back(6); maximalClique3.push_back(7); maximalClique3.push_back(8);

    allMaximalClique.push_back(maximalClique1); allMaximalClique.push_back(maximalClique2); allMaximalClique.push_back(maximalClique3);
    tree.execute(allMaximalClique);
}