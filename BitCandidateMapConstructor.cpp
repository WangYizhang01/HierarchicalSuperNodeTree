//
//  BitCandidateMapConstructor.cpp
//  a
//
//  Created by Yuting on 16/12/5.
//  Copyright (c) 2016年 Yuting. All rights reserved.
//

#include "BitCandidateMapConstructor.h"

void BitCandidateMapConstructor::loadData(const string& filePath){
    ifstream dataFile(filePath.c_str());
    if(dataFile.is_open()){
        string line;
        while(getline(dataFile,line)){
            vector<string> nodes;
            Tool::trim(line);
            Tool::split(line,"	",nodes);
            if (nodes.size()==2){
                int neighbor = 0;
                int currentVertex = 0;
                stringstream ss;
                ss << nodes[1];
                ss >> currentVertex; //string -> int
                vector<int> smallNeighbor;
                vector<int> bigNeighbor;
                vector<string> vertices;
                Tool::split(nodes[0]," ",vertices);
                for(int i=0;i<vertices.size();i++){
                    stringstream ss1;
                    ss1 << vertices[i];
                    ss1 >> neighbor; //string -> int
                    if(neighbor < currentVertex){
                        smallNeighbor.push_back(neighbor);
                    }else{
                        bigNeighbor.push_back(neighbor);
                    }
                }
                if(smallNeighbor.size()>0){
                    degeneracyId.push_back(currentVertex);
                    vector<int> newSmallNeighbor;
                    for(int i = (int)smallNeighbor.size()-1;i>=0;i--){
                        newSmallNeighbor.push_back(smallNeighbor[i]);
                    }
                    smallNeighborMap.insert(make_pair(currentVertex, newSmallNeighbor));
                }
                if(bigNeighbor.size()>=K){
                    bigNeighborMap.insert(make_pair(currentVertex, bigNeighbor));
                }
            }else{
                cout<<"Exception of data format: "<< line <<endl;
                break;
            }
        }
        dataFile.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
};


timeval BitCandidateMapConstructor::constructClique(){
    map<int,vector<bitset<BIT_NUMBER> >*> cliqueMap;
    for(vector<int>::iterator it = degeneracyId.begin();it!=degeneracyId.end();it++){
        int nodeName =*it;
        vector<int> smallNeighborVector = smallNeighborMap[nodeName];
        
        while(!smallNeighborVector.empty()){
            int firstNode = smallNeighborVector.back();
            smallNeighborVector.pop_back();
            
            if(bigNeighborMap.find(firstNode)==bigNeighborMap.end()){
                //cout<<"@@@@@@@@"<<firstNode<<endl;
                continue;
            }
            
            vector<int> bigNeighborVector = bigNeighborMap[firstNode];
            if(bigNeighborVector.size()==0){
                cout<<"ERROR! @ bigNeighborVector size=0 of "<< firstNode <<endl;
            }else if(bigNeighborVector.size()>BIT_NUMBER){
                cout<<"ERROR! @ bigNeighborVector size of "<< firstNode << " is : "<< bigNeighborVector.size() <<endl;
            }
            bitset<BIT_NUMBER> nodeBitSet;
            int nodeBitSetLocation = 0;
            for(;nodeBitSetLocation<bigNeighborVector.size();nodeBitSetLocation++){
                if(bigNeighborVector[nodeBitSetLocation] == nodeName){
                    nodeBitSet[nodeBitSetLocation] = 1;
                    break;
                }
            }
            map<int,vector<bitset<BIT_NUMBER> >*>::iterator updateIt = cliqueMap.find(firstNode);
            if(updateIt==cliqueMap.end()){
                vector<bitset<BIT_NUMBER> >* currentNewVector = new vector<bitset<BIT_NUMBER> >();
                currentNewVector->push_back(nodeBitSet);
                cliqueMap.insert(make_pair(firstNode, currentNewVector));
            }else{
                vector<bitset<BIT_NUMBER> >* currentVector = cliqueMap[firstNode];
                bitset<BIT_NUMBER> smallNeighborBitSet ;
                int j=0;
                int num = (int)smallNeighborVector.size()-1;
                while(j<bigNeighborVector.size() && num>=0){
                    if(bigNeighborVector[j] == smallNeighborVector[num]){
                        smallNeighborBitSet[j]=1;
                        num--;
                        j++;
                    }else if(bigNeighborVector[j] > smallNeighborVector[num]){
                        num--;
                    }else{
                        j++;
                    }
                }
                if(smallNeighborBitSet.none()){
                    currentVector->push_back(nodeBitSet);
                }else{
                    bool completeMatch = false;
                    vector<bitset<BIT_NUMBER> > fullCliqueVector;
                    vector<bitset<BIT_NUMBER> > partCliqueVector;
                    for(vector<bitset<BIT_NUMBER> >::iterator cliqueVectorIt = currentVector->begin();cliqueVectorIt!=currentVector->end();cliqueVectorIt++){
                        bitset<BIT_NUMBER> clique = *cliqueVectorIt;
                        bitset<BIT_NUMBER> commmonBitSet = clique & smallNeighborBitSet;
                        if(commmonBitSet.none()){
                            continue;
                        }else if(commmonBitSet==smallNeighborBitSet){
                            completeMatch = true;
                            if(commmonBitSet==clique){
                                bitset<BIT_NUMBER>* updateClique = &(*cliqueVectorIt);
                                updateClique->set(nodeBitSetLocation);
                                
                            }else {
                                bitset<BIT_NUMBER> newBitSet = commmonBitSet | nodeBitSet;
                                currentVector->push_back(newBitSet);
                                
                            }
                            break;
                        }else if(commmonBitSet==clique){
                            bitset<BIT_NUMBER>* updateClique = &(*cliqueVectorIt);
                            updateClique->set(nodeBitSetLocation);
                            fullCliqueVector.push_back(commmonBitSet);
                        }else {
                            vector<bitset<BIT_NUMBER> >::iterator cleanIt = partCliqueVector.begin();
                            while(cleanIt!=partCliqueVector.end()){
                                bitset<BIT_NUMBER> currentVector = *cleanIt;
                                if(currentVector.count() >= commmonBitSet.count()){
                                    break;
                                }
                                cleanIt++;
                            }
                            partCliqueVector.insert(cleanIt,commmonBitSet);
                        }
                    }
                    if(!completeMatch && partCliqueVector.size()>0){
                        while(!partCliqueVector.empty()){
                            vector<bitset<BIT_NUMBER> >::iterator partIt0 = partCliqueVector.begin();
                            bitset<BIT_NUMBER> firstPartVector = *partIt0;
                            partCliqueVector.erase(partIt0);
                            bool partContain = false;
                            for(vector<bitset<BIT_NUMBER> >::iterator partIt = partCliqueVector.begin();partIt!=partCliqueVector.end();partIt++){
                                bitset<BIT_NUMBER> partVector =  *partIt;
                                if(partVector == (firstPartVector|partVector)){
                                    partContain = true;
                                    break;
                                }
                            }
                            if(!partContain){
                                bool fullContain = false;
                                for(vector<bitset<BIT_NUMBER> >::iterator fullIt = fullCliqueVector.begin();fullIt!=fullCliqueVector.end();fullIt++){
                                    bitset<BIT_NUMBER> fullVector =  *fullIt;
                                    if(fullVector == (firstPartVector|fullVector)){
                                        fullContain = true;
                                        break;
                                    }
                                }
                                if(!fullContain){
                                    firstPartVector.set(nodeBitSetLocation);
                                    currentVector->push_back(firstPartVector);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    timeval c0;
    gettimeofday(&c0, NULL);
    
    int candidateNumber=0;
    for(map<int,vector<bitset<BIT_NUMBER> >*>::iterator cliqueIt = cliqueMap.begin();cliqueIt!=cliqueMap.end();cliqueIt++){
        int firstNode = (*cliqueIt).first;
        vector<bitset<BIT_NUMBER> >* cliqueVectorOfNode = (*cliqueIt).second;
        candidateNumber+=cliqueVectorOfNode->size();
        vector<int> bigNeighborVector = bigNeighborMap[firstNode];
        while(!cliqueVectorOfNode->empty()){
            bitset<BIT_NUMBER> cliqueBitSet = cliqueVectorOfNode->back();
            if(cliqueBitSet.count()<K){
                //cout<<"firstNode="<<firstNode<<" count()="<<cliqueBitSet.count()<<endl;
                cliqueVectorOfNode->pop_back();
                continue;
            }
            InvertTreeNode* currentNode = root;
            for(int i = (int)bigNeighborVector.size()-1; i>=0 && cliqueBitSet.any();i--){
                if(cliqueBitSet[i]){
                    int currentNodeName =bigNeighborVector[i];
                    cliqueBitSet.reset(i);
                    map<int,InvertTreeNode*>::iterator it = currentNode->children.find(currentNodeName);
                    if(it!=currentNode->children.end()){
                        currentNode = (*it).second;
                    }else{
                        InvertTreeNode* newNode = new InvertTreeNode();
                        currentNode->children.insert(make_pair(currentNodeName, newNode));
                        currentNode = newNode;
                    }
                }
            }
            map<int,InvertTreeNode*>::iterator it = currentNode->children.find(firstNode);
            if(it==currentNode->children.end()){
                InvertTreeNode* newNode = new InvertTreeNode();
                currentNode->children.insert(make_pair(firstNode, newNode));
                vector<int> newClique;
                bitset<BIT_NUMBER> maximalCliqueBitSet = cliqueVectorOfNode->back();
                for(int i = (int)bigNeighborVector.size()-1; i>=0 && maximalCliqueBitSet.any();i--){
                    if(maximalCliqueBitSet[i]){
                        newClique.push_back(bigNeighborVector[i]);
                        maximalCliqueBitSet.reset(i);
                    }
                }
                newClique.push_back(firstNode);
                cliqueResult.push_back(newClique);
            }
            cliqueVectorOfNode->pop_back();
        }
        delete cliqueVectorOfNode;
    }
    cout<<"** candidateNumber="<<candidateNumber<<endl;
    return c0;
};


int BitCandidateMapConstructor::recordResult(string suffix){
    int cliqueNumber = 0;
    ofstream cliqueFile;
    string filePath = FILE_PATH1+"result/"+ suffix;
    cliqueFile.open(filePath.c_str());
    if (cliqueFile.is_open()){
        cliqueNumber = (int)cliqueResult.size();
        for(vector<vector<int> >::iterator it = cliqueResult.begin();it!=cliqueResult.end();it++){
            vector<int> clique = *it;
            bool addBlank = false;
            for(vector<int>::iterator setIt = clique.begin();setIt!=clique.end();setIt++,addBlank=true){
                if(addBlank){
                    cliqueFile<<" ";
                }
                cliqueFile<<*setIt;
            }
            cliqueFile<<"\n";
        }
        cliqueFile.close();
    }
    return cliqueNumber;
};
