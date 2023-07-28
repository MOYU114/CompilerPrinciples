#include<iostream>
#include <fstream>
#include <string>
#include<vector>
#include<unordered_map>
#include <regex>
#include<math.h>
#include <stdlib.h>
using namespace std;
//为了方便，epsilon使用'#'代替
typedef struct Grammar
{
    string G;
    char S;
    vector<char> Vn;
    vector<string> P;
    vector<char> Vt;
    unordered_map<string, vector<string>> map;
    int ThreeType = -1;//专门判断三型文法具体类型
}Gr ;
//重要，因为无限输入，需要ctrl+z终止，故最后需要读入一个‘\n’清除，故如果直接复制粘贴不加回车会有问题
void writeFile(string PATH) {
    ofstream origin_file;
    string data;
    origin_file.open(PATH.c_str());
    printf("输入文法：");
    getline(cin, data);
    origin_file << data << endl;
    printf("输入Vn：");
    getline(cin, data);
    origin_file << data << endl;
    printf("输入产生式规则：");
    while (getline(cin, data)) {
        if (data.empty()) break;
        origin_file << data << endl;
    }
    origin_file.close();

}
// 使用字符串分割
void Stringsplit(const string& str, const string& splits, vector<string>& res)
{
    if (str == "")		return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}
void readFile(string PATH,Gr * Gra ) {
    ifstream infile;
    infile.open(PATH.c_str());
    string data;
    //读取文法、终结符、非终结符、生成式、开始符
    getline(infile, data);
    Gra->G = data;
    Gra->S;
    for (int i = 1; i < Gra->G.length(); i++)
        if (Gra->G[i - 1] == '[' && Gra->G[i + 1] == ']') {
            Gra->S = Gra->G[i];
        }
    getline(infile, data);
    for (int i = 0; data[i] != '\0'; i++)
        if (data[i] != ',') {
            Gra->Vn.push_back(data[i]);
        }

    while (!infile.eof()) {
        getline(infile, data);
        Gra->P.push_back(data);
        for (int i = 0; i < data.size(); i++) {
            if (find(Gra->Vt.begin(), Gra->Vt.end(), data[i]) == Gra->Vt.end() && data[i] >= 'a' && data[i] <= 'z' || data[i] >= '0' && data[i] <= '9' || data[i] == '#') {
                Gra->Vt.push_back(data[i]);
            }
        }
    }
    Gra->P.pop_back();
    infile.close();
    // 使用 unordered_map 存储产生式对应关系
    for (int i = 0; i < Gra->P.size(); i++) {
        vector<string> v;
        Stringsplit(Gra->P[i], "::=", v);
        vector<string> vs;
        Stringsplit(v[1], "|", vs);
        for (string ss : vs) {
            Gra->map[v[0]].push_back(ss);
        }
        
    }
}




int isValid(string left, string right, vector<char> Vn, vector<char> Vt)
{
    //左边必须有一个非终结符
    for (int i = 0; i < left.length(); i++) {
        char s = left[i];
        //如果终结符和非终结符中都不包含该字符，返回false
        if (find(Vn.begin(), Vn.end(), s) == Vn.end() && find(Vt.begin(), Vt.end(), s) == Vt.end() && s != '#')
            return 0;
    }
    for (int i = 0; i < right.length(); i++) {
        char s = right[i];
        //如果终结符和非终结符中都不包含该字符，返回false
        if (find(Vn.begin(), Vn.end(), s) == Vn.end() && find(Vt.begin(), Vt.end(), s) == Vt.end() && s != '#' && s != '|')
            return 0;
    }
    return 1;
}
/* int JudgeVNLinear(char s, vector<char> Vn, vector<char> Vt, unordered_map<string, string> map) {
    //先判断是否为非终结符
    if (find(Vn.begin(), Vn.end(), s) != Vn.end()) {
        //判断产生式是否为3型文法
        string result = map[string(1, s)];
        //式子右边两个字符
        if (result.length() == 2) {
            //判断是左线型还是右线型
            char first = result[0];
            char second = result[1];
            //左边为非终结符，右边为终结符,判断是否为左线性
            if (find(Vn.begin(), Vn.end(), first) != Vn.end() && find(Vt.begin(), Vt.end(), second) != Vt.end()) {
                if (first == s || JudgeVNLinear(first, Vn, Vt, map) == 2) {
                    return 2;
                }
            }
            //左边为终结符，右边为非终结符，判断是否为右线性
            else if (find(Vn.begin(), Vn.end(), second) != Vn.end() && find(Vt.begin(), Vt.end(), first) != Vt.end()) {
                if (second == s || JudgeVNLinear(second, Vn, Vt, map) == 1)
                    return 1;
            }
        }
        //右边只有一个终结符
        else if (result.length() == 1) {
            if (find(Vt.begin(), Vt.end(), result[0]) != Vt.end())
                return 0;
        }
    }
    return -1;
}*/
int isThreeGrammar(string left, string right, vector<char> Vn, vector<char> Vt) {
    //先判断左边，左边只能有一个字符，而且必须是非终结符
    if (left.length() == 1 && find(Vn.begin(), Vn.end(), left[0]) != Vn.end()) {
        //判别式的右边最多只能有两个字符，且当判别式的右边有两个字符时必须有一个为终结符而另一个为非终结符。
        if (right.length() == 2) {
            //判断是左线型还是右线型
            char first = right[0];
            char second = right[1];
            //左边为非终结符(对应产生式也为左线性)，右边为终结符,判断是为左线性
            if (find(Vn.begin(), Vn.end(), first) != Vn.end() && find(Vt.begin(), Vt.end(), second) != Vt.end()) {
                
                    return 2;//代表左线性
            }
            //左边为终结符，右边为非终结符(对应产生式也为右线性)，判断是否为右线性
            else if (find(Vn.begin(), Vn.end(), second) != Vn.end() && find(Vt.begin(), Vt.end(), first) != Vt.end()) {
                
                    return 3;//代表右线性
            }
        }
        //当判别式的右边只有一个字符时，此字符必须为终结符；
        else if (right.length() == 1) {
            if (find(Vt.begin(), Vt.end(), right[0]) != Vt.end())
                return 1;
        }
    }
    return 0;
}

