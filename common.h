#ifndef  __COMMON_H
#define  __COMMON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>

#define  isUppercase(ch)        (((ch) >= 'A') && ((ch) <= 'Z'))
#define  isLowercase(ch)        (((ch) >= 'a') && ((ch) <= 'z') || ((ch) == '_'))
#define  isLetter(ch)           (isUppercase(ch) || isLowercase(ch))
#define  isDigit(ch)            (((ch) >= '0') && ((ch) <= '9'))
#define  isLetterOrDigit(ch)        (isLetter(ch) || isDigit(ch))

#define  For(i,s,t)  for(auto i = (s); i != (t); ++i)

#define  myout(dataStruct)  \
    template <typename T> \
    std::ostream& operator << (std::ostream &out, const dataStruct<T> &container) { \
        For(it, container.begin(), container.end()) \
            out << (*it); \
        return out; \
    }
myout(std::vector)
myout(std::set)


// 符号类，终结符 or 非终结符
class Symbol {
    friend std::ostream& operator << (std::ostream&, const Symbol &);
public:
    char code;
    std::string value;                 // 暂时用不到
    static const char NULCHAR;
    static const char ENDCHAR;
    static const char SPACE;
    static const std::string startSign;

    Symbol(char _code = NULCHAR): code(_code), value("") { }
    Symbol(char _code, const std::string &_value): code(_code), value(_value) { }
    bool isNUL() const {   return code == NULCHAR;   }          // 是否为空字符
    bool isNonEndChar() const {   return isNUL() == false && isUppercase(code);   }             // 是否为非终结符
    bool isEndChar() const {   return isNUL() == false && isUppercase(code) == false;   }        // 是否为终结符
    bool isNonValue() const {   return value == "";   }
    bool isStart() const {   return value == startSign;   }
    bool operator < (const Symbol &rhs) const {
//        if(code == rhs.code)    return value < rhs.value;
        return code < rhs.code;
    }
    bool operator == (const Symbol &rhs) const {
        return code == rhs.code;// && value == rhs.value;
    }
};

const char Symbol::NULCHAR = '@';
const char Symbol::ENDCHAR = '#';
const char Symbol::SPACE = ' ';
const std::string Symbol::startSign = "__STARTSYMBOL";
const Symbol ENDSYMBOL(Symbol::ENDCHAR), NULSYMBOL(Symbol::NULCHAR), SPACESYMBOL(Symbol::SPACE);

std::ostream& operator << (std::ostream &out, const Symbol &s) {
    out << s.code;
    if(s.isNonValue() == false)    out << "  " << s.value;
    return out;
}

#endif // __COMMON_H
