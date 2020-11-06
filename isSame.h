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
    void trim(string& s); // È¥³ý×Ö·û´®Ê×Î²¿Õ¸ñ
    vector<string> split(const string& str, const string& delim); // ·Ö¸î×Ö·û´®
    vector<vector<int>> loadData(const string& filePath); // ¶ÁÈ¡½á¹û¾ØÕó
    void testIsSame(int i);
};

#endif // !isSame_H