int isTwoGrammar(string left, string right, vector<char> Vn, vector<char> Vt)
{
    // 左边必须有且仅有一个非终结符
    if (left.length() == 1 && find(Vn.begin(), Vn.end(), left[0]) != Vn.end()) {
        return 1;
    }
    return 0;
}

int isOneGrammar(string left, string right, vector<char> Vn, vector<char> Vt)
{
    if (right.length() >= left.length()) {
        // 左边必须有一个非终结符
        for (int i = 0; i < left.length(); i++) {
            char s = left[i];
            if (find(Vn.begin(), Vn.end(), s) != Vn.end())
                return 1;
        }
    }
    // 特意判断ε
    else if (left.length() == 1 && right == "#" && find(Vn.begin(), Vn.end(), left[0]) != Vn.end()) {
        return 1;
    }
    return 0;
   
}

int isZeroGrammar(string left, string right, vector<char> Vn, vector<char> Vt)
{
    // 左边必须有一个非终结符
    for (int i = 0; i < left.length(); i++) {
        char s = left[i];
        if (find(Vn.begin(), Vn.end(), s) != Vn.end())
            return 1;
    }
    return 0;
}
int judgeType(Gr* Gra) {
    int type = 4;
    int crr = -1;//初始
    for (const auto& kv : Gra->map) {
        auto& left = kv.first;
        for (const auto& ss : Gra->map[left]) {
            auto& right = ss;
            if (!isValid(left, right, Gra->Vn, Gra->Vt)) {
                printf("产生式不合法！\n");
                return -1;
            }
            if (isThreeGrammar(left, right, Gra->Vn, Gra->Vt)) {
                crr = isThreeGrammar(left, right, Gra->Vn, Gra->Vt);
                type = min(type, 3);
                if (Gra->ThreeType != -1 && crr != 1 && Gra->ThreeType != crr) {//ThreeType不为初始情况，且crr不为1（不考虑只为终结符），判断ThreeType与crr是否相等
                    type = 2;//如果不为统一的左/右线性文法，则说明不为三型文法，直接降级为二型
                }
                if (crr != 1) {
                    Gra->ThreeType = crr;
                }
                else if(Gra->ThreeType==-1)//不存在三型文法时
                {//对右侧只用单个字符进行判断
                    right == "#" ? type = 0 : type = 1;
                }
            }
            else if (isTwoGrammar(left, right, Gra->Vn, Gra->Vt)) {
                type = min(type, 2);
            }
            else if (isOneGrammar(left, right, Gra->Vn, Gra->Vt)) {
                type = min(type, 1);
            }
            else if (isZeroGrammar(left, right, Gra->Vn, Gra->Vt)) {
                type = min(type, 0);
            }
            //不是文法
            else {
                type = -1;
            }
        }
        

    }
    return type;
}
void printResult(int type,Gr*Gra) {
    string VN, VT;
    for (int i = 0; i < Gra->Vt.size(); i++) {
        VT += string(1, Gra->Vt[i]) + ',';
    }
    VT.pop_back();
    for (int i = 0; i < Gra->Vn.size(); i++) {
        VN += string(1, Gra->Vn[i]) + ',';
    }
    VN.pop_back();;
    cout << "文法" << Gra->G << "=({" << VN << "},{" << VT << "},P," << Gra->S << ")" << endl;
    cout << "P: "<<endl;
    for (int i = 0; i < Gra->P.size(); i++)
    {
        cout << Gra->P[i] << endl;
    }
    switch (type)
    {
    case 0:
        printf("该文法为Chomsky0型文法（即无限制文法）\n");
        break;
    case 1:
        printf("该文法为Chomsky1型文法（即上下文有关文法）\n");
        break;
    case 2:
        printf("该文法为Chomsky2型文法（即上下文无关文法）\n");
        break;
    case 3:
        printf("该文法为Chomsky3型文法（即正则文法），");
        Gra->ThreeType == 3? printf("为右线性\n"): printf("为左线性\n");
        break;
    default:
        printf("不为文法\n");
        break;
    }
}

int main() {
    //1.文件输入
    string PATH = "test.txt";
    writeFile(PATH);
    //2.数据预处理
    Gr* Gra = new Grammar;
    readFile(PATH, Gra);
    //3.文法类型判断
    int type = judgeType(Gra);
    //4.结果打印
    printResult(type,Gra);
    cout << endl;
    system("pause");
    return 0;
}

