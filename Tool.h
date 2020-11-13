//
//  Tool.h
//  a
//
//  Created by Yuting on 16/11/10.
//  Copyright (c) 2015年 Yuting. All rights reserved.
//

#ifndef a_Tool_h
#define a_Tool_h

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <time.h>
#include <stdlib.h>


const long unsigned int bitNumber = 155;

using namespace std;

struct labelWeight
{
    int myLabel;
    double myWeight;
    bool operator < (const labelWeight &a) const
    {
        if(a.myWeight < myWeight){
            return true;
        }else if(a.myWeight == myWeight){
            return a.myLabel<= myLabel;
        }else{
            return false;
        }
    }
};

struct labelNumber
{
    int myLabel;
    int myNumber;
    bool operator < (const labelNumber &a) const
    {
        return a.myNumber<= myNumber;
        
    }
};

struct vertexDegree
{
    int myId;
    int myDegree;
    bool operator < (const vertexDegree &a) const
    {
        return a.myDegree >= myDegree;
    }

};

struct nodeWeight
{
    string myName;
    int weight;
    bool operator < (const nodeWeight &a) const
    {
        return a.weight <= weight;
    }
};

struct treeIdentification
{
    string name;
    int identification;
   
    bool operator < (const treeIdentification &a) const
    {
        return a.identification >= identification;
    }
};

struct cliqueStruct
{
    int edgeWeight;
    int nodeQuality;
    double sortWeight;
    list<string> cliqueList;
    bool operator < (const cliqueStruct &a) const
    {
        return a.sortWeight <= sortWeight;
    }
};


class Tool{
public:
    static void split(const string& src, const string& separator, vector<string>& dest);
    static void trim(string & str);
    static list< list<int> > cliqueFilter(list< list<int> > beforeList);
    static void countCliqueResult(const string& filePath1);
    static void countCandidateResult(const string& filePath1);
    static void countDegeneracy(const string& filePath1);
    static void compareResult0(const string& filePath1,const string& filePath2);
    static void compareResult1(const string& filePath1,const string& filePath2);
    static bool compareResult2(const string& filePath1,const string& filePath2);
    
    static void weiboDataGeneration(const string& filePath1,const string& fileName1,const string& filePath2);
    
    
    static void pajekDataGeneration1(const string& filePath1,const string& filePath2,const string& title);
    static void pajekDataGeneration2(const string& filePath1,const string& filePath2,const string& title);
    static void networkDataGeneration(const string& filePath1,const string& filePath2,const string& title);
    static void cleanRawData(const string& filePath1,const string& filePath2);
    static void cleanRawDataSpace(const string& filePath1,const string& filePath2);

    
    static void vectorIntersection(const vector<int> v1,bool inverse1,const vector<int> v2,vector<int>& des);
    static void vectorIntersection(const vector<int> v1,const vector<int> v2,vector<int>& des);
    static void vectorUnion(const vector<int> v1,const vector<int> v2,vector<int>& des);
    static void vectorDifference(const vector<int> v1,const vector<int> v2,vector<int>& des);

    static void vectorbingji2( vector<int>& v1, vector<int> v2);
    static void vectorchaji2(vector<int> &v1, vector<int> v2);
    static void setJiaoji(const set<int> s1,const set<int> s2,set<int>& des);
    static bool containSet(const set<int> s1,const set<int> s2);
    static bool containVector(const vector<int> v1,const vector<int> v2);
    static void commonNeighborForPeco4(const set<int>* v1,const set<int>* v2,string& result);
    
};

#endif
