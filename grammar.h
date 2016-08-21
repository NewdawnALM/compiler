#ifndef  __HEADER_H
#define  __HEADER_H

#include "common.h"

// 产生式类
class Production {
    friend std::ostream& operator << (std::ostream&, const Production &);
public:
    Symbol left;                     // 产生式的左部分
    std::vector<Symbol> right;      // 产生式的右部分
    Production() { }
    Production(const Symbol &_left, const std::vector<Symbol> &_right): left(_left), right(_right) { }
    bool operator < (const Production &rhs) const {
        if(left == rhs.left)    return right < rhs.right;
        return left < rhs.left;
    }
};

std::ostream& operator << (std::ostream &out, const Production &p) {
    out << p.left << " -> ";
    return (out << p.right);
}


// 使用预测分析法的类
class PredictiveAnalysis {
    friend std::ostream& operator << (std::ostream&, const PredictiveAnalysis &);
public:
    const std::vector<Production> grammar;              // 由一系列产生式组成的语法
    std::map<Symbol, std::set<Symbol>> SymbolFirst;               // 文法符号 First 集
    std::map<Production, std::set<Symbol>> CandidateFirst;        // 候选式 First 集
    std::map<Symbol, std::set<Symbol>> NonEndFollow;              // 非终结符 Follow 集
    std::map<Symbol, std::map<Symbol, int>> table;                // LL(1)分析表（预测分析表）
    PredictiveAnalysis(const std::vector<Production> &_grammar): grammar(_grammar) {   calculate();   }
    ssize_t size() const {   return grammar.size();   }
    void showSymbolFirst() const;
    void showCandidateFirst() const;
    void showNonEndFollow() const;
    void showTable(bool) const;
    void showAll(bool) const;
    static const int errorSign;         // 预测分析表中的错误标记，初始化为-1
    Symbol startSymbol;                 // 文法的开始符号
private:
    std::map<Symbol, std::set<Production>> productionSet;       // 非终结符和其对应的产生式的集合
    void calculate();
    void calculateSymbolFirst();            // 计算文法符号 First 集
    void calculateCandidateFirst();         // 计算候选式 First 集
    void calculateNonEndFollow();           // 计算非终结符 Follow 集
    void calculateTable();                  // 根据以上结构构造 LL(1)分析表（预测分析表）
};

const int PredictiveAnalysis::errorSign = -1;

std::ostream& operator << (std::ostream &out, const PredictiveAnalysis &pre) {
    return out << pre.grammar;
}

void PredictiveAnalysis::calculateSymbolFirst() {
    std::map<Symbol, bool> isFinished;                          // 该 Symbol 的 First 是否已计算好
    For(it, grammar.begin(), grammar.end()) {
        productionSet[it->left].insert(*it);
        isFinished[it->left] = false;
    }
    std::function<std::set<Symbol>(const Symbol &)> firstOfSymbol = [&] (const Symbol &s) -> std::set<Symbol> {
        if(isFinished[s])    return SymbolFirst[s];
        For(sit, productionSet[s].begin(), productionSet[s].end()) {
            assert(sit->right.size() > 0);
            if(sit->right[0].isNUL() || sit->right[0].isEndChar())
                SymbolFirst[s].insert(sit->right[0]);
            else if(sit->right[0].isNonEndChar()) {
                For(vit, sit->right.begin(), sit->right.end()) {
                    const auto &y = firstOfSymbol(sit->right[0]);
                    SymbolFirst[s].insert(y.begin(), y.end());
                    if(y.count(NULSYMBOL) == 0) {
                        SymbolFirst[s].erase(NULSYMBOL);
                        break;
                    }
                }
            }
        }
        isFinished[s] = true;
        return SymbolFirst[s];
    };
    For(mit, productionSet.begin(), productionSet.end()) {
        firstOfSymbol(mit->first);
    }
    For(vit, grammar.begin(), grammar.end()) {
        For(it, vit->right.begin(), vit->right.end())
            if(it->isEndChar())    SymbolFirst[*it].insert(*it);
    }
}

void PredictiveAnalysis::calculateCandidateFirst() {
    For(it, grammar.begin(), grammar.end()) {
        assert(it->right.size() > 0);
        if(it->right[0].isEndChar() || it->right[0].isNUL())
            CandidateFirst[*it] = std::set<Symbol>{it->right[0]};
        else if(it->right[0].isNonEndChar())    CandidateFirst[*it] = SymbolFirst[it->right[0]];
    }
}

