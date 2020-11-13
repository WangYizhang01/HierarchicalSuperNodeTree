//
//  Tool.cpp
//  a
//
//  Created by Yuting on 16/11/10.
//  Copyright (c) 2016年 Yuting. All rights reserved.
//

#include "Tool.h"

void Tool::split(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    
    do{
        index = str.find_first_of(separator,start);
        if (index != string::npos)
        {
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == string::npos) return;
        }
    }while(index != string::npos);
    
    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
};

void Tool::trim(string & str )
{
    string::size_type pos0 = str.find_last_not_of('\r');
    if (pos0 != string::npos)
    {
        str = str.substr(0, pos0 + 1);
    }
    
    string::size_type pos = str.find_first_not_of(' ');
    if (pos != string::npos)
    {
        string::size_type pos2 = str.find_last_not_of(' ');
        if (pos2 != string::npos)
        {
            str = str.substr(pos, pos2 - pos + 1);
        }else{
            str = str.substr(pos);
        }
    }
};


list< list<int> > Tool::cliqueFilter(list< list<int> > beforeList){
    list< list<int> > afterList;
    vector< list<int> > sortList;
    list< list<int> >::iterator beforeIt;
    for(beforeIt=beforeList.begin();beforeIt!=beforeList.end();beforeIt++){
        list<int> beforeFirstList = *beforeIt;
        vector< list<int> >::iterator cleanIt = sortList.begin();
        while(cleanIt!=sortList.end()){
            list<int> currentList = *cleanIt;
            if(currentList.size()<=beforeFirstList.size()){
                break;
            }
            cleanIt++;
        }
        sortList.insert(cleanIt,beforeFirstList);
    }
    return afterList;
};

void Tool::cleanRawDataSpace(const string& filePath1,const string& filePath2){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    
    ifstream dataFile(filePath1.c_str());
    if(dataFile.is_open()){
        string line;
        while(getline(dataFile,line)){
            Tool::trim(line);
            if(line.at(0)!='#'){
                vector<string> nodes;
                Tool::split(line," ",nodes);
                if (nodes.size()>=2 && nodes.size()<=3 ){
                    if(nodes[0]==nodes[1]){
                        cout<<"Skip line : "<<line<<endl;
                        continue;
                    }else{
                        newFile<<nodes[0]<<"	"<<nodes[1]<<"\n";
                    }
                }else{
                    cout<<"Exception of data format: "<< line <<endl;
                    break;
                }
            }else{
                newFile<<line<<"\n";
            }
        }
        dataFile.close();
    }else{
        cout<<"!!Open File Error!!"<<endl;
    }
    
    newFile.close();

}

void Tool::cleanRawData(const string& filePath1,const string& filePath2){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    
    ifstream dataFile(filePath1.c_str());
    if(dataFile.is_open()){
        string line;
        while(getline(dataFile,line)){
            Tool::trim(line);
            if(line.at(0)!='#'){
                vector<string> nodes;
                Tool::split(line," ",nodes);
                if (nodes.size()>=2 && nodes.size()<=3 ){
                    if(nodes[0]==nodes[1]){
                        cout<<"Skip line : "<<line<<endl;
                        continue;
                    }else{
                        newFile<<nodes[0]<<"	"<<nodes[1]<<"\n";
                        //newFile<<line<<"\n";
                    }
                }else{
                    cout<<"Exception of data format: "<< line <<endl;
                    break;
                }
            }else{
                newFile<<line<<"\n";
            }
        }
        dataFile.close();
    }else{
        cout<<"!!Open File Error!!"<<endl;
    }
   
    newFile.close();
};

