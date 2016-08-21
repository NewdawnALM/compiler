#ifndef  __LEXICAL_H
#define  __LEXICAL_H

#include "common.h"

#define  isLineBreak(ch)   (('\n' == (ch)) || ('\r' == (ch)))
#define  canIgnore(ch)     ((Symbol::SPACE == (ch)) || ('\t' == (ch)) || isLineBreak(ch))

const int MAX_BUFLEN = 1 * 1024 * 1024 + 3;          // 设置文件最大长度为 1M

/* 预处理程序，去除注释和空格，跳格和换行
 * filename 为文件名，buf返回处理好的结果
*/
int pretreatment(const char *const fileName, std::string &buf) {
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL) {
        fprintf(stderr, "can't open file %s!\n", fileName);
        return -1;
    }
    int ch = 0;
    bool needToRead = true;
    while(true) {
        if(needToRead)   ch = fgetc(fp);
        else    needToRead = true;

        if(ch == EOF)   break;

        else if(ch == '/') {
            int ch2 = fgetc(fp);
            if(ch2 == EOF) {
                buf.push_back(ch);
                ch = ch2;
            }
            else if(ch2 == '/') {
                while(ch != EOF && !isLineBreak(ch))   ch = fgetc(fp);
//                if(isLineBreak(ch))    ch = fgetc(fp);
            }
            else if(ch2 == '*') {
                ch = fgetc(fp);
                if(ch == EOF)   break;
                ch2 = fgetc(fp);
                if(ch2 == EOF)    ch = ch2;
                else {
                    while(true) {
                        if(ch == '*' && ch2 == '/') {
                            ch = fgetc(fp);
                            break;
                        }
                        ch = fgetc(fp);
                        if(ch == EOF)   break;
                        if(ch2 == '*' && ch == '/') {
                            ch = fgetc(fp);
                            break;
                        }
                        ch2 = fgetc(fp);
                        if(ch2 == EOF) {
                            ch = ch2;
                            break;
                        }
                    }
                }
            }
            else {
                buf.push_back(ch);
                ch = ch2;
            }
            needToRead = false;
        }

        else if(canIgnore(ch)) {
            if(buf.size() == 0 || Symbol::SPACE != buf[buf.size() - 1])    buf.push_back(Symbol::SPACE);
        }
        else    buf.push_back(ch);

        if(buf.size() > MAX_BUFLEN) {
            fprintf(stderr, "the size of file can't exceed 8M!\n");
            return -2;
        }
    }
//    if(buf.size() > 0 && canIgnore(*(buf.end() - 1)))    *(buf.end() - 1) = '#';
    /*else    */buf.push_back('#');
    fclose(fp);
    return buf.size();
}


typedef unsigned char uchar;

// 用于表示 nfa 状态图中每个状态的类
class Status {
    friend std::ostream& operator << (std::ostream&, const Status &st);
public:
    std::string id;         // 标识符
    bool isFinal;           // 是否为终态
    std::map<uchar, std::set<Status*>> next;        // 每个状态的后继状态，uchar为边，保存其后继状态的指针
    Status(const std::string &_id = "", bool _isFinal = false): id(_id), isFinal(_isFinal) { }
    bool operator < (const Status &rhs) const {   return id < rhs.id;   }
    bool operator == (const Status &rhs) const {   return id == rhs.id;   }
};

std::ostream& operator << (std::ostream &out, const Status &st) {
    return out << st.id;
}

// NFA 转换为 DFA 的类
class NFAToDFA {
public:
    std::set<uchar> I;          // 所有字符集，即边集
    std::vector<std::set<Status>> leftStatusSet;        // 表格的第一列，即每轮循环的初始状态集
    std::vector<std::map<uchar, std::set<Status>>> tableSet;        // 表格除了第一列外剩余的状态集
    std::vector<std::map<uchar, int>> DFATableSet;                  // DFA 表
    NFAToDFA(const Status &_start, const std::string &_name = ""): start(_start), name(_name) {   initTable();   }
    bool isFinalSet(int idx) const ;        // 判断状态集 leftStatusSet[idx] 是否为终态
    void showAll();
private:
    Status start;
    std::string name;
    void initI();
    void initTable();
};

