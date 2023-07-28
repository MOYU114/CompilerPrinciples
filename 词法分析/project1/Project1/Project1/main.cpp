#include<iostream>
#include <fstream>
#include <string>
#include<vector>
#include<unordered_map>
#include <regex>
#include<math.h>
#include <stdlib.h>
#include <stdbool.h>
#include<regex>
using namespace std;

unordered_map<string, string> words_encoding_table;


void Stringsplit(const string& str, vector<string>& res)
{
    string temp = "";
    for (int i = 0; i < str.size(); i++) {
        char c = str[i];
        if (isspace(c)) {
            if (!temp.empty()) {
                res.push_back(temp);
                temp = "";
            }
        }
        else if (isdigit(c) || isalpha(c) ||ispunct(c)){
            if (c == ';' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '{' || c == '}') {
                if (!temp.empty()) {
                    res.push_back(temp);
                    temp = "";
                }
                res.push_back(string(1, c));
                
            }
            else if (c == '=' || c == '!' || c == '<' || c == '>') {
                if (!temp.empty()) {
                    res.push_back(temp);
                    temp = "";
                }
                if (i + 1 < str.size() && str[i + 1] == '=') {
                    string doublepunct = string(1, c) + string(1, str[i + 1]);
                    res.push_back(doublepunct);
                    i++;
                }
                else {
                    res.push_back(string(1, c));
                }
                
               
            }
            else 
                temp += c;
            
        }
    }
    if (!temp.empty()) {
          res.push_back(temp);
    }
}
vector<string> readProgramFile(string PATH) {
    vector<string> result_vector;
    ifstream infile;
    infile.open(PATH.c_str());
    string data;
    while (!infile.eof()) {
        getline(infile, data);
        Stringsplit(data, result_vector);
    }
    return result_vector;
}
void readTableFile(string PATH) {
    vector<string> result_vector;
    ifstream infile;
    infile.open(PATH.c_str());
    string data;
    while (!infile.eof()) {
        getline(infile, data);
        size_t pos = data.find("\t");
        string word = data.substr(0, pos);
        string number = data.substr(pos + 1, data.size());
        words_encoding_table[word] = number;
    }
    return;
}

bool isKeyWord(string word) {//�ж����޸ùؼ���
    if (word == "auto" || word == "double" || word == "float" || word == "short" || word == "char" || word == "long" || word == "int")
        return true;
    else
        return false;
}
bool isIdentify(string word) {//������ʽ�ж�
    regex pattern("[a-zA-Z_][a-zA-Z0-9_]*");
    if (regex_match(word, pattern))
        return true;
    else
        return false;
}
bool isNumber(string word) {//������ʽ�жϷ��ϣ����ֽ��ƵĽ���С������������ѧ������
    regex pattern(("^[-+]?((0[bB][01]+)|(0[oO][0-7]+)|(0[xX][0-9a-fA-F]+)|([0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?))$"));
    if (regex_match(word, pattern))
        return true;
    else
        return false;
}
vector<string>wordsAnalyze(vector<string> words_list) {
    vector<string>result;
    for(int i=0;i < words_list.size();++i)
    {
        bool flag = true;
        if (isKeyWord(words_list[i])) {//�ж��Ƿ�Ϊ�ؼ���
            if (i != words_list.size() - 1 && isIdentify(words_list[i + 1])) {
                result.push_back(words_encoding_table[words_list[i]]);
                result.push_back(words_encoding_table["identify"]);
                i++;//�����ж���һ�����
            }
            else {
                result.push_back("error " + words_encoding_table[words_list[i]]);//����
            }
        }
        else if(words_encoding_table.find(words_list[i]) != words_encoding_table.end()){//�Ƿ����ڱ��еķ���
            result.push_back(words_encoding_table[words_list[i]]);
        }
        else if (isNumber(words_list[i]))//�Ƿ�Ϊ��ȷ����
        {
            result.push_back(words_encoding_table["constant"]);
        }
        else if (isIdentify(words_list[i])) {//�ж��Ƿ�Ϊ�Ϸ�������
            result.push_back(words_encoding_table["identify"]);
        }
        else {//��������
            result.push_back("error 0");
        }
    }
    return result;
}

int main() {
    string TABLE_PATH = "./data/table.txt";
    string PROGRAME_PATH_CORRECT = "./data/test_correct.txt";
    string SAVE_PATH_CORRECT = "./data/result_correct.txt";
    string PROGRAME_PATH_WRONG = "./data/test_wrong.txt";
    string SAVE_PATH_WRONG = "./data/result_wrong.txt";
    vector<string> words_list,result;
    readTableFile(TABLE_PATH);
    cout << "��ȷ���룺" << endl;
    words_list = readProgramFile(PROGRAME_PATH_CORRECT);
    result = wordsAnalyze(words_list);
    ofstream outFileCorrect(SAVE_PATH_CORRECT);
    for (int i = 0; i < words_list.size(); ++i) {
        cout << words_list[i] << "\t" << result[i] << endl;
        outFileCorrect << words_list[i] << "\t" << result[i] << endl;
    }
    outFileCorrect.close();
    words_list.clear();
    result.clear();
    cout << "������룺" << endl;
    words_list = readProgramFile(PROGRAME_PATH_WRONG);
    result = wordsAnalyze(words_list);
    ofstream outFileWrong(SAVE_PATH_WRONG);
    for (int i = 0; i < words_list.size(); ++i) {
        cout << words_list[i] << "\t" << result[i] << endl;
        outFileWrong << words_list[i] << "\t" << result[i] << endl;
    }
    outFileWrong.close();
}