void Tool::countCliqueResult(const string& filePath1){
    int cliqueNumber = 0;
    int totalCliqueSize = 0;
    int MaxCliqueSize = 0;
    set<string> cliqueSet1;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            Tool::split(line1," ",nodes);
            if(nodes.size()<2){
                cout<<"Clique Format Error: "<<line1<<endl;
            }else if(nodes.size()>=2){
                cliqueNumber++;
                totalCliqueSize += nodes.size();
            }
            if (nodes.size()>MaxCliqueSize){
                MaxCliqueSize = (int)nodes.size();
            }
            
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    
    cout<<"# MaximalCliquesNumber:"<<cliqueNumber<<"    totalCliqueSize:"<<totalCliqueSize
    <<"    averageCliqueSize:"<<(double)totalCliqueSize/cliqueNumber<<"     MaxCliqueSize:"<<MaxCliqueSize<<endl;
    
}
void Tool::countCandidateResult(const string& filePath1){
    int cliqueNumber = 0;
    int MaxCliqueSize = 2;
    set<string> cliqueSet1;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            Tool::split(line1,"\t",nodes);
            if(nodes.size()==2){
                cliqueNumber++;
            }else if(nodes.size()==3){
                vector<string> node;
                Tool::split(nodes[1]," ",node);
                
                if (node.size()>0){
                    MaxCliqueSize = (int)node.size() + 2;
                }

            }else{
                cout<<"Clique Format Error: "<<line1<<endl;
            }
            
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    
    cout<<"# Maximal Cliques:"<<cliqueNumber<<"     MaxCliqueSize:"<<MaxCliqueSize<<endl;
    
}



void Tool::countDegeneracy(const string& filePath1){
    int Degeneracy = 0;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            Tool::split(line1,"\t",nodes);
            if(nodes.size()==2){
                int newInt =0;
                std::stringstream ss;
                ss << nodes[1];
                ss >> newInt; //string -> int
                if(newInt>Degeneracy){
                    Degeneracy= newInt;
                }
            }else{
                 cout<<"Degeneracy Format Error: "<<line1<<endl;
            }
            
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    cout<<"#    Degeneracy:"<<Degeneracy<<endl;    
}