void PredictiveAnalysis::calculateNonEndFollow() {
    For(mit, productionSet.begin(), productionSet.end()) {
        if(mit->first.isStart()) {
            NonEndFollow[mit->first].insert(ENDSYMBOL);
            this->startSymbol = mit->first;
            break;
        }
    }
    For(git, grammar.begin(), grammar.end()) {
        if(git->right.size() < 2)   continue;
        const auto &A = git->left;
        For(vit, git->right.begin(), git->right.end() - 1) {
            if(vit->isNonEndChar()) {
                const auto &beta = *(vit + 1);
                if(beta.isNUL())    continue;
                if(beta.isEndChar())    NonEndFollow[*vit].insert(beta);
                else if(beta.isNonEndChar()) {
                    NonEndFollow[*vit].insert(SymbolFirst[beta].begin(), SymbolFirst[beta].end());
                    NonEndFollow[*vit].erase(NULSYMBOL);
                }
            }
        }
    }
    std::map<Symbol, std::set<Symbol>> directedGraph;
    auto leadToNUL = [&](const Symbol &s) -> bool {
        if(s.isNUL())   return true;
        else if(s.isEndChar())   return false;
        else if(s.isNonEndChar())    return SymbolFirst[s].count(NULSYMBOL) != 0;
    };
    For(git, grammar.begin(), grammar.end()) {
        assert(git->right.size() > 0);
        if((git->right.end() - 1)->isNonEndChar() && (!(git->left == *(git->right.end() - 1))) ) {
            directedGraph[git->left].insert(*(git->right.end() - 1));
        }
        if(git->right.size() < 2)    continue;
        for(auto vit = git->right.end() - 2; vit >= git->right.begin(); --vit) {
            if(vit->isEndChar())    break;
            else if(vit->isNonEndChar() && leadToNUL(*(vit + 1)) == true) {
                if(!(git->left == *vit))    directedGraph[git->left].insert(*vit);
                if(leadToNUL(*vit) == false)    break;
            }
        }
    }
    std::map<Symbol, bool> outQueue, inQueue, canDiscard;
    std::queue<Symbol> symbolQueue;
    auto pushQueue = [&](const Symbol &s) -> void {
        symbolQueue.push(s);
        while(!symbolQueue.empty()) {
            Symbol p = symbolQueue.front();
            symbolQueue.pop();
            if(canDiscard[p] == true) {
                canDiscard[p] = false;
                continue;
            }
            outQueue[p] = true;
            inQueue[p] = false;
            For(sit, directedGraph[p].begin(), directedGraph[p].end()) {
                if(outQueue[*sit] == false) {
                    NonEndFollow[*sit].insert(NonEndFollow[p].begin(), NonEndFollow[p].end());
                    symbolQueue.push(*sit);
                    if(inQueue[*sit] == true)   canDiscard[*sit] = true;
                    inQueue[*sit] = true;
                }
                else if(outQueue[*sit] == true) {
                    const auto old = NonEndFollow[*sit];
                    NonEndFollow[*sit].insert(NonEndFollow[p].begin(), NonEndFollow[p].end());
                    if(NonEndFollow[*sit] == old)   continue;
                    symbolQueue.push(*sit);
                    if(inQueue[*sit] == true)   canDiscard[*sit] = true;
                    inQueue[*sit] = true;
                }
            }
        }
    };
    For(dit, directedGraph.begin(), directedGraph.end()) {
        outQueue[dit->first] = false;
        inQueue[dit->first] = false;
        canDiscard[dit->first] = false;
    }
    For(dit, directedGraph.begin(), directedGraph.end()) {
        if(outQueue[dit->first] == false)   pushQueue(dit->first);
    }
}

void PredictiveAnalysis::calculateTable() {
    For(bit, SymbolFirst.begin(), SymbolFirst.end()) {
        if(bit->first.isNonEndChar() == false)   continue;
        For(lit, SymbolFirst.begin(), SymbolFirst.end()) {
            if(lit->first.isEndChar() == false)    continue;
            table[bit->first][lit->first] = errorSign;
        }
        table[bit->first][ENDSYMBOL] = errorSign;
    }
    auto addToTable = [&](const Symbol &r, const Symbol &c, int index) -> void {
        static const std::vector<Symbol> onlyNUL{ NULSYMBOL };
        if(table[r][c] == errorSign)    table[r][c] = index;
        else {
            if(grammar[table[r][c]].right == onlyNUL)   table[r][c] = index;
        }
    };

    For(vit, grammar.begin(), grammar.end()) {
        const auto A = vit->left;
        const auto AlphaFirst = CandidateFirst[*vit];
        For(a, AlphaFirst.begin(), AlphaFirst.end()) {
            if(a->isEndChar())   addToTable(A, *a, vit - grammar.begin());
            else if(a->isNUL()) {
                For(b, NonEndFollow[A].begin(), NonEndFollow[A].end())
                    addToTable(A, *b, vit - grammar.begin());
            }
        }
    }
}

void PredictiveAnalysis::calculate() {
    calculateSymbolFirst();
    calculateCandidateFirst();
    calculateNonEndFollow();
    calculateTable();
}

void PredictiveAnalysis::showSymbolFirst() const {
    For(mit, SymbolFirst.begin(), SymbolFirst.end())
        std::cout << mit->first << " : " << mit->second << "\n";
}

void PredictiveAnalysis::showCandidateFirst() const {
    For(mit, CandidateFirst.begin(), CandidateFirst.end())
        std::cout << mit->first << " : " << mit->second << "\n";
}

void PredictiveAnalysis::showNonEndFollow() const {
    For(mit, NonEndFollow.begin(), NonEndFollow.end())
        std::cout << mit->first << " : " << mit->second << "\n";
}

void PredictiveAnalysis::showTable(bool simple = true) const {
    std::cout << Symbol('\\');
    For(cit, table.begin()->second.begin(), table.begin()->second.end())
        std::cout << '\t' << cit->first;
    std::cout << std::endl;
    For(rit, table.begin(), table.end()) {
        std::cout << rit->first;
        For(cit, rit->second.begin(), rit->second.end()) {
            if(cit->second != errorSign) {
                std::cout << '\t';
                if(simple == true)     std::cout << cit->second;
                else    std::cout << grammar[cit->second];
            }
            else    std::cout << '\t';
        }
        std::cout << std::endl;
    }
}

void PredictiveAnalysis::showAll(bool simple = true) const {
    std::cout << "文法符号 First 集：\n";
    showSymbolFirst();
    std::cout << "\n候选式 First 集：\n";
    showCandidateFirst();
    std::cout << "\n非终结符 Follow 集：\n";
    showNonEndFollow();
    std::cout << "\nLL(1)分析表（预测分析表）：\n";
    showTable(simple);
    std::cout << "\n";
}

#endif // __HEADER_H
