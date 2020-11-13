//
//  BitCandidateMapConstructor.h
//  a
//
//  Created by Yuting on 16/12/5.
//  Copyright (c) 2016年 Yuting. All rights reserved.
//

#ifndef __a__BitCandidateMapConstructor__
#define __a__BitCandidateMapConstructor__

#include <map>
#include <set>
#include <string>
#include <vector>
#include <bitset> 
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include "../Data/Tool.h"

const long unsigned int BIT_NUMBER = 155;

using namespace std;

class InvertTreeNode
{
public:
    InvertTreeNode(){};
    map<int,InvertTreeNode*> children;
};

class BitCandidateMapConstructor
{
public:
    BitCandidateMapConstructor(int k,string TEST_FILE_PATH,string TEST_DATA_NAME){
        cout<<"== BIT_NUMBER == "<<BIT_NUMBER<<endl;
        K = k-1;
        FILE_PATH1 = TEST_FILE_PATH;
        DATA_NAME1 = TEST_DATA_NAME;
        loadData(FILE_PATH1+"split/"+DATA_NAME1 + ".txt");
        root = new InvertTreeNode();
    };

    ~BitCandidateMapConstructor(){
        destroy(root);
    };

    void loadData(const string& filePath);

    timeval constructClique();
    
    int recordResult(string suffix);
    
    
private:
    vector< vector<int> > cliqueResult;
    vector<int> degeneracyId;
    map< int,vector<int> > smallNeighborMap;
    map< int,vector<int> > bigNeighborMap;

    InvertTreeNode* root;
    
    string FILE_PATH1;
    string DATA_NAME1;
    int K;
    
    void destroy(InvertTreeNode* itn){
        for(map<int,InvertTreeNode*>::iterator it = itn->children.begin();it!=itn->children.end();it++){
            destroy((*it).second);
        }
        delete itn;
    }
};


#endif /* defined(__a__BitCandidateMapConstructor__) */