void Tool::compareResult0(const string& filePath1,const string& filePath2){
    set<string> cliqueSet1;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            cliqueSet1.insert(line1);
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    set<string> cliqueSet2;
    ifstream dataFile2(filePath2.c_str());
    if(dataFile2.is_open()){
        string line2;
        while(getline(dataFile2,line2)){
            Tool::trim(line2);
            vector<string> nodes;
            cliqueSet2.insert(line2);
        }
        dataFile2.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    string nodeIn1 = "";
    for(set<string>::iterator it2 = cliqueSet2.begin();it2!=cliqueSet2.end();it2++){
        if(cliqueSet1.find(*it2)==cliqueSet1.end()){
            nodeIn1 += *it2 +"\n";
        }
    }
    cout<< "---------------- Not In File1: ----------------"<<endl;
    cout<< nodeIn1;
    
    
    string nodeIn2 = "";
    for(set<string>::iterator it1 = cliqueSet1.begin();it1!=cliqueSet1.end();it1++){
        if(cliqueSet2.find(*it1)==cliqueSet2.end()){
            nodeIn2 += *it1 +"\n";
        }
    }
    cout<< "---------------- Not In File2: ----------------"<<endl;
    cout<< nodeIn2;
    
}


void Tool::compareResult1(const string& filePath1,const string& filePath2){
    map<string,set<string>*> cliqueMap1;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            Tool::split(line1," ",nodes);
            if (nodes.size()==1){
                cout<<"Error in file2 : "<<nodes[0]<<endl;
            }else if(nodes.size()>=2 ){
                map<string,set<string>*>::iterator it1 = cliqueMap1.find(nodes[0]);
                if(it1==cliqueMap1.end()){
                    set<string>* set1=new set<string>();
                    set1->insert(line1);
                    cliqueMap1.insert(make_pair(nodes[0],set1));
                }else{
                    set<string>* set1=cliqueMap1[nodes[0]];
                    set1->insert(line1);
                }
            }
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    map<string,set<string>*> cliqueMap2;
    ifstream dataFile2(filePath2.c_str());
    if(dataFile2.is_open()){
        string line2;
        while(getline(dataFile2,line2)){
            Tool::trim(line2);
            vector<string> nodes;
            Tool::split(line2," ",nodes);
            if (nodes.size()==1){
                cout<<"Error in file2 : "<<nodes[0]<<endl;
            }else if(nodes.size()>=2 ){
                map<string,set<string>*>::iterator it2 = cliqueMap2.find(nodes[0]);
                if(it2==cliqueMap2.end()){
                    set<string>* set2=new set<string>();
                    set2->insert(line2);
                    cliqueMap2.insert(make_pair(nodes[0],set2));
                }else{
                    set<string>* set2=cliqueMap2[nodes[0]];
                    set2->insert(line2);
                }
            }
        }
        dataFile2.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    string nodeIn1 = "";
    for(map<string,set<string>*>::iterator it2 = cliqueMap2.begin();it2!=cliqueMap2.end();it2++){
        string header2 = (*it2).first;
        set<string>* set2 = (*it2).second;
        if(cliqueMap1.find(header2)==cliqueMap1.end()){
            for(set<string>::iterator setIt2 = set2->begin();setIt2!=set2->end();setIt2++){
                nodeIn1 += *setIt2 +"\n";
            }
        }else{
            set<string>* set1 = cliqueMap1[header2];
            for(set<string>::iterator setIt2 = set2->begin();setIt2!=set2->end();setIt2++){
                if(set1->find(*setIt2)==set1->end()){
                    nodeIn1 +=*setIt2 +"\n";
                }
            }
        }
    }
    cout<< "---------------- Not In File1: ----------------"<<endl;
    cout<< nodeIn1;
    
    
    string nodeIn2 = "";
    for(map<string,set<string>*>::iterator it1 = cliqueMap1.begin();it1!=cliqueMap1.end();it1++){
        string header1 = (*it1).first;
        set<string>* set1 = (*it1).second;
        if(cliqueMap2.find(header1)==cliqueMap2.end()){
            for(set<string>::iterator setIt1 = set1->begin();setIt1!=set1->end();setIt1++){
                nodeIn2 += *setIt1 +"\n";
            }
        }else{
            set<string>* set2 = cliqueMap2[header1];
            for(set<string>::iterator setIt1 = set1->begin();setIt1!=set1->end();setIt1++){
                if(set2->find(*setIt1)==set2->end()){
                    nodeIn2 +=*setIt1 +"\n";
                }
            }
        }
    }
    cout<< "---------------- Not In File2: ----------------"<<endl;
    cout<< nodeIn2;
    
}


bool Tool::compareResult2(const string& filePath1,const string& filePath2){
    map<string,set<string>*> cliqueMap1;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> nodes;
            Tool::split(line1," ",nodes);
            if (nodes.size()==1){
                cout<<"Error in file1 : "<<nodes[0]<<endl;
            }else if(nodes.size()>=2 ){
                set<string> sortClique;
                for(int j = 0;j<nodes.size();j++){
                    sortClique.insert(nodes[j]);
                }
                set<string>::iterator it = sortClique.begin();
                string head = *it;
                string line = "";
                for(;it!=sortClique.end();it++){
                    line += *it +" ";
                }
                Tool::trim(line);
                if(cliqueMap1.find(head)==cliqueMap1.end()){
                    set<string>* set1=new set<string>();
                    set1->insert(line);
                    cliqueMap1.insert(make_pair(head,set1));
                }else{
                    set<string>* set1=cliqueMap1[head];
                    set1->insert(line);
                }
            }
        }
        dataFile1.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    map<string,set<string>*> cliqueMap2;
    ifstream dataFile2(filePath2.c_str());
    if(dataFile2.is_open()){
        string line2;
        while(getline(dataFile2,line2)){
            Tool::trim(line2);
            vector<string> nodes;
            Tool::split(line2," ",nodes);
            if (nodes.size()==1){
                cout<<"Error in file2 : "<<nodes[0]<<endl;
            }else if(nodes.size()>=2 ){
                set<string> sortClique;
                for(int j = 0;j<nodes.size();j++){
                    sortClique.insert(nodes[j]);
                }
                set<string>::iterator it = sortClique.begin();
                string head = *it;
                string line = "";
                for(;it!=sortClique.end();it++){
                    line += *it +" ";
                }
                Tool::trim(line);
                if(cliqueMap2.find(head)==cliqueMap2.end()){
                    set<string>* set2=new set<string>();
                    set2->insert(line);
                    cliqueMap2.insert(make_pair(head,set2));
                }else{
                    set<string>* set2=cliqueMap2[head];
                    set2->insert(line);
                }
            }
        }
        dataFile2.close();
    }else{
        cout<<"Open File Error!"<<endl;
    }
    
    string nodeIn1 = "";
    for(map<string,set<string>*>::iterator it2 = cliqueMap2.begin();it2!=cliqueMap2.end();it2++){
        string header2 = (*it2).first;
        set<string>* set2 = (*it2).second;
        if(cliqueMap1.find(header2)==cliqueMap1.end()){
            for(set<string>::iterator setIt2 = set2->begin();setIt2!=set2->end();setIt2++){
                nodeIn1 += *setIt2 +"\n";
            }
        }else{
            set<string>* set1 = cliqueMap1[header2];
            for(set<string>::iterator setIt2 = set2->begin();setIt2!=set2->end();setIt2++){
                if(set1->find(*setIt2)==set1->end()){
                    nodeIn1 +=*setIt2 +"\n";
                }
            }
        }
    }
    cout<< "---------------- Not In File1: ----------------"<<endl;
    cout<< nodeIn1;
    
    
    string nodeIn2 = "";
    for(map<string,set<string>*>::iterator it1 = cliqueMap1.begin();it1!=cliqueMap1.end();it1++){
        string header1 = (*it1).first;
        set<string>* set1 = (*it1).second;
        if(cliqueMap2.find(header1)==cliqueMap2.end()){
            for(set<string>::iterator setIt1 = set1->begin();setIt1!=set1->end();setIt1++){
                nodeIn2 += *setIt1 +"\n";
            }
        }else{
            set<string>* set2 = cliqueMap2[header1];
            for(set<string>::iterator setIt1 = set1->begin();setIt1!=set1->end();setIt1++){
                if(set2->find(*setIt1)==set2->end()){
                    nodeIn2 +=*setIt1 +"\n";
                }
            }
        }
    }
    cout<< "---------------- Not In File2: ----------------"<<endl;
    cout<< nodeIn2;
    
    
    for(map<string,set<string>*>::iterator it1 = cliqueMap1.begin();it1!=cliqueMap1.end();it1++){
        set<string>* set1 = (*it1).second;
        delete set1;
    }
    
    for(map<string,set<string>*>::iterator it2 = cliqueMap2.begin();it2!=cliqueMap2.end();it2++){
        set<string>* set2 = (*it2).second;
        delete set2;
    }
    if(nodeIn1=="" && nodeIn2==""){
        return true;
    }else{
        return false;
    }
}

void Tool::weiboDataGeneration(const string& filePath1,const string& fileName1,const string& filePath2){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    newFile<<"# weibo data : "<<fileName1<<"\n";
    string readFile = filePath1 + fileName1;
    ifstream dataFile1(readFile.c_str());
    if(dataFile1.is_open()){
        string line1;
        while(getline(dataFile1,line1)){
            Tool::trim(line1);
            vector<string> sep;
            split(line1,"]",sep);
            if(sep.size()==2){
                vector<string> sepAgain;
                split(sep[0],"[",sepAgain);
                if(sepAgain.size()==2){
                    string currentNode="XX";
                    vector<string> nodes;
                    split(sepAgain[0],",",nodes);
                    if(nodes.size()==2){
                        vector<string> currentNodes;
                        split(nodes[0],": ",currentNodes);
                        if(currentNodes.size()==2){
                            currentNode = currentNodes[1];
                        }else{
                            cout<<" weibo data format error  1"<<nodes[0]<<endl;
                        }
                    }else{
                        cout<<" weibo data format error  2"<<sepAgain[0]<<endl;
                    }

                    vector<string> neighbors;
                    split(sepAgain[1],", ",neighbors);
                    for(int i =0;i<neighbors.size();i++){
                        newFile << currentNode << "\t" << neighbors[i] << "\n";
                    }
                }else{
                    cout<<" weibo data format error  3"<<sep[0]<<endl;
                }
            }else{
                cout<<" weibo data format error  4"<<line1<<endl;
            }
        }
    }
    
    newFile.close();

}

void Tool::pajekDataGeneration2(const string& filePath1,const string& filePath2,const string& title){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    newFile << "# "<< title<<endl;
    newFile << "# FromNodeId" << "\t"<<"ToNodeId"<<endl;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line;
        while(getline(dataFile1,line)){
            if(line.at(0)=='#'){
                newFile << line << "\n";
            }else if(line.at(0)=='*' || line.at(0)=='%'){
                cout << line << "\n";
            }else{
                Tool::trim(line);
                vector<string> nodes;
                split(line," ",nodes);
                if(nodes.size()>=2){
                    for(int i=1;i<nodes.size();i++){
                        newFile << nodes[0] << "\t" << nodes[i] << "\n";
                    }
                }else{
                    cout<<nodes.size()<<" ";
                    for(vector<string>::iterator it = nodes.begin();it!=nodes.end();it++){
                        cout<<"<"<<*it<<">";
                    }
                    cout<<endl;
                }
            }
        }
    }
    
    newFile.close();
    
}


