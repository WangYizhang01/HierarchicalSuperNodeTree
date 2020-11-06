#include "isSame.h"


void isSame::trim(string& s) {
    if (s.empty()) {
        return;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

vector<string> isSame::split(const string& s, const string& delim) {
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

vector<vector<int>> isSame::loadData(const string& filePath) {
    ifstream dataFile;
    dataFile.open(filePath);
    vector<vector<int>> result;
    vector<int> row;

    if (dataFile.is_open()) {
        string line;
        while (getline(dataFile, line)) {
            vector<string> row_s;
            trim(line);
            row_s = split(line, " ");
            row = {};

            for (unsigned int i = 0; i < row_s.size(); i++) {
                int vertex;
                stringstream ss1;
                ss1 << row_s[i];
                ss1 >> vertex; //string -> int
                row.push_back(vertex);
            }
            result.push_back(row);
        }
        dataFile.close();
    }
    else {
        cout << "Open File Error!" << endl;
    }
    return result;
}

void isSame::testIsSame(int i) {
    stringstream ss; string i_s;
    ss << i; ss >> i_s;

    string result_of_GHT = "C://Users//Administrator//Desktop//result//GHT//B" + i_s + ".txt";
    string result_of_HSNT = "C://Users//Administrator//Desktop//result//HSNT//HSNT" + i_s + ".txt";

    vector<vector<int>> matrix_of_GHT = loadData(result_of_GHT);
    vector<vector<int>> matrix_of_HSNT = loadData(result_of_HSNT);

    int flag = 0; vector<vector<int>> res; vector<int> row;

    for (unsigned int i = 0; i < matrix_of_GHT.size(); i++) {
        int flag1 = 0; row = {};
        for (unsigned int j = 0; j < matrix_of_HSNT.size(); j++) {
            if (matrix_of_GHT[i][j] != matrix_of_HSNT[i][j]) {
                flag = 1;
                if (flag1 == 0) {
                    row.push_back(i + 1);
                    flag1 = 1;
                }
                row.push_back(j + 1);
            }
            else {
                continue;
            }
        }
        if (row.size() > 0) {
            res.push_back(row);
        }
    }

    if (flag == 1) {
        cout << "The " + i_s + "th file is not same!" << endl;
        cout << "row        col" << endl;

        for (vector<vector<int>>::iterator it = res.begin(); it != res.end(); it++) {
            int flag2 = 0;
            for (vector<int>::iterator iit = (*it).begin(); iit != (*it).end(); iit++) {
                if (flag2 == 0) {
                    cout << *iit << "        ";
                    flag2 = 1;
                }
                else {
                    cout << *iit << "   ";
                }
            }
            cout << endl;
        }
    }
}


int main() {
    /*int num = 15;

    for (int i = 0; i < num; i++) {
        isSame T;
        T.testIsSame(i);
    }*/


    int num = 4;

    isSame T;
    T.testIsSame(num);
}