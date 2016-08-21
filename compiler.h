#ifndef  __COMPILER_H
#define  __COMPILER_H

#include "lexical.h"
#include "grammar.h"
#include "rules.h"

// 含词法分析和使用LL(1)预测分析法的语法分析的编译器前端
class Compiler {
public:
    Compiler(const std::string &_fileName): fileName(_fileName) { }
    Compiler(const Compiler &rhs) = delete;
    Compiler& operator = (const Compiler &rhs) = delete;
    int cat() const;                                        // 查看文件原内容
    std::string pretreatment() const ;                      // 返回预处理的结果
    std::vector<Symbol> lexicalAnalysis() const ;           // 词法分析的结果，返回单词二元式
    bool predictiveAnalysis() const ;                       // 文法分析的结果
public:
    std::string strLexicalAnalysis() const ;           // 这两个函数是给肥轩你用的，一个是以 string 格式返回的单词二元式
    const char* charLexicalAnalysis() const ;          // 另一个是以 char* 格式返回的单词二元式
private:
    const std::string fileName;                 // 需要进行词法分析和文法分析的源文件
    Symbol scanner(const std::string &, int &) const;       // 词法分析的扫描器
};

int Compiler::cat() const {
    FILE *fp = fopen(this->fileName.c_str(), "r");
    if(fp == NULL) {
        fprintf(stderr, "can't open file %s!\n", this->fileName.c_str());
        return 1;
    }
    std::string ret = "";
    char ch = fgetc(fp);
    while(ch != EOF) {
        ret += ch;
        ch = fgetc(fp);
    }
    fclose(fp);
    std::cout << ret << "\n";
    return 0;
}

std::string Compiler::pretreatment() const {
    static std::string pretreatmentResult = "";
    if(pretreatmentResult != "")    return pretreatmentResult;
    int ret = ::pretreatment(fileName.c_str(), pretreatmentResult);
    if(ret < 0) {
        std::cerr << "pretreatment error!\n";
        exit(0);
    }
    return pretreatmentResult;
}

std::vector<Symbol> Compiler::lexicalAnalysis() const {
    static std::vector<Symbol> lexicalAnalysisResult = { };
    if(lexicalAnalysisResult.size() != 0)    return lexicalAnalysisResult;
    std::string buf = this->pretreatment();
    int i = 0;
    do {
        while(buf[i] == Symbol::SPACE)    ++i;
        lexicalAnalysisResult.push_back(this->scanner(buf, i));
    } while((lexicalAnalysisResult.end() - 1)->code != Symbol::ENDCHAR);
    return lexicalAnalysisResult;
}

bool Compiler::predictiveAnalysis() const {
    static std::vector<Symbol> lexical = this->lexicalAnalysis();
    static PredictiveAnalysis prediction(myGrammar85::pro);
    prediction.showAll();
    std::stack<Symbol> symbolStack;
    symbolStack.push(ENDSYMBOL);
    symbolStack.push(prediction.startSymbol);
    auto it = lexical.begin();
    while(true) {
        Symbol X = symbolStack.top();
        symbolStack.pop();
        if(X == ENDSYMBOL) {
            if(*it == ENDSYMBOL)    return true;
            else {
                std::cerr << "error in " << it->code << ", expect the program ends here.\n";
                return false;
                exit(0);
            }
        }
        else if(X.isEndChar()) {
            if(X.code == it->code)   ++it;
            else {
                std::cerr << "error in " << it->code << ", expect " << X.code << " for the next symbol.\n";
                return false;
                exit(0);
            }
        }
        else if(X.isNonEndChar()) {
            int productionIndex = prediction.table[X][*it];
            if(productionIndex == prediction.errorSign) {
                std::cerr << "error in " << it->code << ", not found a production for " << it->code << " behind " << X.code << ".\n";
                return false;
                exit(0);
            }
            else {
                std::vector<Symbol> rightSymbol = prediction.grammar[productionIndex].right;
                assert(rightSymbol.size() > 0);
                if(rightSymbol[0] == NULSYMBOL)   continue;
                For(rit, rightSymbol.rbegin(), rightSymbol.rend()) {
                    symbolStack.push(*rit);
                }
            }
        }
    }
}

Symbol Compiler::scanner(const std::string &buf, int &idx) const {
    static NFAToDFA nfa(myNFA::start(), "my: ");
    static auto trans = [&](char ch) -> uchar {
        if('e' == ch || 'E' == ch)   return 'e';
        if(isUppercase(ch) || isLowercase(ch))   return 'a';
        if(isDigit(ch))     return '0';
        return ch;
    };
    if(buf[idx] == Symbol::ENDCHAR)    return Symbol(Symbol::ENDCHAR);
    Symbol ret;
    int row = 0, col = trans(buf[idx]);
    while(true) {
        if(nfa.DFATableSet[row].count(col) == 0) {
            std::cerr << "can't not recognize the letter " << buf[idx] << "!\n";
            exit(0);
        }
        int nextRow = nfa.DFATableSet[row][col];
        if(nextRow == -1) {
            if(nfa.isFinalSet(row) == false) {
                std::cerr << "can't not recognize the word " << ret.value << "!\n";
                exit(0);
            }
            break;
        }
        row = nextRow;
        ret.value += buf[idx];
        ++idx;
        if(Symbol::ENDCHAR == buf[idx] || Symbol::SPACE == buf[idx])    break;
        col = trans(buf[idx]);
    }
    if(nfa.isFinalSet(row) == false) {
        std::cerr << "can't not recognize the word " << ret.value << "!\n";
        exit(0);
    }
    assert(ret.value.size() > 0);
    if(isLetter(ret.value[0])) {
        if(ret.value == "begin")    ret.code = '{';
        else if(ret.value == "end")     ret.code = '}';
        else if(ret.value == "uint")    ret.code = 'a';
        else if(ret.value == "udouble")      ret.code = 'c';
        else    ret.code = 'i';
    }
    else if(ret.value[0] == '.')    ret.code = 'y';
    else if(isDigit(ret.value[0])) {
        if(ret.value.find('.') != std::string::npos || ret.value.find('E') != std::string::npos
                || ret.value.find('e') != std::string::npos)
            ret.code = 'y';
        else    ret.code = 'x';
    }
    else {
        assert(ret.value.size() == 1);
        ret.code = ret.value[0];
    }
    return ret;
}

std::string Compiler::strLexicalAnalysis() const {
    static std::string str = "";
    if(str != "")    return str;
    std::vector<Symbol> lexicalAnalysisResult = this->lexicalAnalysis();
    for(const auto &it: lexicalAnalysisResult)
        str += it.code;
    return str;
}

const char* Compiler::charLexicalAnalysis() const {                              // 这个函数有点危险，慎用！建议你还是用上面那个更安全点
    std::string *p = new std::string(this->strLexicalAnalysis());
    return p->c_str();
}

#endif // __COMPILER_H