void Tool::pajekDataGeneration1(const string& filePath1,const string& filePath2,const string& title){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    newFile << "# "<< title<<endl;
    newFile << "# FromNodeId" << "\t"<<"ToNodeId"<<endl;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string line;
        while(getline(dataFile1,line)){
            if(line.at(0)=='#'){
                newFile << line << "\n";
            }else if(line.at(0)=='*' || line.at(0)=='%'){
                cout << line << "\n";
            }else{
                vector<string> seps;
                split(line,"\"",seps);
                if(seps.size()>1){
                    cout << "=== " << line << "\n";
                }else{
                    Tool::trim(line);
                    vector<string> nodes;
                    split(line," ",nodes);
                    if(nodes.size()>=2){
                        newFile << nodes[0] << "\t" << nodes[1] << "\n";
                    }else{
                        cout<<nodes.size()<<" ";
                        for(vector<string>::iterator it = nodes.begin();it!=nodes.end();it++){
                            cout<<"<"<<*it<<">";
                        }
                        cout<<endl;
                    }
                }
            }
        }
    }
    
    newFile.close();
    
}


void Tool::networkDataGeneration(const string& filePath1,const string& filePath2,const string& title){
    ofstream newFile;
    newFile.open(filePath2.c_str());
    newFile << "# "<< title<<endl;
    newFile << "# FromNodeId" << "\t"<<"ToNodeId"<<endl;
    ifstream dataFile1(filePath1.c_str());
    if(dataFile1.is_open()){
        string source = "";
        string line;
        while(getline(dataFile1,line)){
            if(line.at(0)=='#'){
                newFile << line << "\n";
            }else{
                Tool::trim(line);
                vector<string> nodes;
                split(line," ",nodes);
                if(nodes.size()==2){
                    if(nodes[0]=="source"){
                        source = nodes[1];
                    }else if (nodes[0]=="target" && source!=""){
                        newFile << source << "\t" << nodes[1] << "\n";
                        source ="";
                    }
                }
            }
        }
    }
    
    newFile.close();
    
}





