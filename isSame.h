#pragma once
#ifndef isSame_H
#define isSame_H


#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;


class isSame {
public:
    void trim(string& s); // ȥ���ַ�����β�ո�
    vector<string> split(const string& str, const string& delim); // �ָ��ַ���
    vector<vector<int>> loadData(const string& filePath); // ��ȡ�������
    void testIsSame(int i);
};

#endif // !isSame_H
