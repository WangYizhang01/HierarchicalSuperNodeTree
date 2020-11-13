
/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016年11月09日 21时55分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yuting
 *        Company:  whu.sklse
 *
 * =====================================================================================
 */
#define CLOCKS_PER_SEC ((clock_t)1000)

#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <bitset>
#include <algorithm>
#include <string.h>
#include "BitCandidateMapConstructor.h"
using namespace std;

int main(int argc, char **argv)
{
    cout<<"start!! \n"<<endl;
    list<string> dataList;
    dataList.push_back("D1");
    dataList.push_back("D2");
    string dataPath = "/Users/yuting/workspace/GraphData/"; //Here is the graph data path, modify it according to your environment

    list<int> KList;
    KList.push_back(2);
    for(list<string>::iterator dit = dataList.begin();dit!=dataList.end();dit++){
        string dataName = *dit;
        for(list<int>::iterator kit = KList.begin();kit!=KList.end();kit++){
            int K = *kit;
            cout<<"=====[ dataName="<<dataName<<", K="<<K<<" ]====="<<endl;
            BitCandidateMapConstructor* bitCMC3 = new BitCandidateMapConstructor(K,dataPath, dataName);
            bitCMC3->constructClique();
            cliqueNumber = bitCMC3->recordResult(dataName + "-result-bitCMC.txt");
            delete bitCMC3;
        }
    }

    cout<<"\n end！"<<endl;
}