void Tool::commonNeighborForPeco4(const set<int>* v1,const set<int>* v2,string& result){
    set<int>::iterator it1 = v1->begin();
    set<int>::iterator it2 = v2->begin();
    stringstream ss;
    while(it1!=v1->end() && it2!=v2->end()){
        if(*it1 == *it2){
            ss.str("");
            ss<<*it1;
            result += ss.str()+" ";
            it1 ++;
            it2 ++;
        }else if(*it1 < *it2){
            it1 ++;
        }else{
            it2 ++;
        }
    }
}


void Tool::setJiaoji(const set<int> s1,const set<int> s2,set<int>& des){
    set<int>::iterator it1 = s1.begin();
    set<int>::iterator it2 = s2.begin();
    des.clear();
    while(it1!=s1.end() && it2!=s2.end()){
        if(*it1 == *it2){
            des.insert(*it1);
            it1 ++;
            it2 ++;
        }else if(*it1 < *it2){
            it1 ++;
        }else{
            it2 ++;
        }
    }
}

bool Tool::containSet(const set<int> s1,const set<int> s2){
    bool contain = true;
    for(set<int>::iterator it2 = s2.begin();it2!=s2.end();it2++){
        if(s1.find(*it2) == s1.end()){
            contain = false;
            break;
        }
    }
    return contain;
}