// 先提取出 nfa 状态转移图的所有字符
void NFAToDFA::initI() {
    std::queue<Status> q;
    std::map<Status, bool> sign;
    q.push(start);
    sign[start] = true;
    while(!q.empty()) {
        auto p = q.front();
        q.pop();
        for(const auto &mit: p.next) {
            I.insert(mit.first);
            for(const auto &sit: mit.second) {
                if(sign[*sit] == false) {
                    q.push(*sit);
                    sign[*sit] = true;
                }
            }
        }
    }
}

// 计算 tableSet 和 DFATableSet
void NFAToDFA::initTable() {
    initI();
    leftStatusSet.push_back(std::set<Status>{ start });
    std::function<void(std::set<Status>&, const Status&)> push = [&](std::set<Status> &s, const Status &st) {
        s.insert(st);
        for(const auto &it: st.next)
            if(it.first == Symbol::NULCHAR)
            for(const auto &sit: it.second)
                push(s, *sit);
    };
    push(leftStatusSet[0], start);
    int cur = 0;
    tableSet.resize(1);

    while(cur < leftStatusSet.size()) {
        for(const auto &ch: I) {
            if(ch == Symbol::NULCHAR)   continue;
            std::set<Status> tmp;
            for(const auto &st: leftStatusSet[cur]) {
                if(st.next.count(ch)) {
                    for(const auto &sit: st.next.find(ch)->second)
                        push(tmp, *sit);
                }
            }
            tableSet[cur][ch] = tmp;
            if(tmp.size() && std::find(leftStatusSet.begin(), leftStatusSet.end(), tmp) == leftStatusSet.end()) {
                leftStatusSet.push_back(tableSet[cur][ch]);
                tableSet.push_back({});
            }
        }
        ++cur;
    }
    DFATableSet.resize(tableSet.size());
    For(i, 0, DFATableSet.size()) {
        for(const auto &row: tableSet[i]) {
            if(row.second.size() == 0)    DFATableSet[i][row.first] = -1;
            else    DFATableSet[i][row.first] = std::find(leftStatusSet.begin(), leftStatusSet.end(),
                                                        row.second) - leftStatusSet.begin();
        }
    }
}

bool NFAToDFA::isFinalSet(int idx) const {
//    std::cout << leftStatusSet[idx] << "\n";
    for(const auto &st: leftStatusSet[idx])
        if(st.isFinal == true)   return true;
    return false;
}

void NFAToDFA::showAll() {
    std::cout << "\n";
    if(name != "")     std::cout << name << "\n";
    std::cout << "NFA 表：\n" << "I";
    for(const auto &ch: I) {
        if(ch == Symbol::NULCHAR)   continue;
        std::cout << "\tI" << ch;
    }
    std::cout << "\n";
    For(idx, 0, leftStatusSet.size()) {
        std::cout << "{" << leftStatusSet[idx] << "}";
        for(const auto &ch: I) {
            if(ch == Symbol::NULCHAR)   continue;
            std::cout << "\t{" << tableSet[idx][ch] << "}";
        }
        std::cout << "\n";
    }
    std::cout << "\nDFA 表：\n" << "状态\\字符";
    for(const auto &ch: I) {
        if(ch == Symbol::NULCHAR)   continue;
        std::cout << "\t" << ch;
    }
    std::cout << "\n";
    For(idx, 0, leftStatusSet.size()) {
        std::cout << "    " << idx << "    ";
        for(const auto &ch: I) {
            if(ch == Symbol::NULCHAR)   continue;
            std::cout << "\t";
            if(DFATableSet[idx][ch] != -1)    std::cout << DFATableSet[idx][ch];
        }
        std::cout << "\n";
    }
}

#endif // __LEXICAL_H