bool Tool::containVector(const vector<int> v1,const vector<int> v2){
    int i=0;
    int j=0;
    bool contain = true;
    while(i < v1.size() && j < v2.size()){
        if(v1[i] == v2[j]){
            i += 1;
            j += 1;
        }else if(v1[i] < v2[j]){
            i += 1;
        }else{
            contain = false;
            break;
        }
    }
    if(j < v2.size()){
        contain = false;
    }
    return contain;
}


void Tool::vectorIntersection(const vector<int> v1,bool inverse1,const vector<int> v2,vector<int>& des){
    if(inverse1){
        int i=0;
        int j=v2.size()-1;
        
        des.clear();
        
        while(j >=0 && i < v1.size()){
            if(v1[i] == v2[j]){
                des.push_back(v1[i]);
                i += 1;
                j -= 1;
            }else if(v1[i] > v2[j]){
                i += 1;
            }else{
                j -= 1;
            }
        }
    }else{
        int i=0;
        int j=0;
        
        des.clear();
        
        while(i < v1.size() && j < v2.size()){
            if(v1[i] == v2[j]){
                des.push_back(v1[i]);
                i += 1;
                j += 1;
            }else if(v1[i] < v2[j]){
                i += 1;
            }else{
                j += 1;
            }
        }
    }
}


void Tool::vectorIntersection(const vector<int> v1,const vector<int> v2,vector<int>& des){
    int i=0;
    int j=0;
    
    des.clear();
    
    while(i < v1.size() && j < v2.size()){
        if(v1[i] == v2[j]){
            des.push_back(v1[i]);
            i += 1;
            j += 1;
        }else if(v1[i] < v2[j]){
            i += 1;
        }else{
            j += 1;
        }
    }
}


void Tool::vectorUnion(const vector<int> v1,const vector<int> v2,vector<int>& des){
    int i=0;
    int j=0;
    
    des.clear();
    while(i < v1.size() && j < v2.size()){
        if(v1[i] == v2[j]){
            des.push_back(v1[i]);
            i += 1;
            j += 1;
        }else if(v1[i] < v2[j]){
            des.push_back(v1[i]);
            i += 1;
        }else{
            des.push_back(v2[j]);
            j += 1;
        }
    }
    
    while(i < v1.size()){
        des.push_back(v1[i]);
        i++;
    }
    while(j < v2.size()){
        des.push_back(v2[j]);
        j++;
    }
}


void Tool::vectorDifference(const vector<int> v1,const vector<int> v2,vector<int>& des){
    int i=0;
    int j=0;
    
    des.clear();
 
    while(i < v1.size() && j < v2.size()){
        if(v1[i] == v2[j]){
            i += 1;
            j += 1;
        }else if(v1[i] < v2[j]){
            des.push_back(v1[i]);
            i += 1;
        }else{
            j += 1;
        }
    }
    
    while(i < v1.size()){
        des.push_back(v1[i]);
        i++;
    }
    
}



void Tool::vectorbingji2( vector<int>& v1, vector<int> v2){
    
    vector<int>::iterator it1 = v1.begin();
    vector<int>::iterator it2 = v2.begin();

    while(it1!=v1.end() && it2!=v2.end()){
        if(*it1 == *it2){
            it1++;
            it2++;
        }else if(*it1 < *it2){
            it1++;
        }else{
            v1.insert(it1, *it2);
            it2++;
        }
    }
    
    while(it2 != v2.end()){
        v1.insert(it1,*it2);
        it1++;
        it2++;
    }
}


void Tool::vectorchaji2(vector<int>& v1, vector<int> v2){
    vector<int>::iterator it1 = v1.begin();
    vector<int>::iterator it2 = v2.begin();
    while(it1!=v1.end() && it2!=v2.end()){
        if(*it1 == *it2){
            v1.erase(it1);
            it1++;
            it2++;
        }else if(*it1 < *it2){
            it1++;
        }else{
            it2++;
        }
    }
    
}

