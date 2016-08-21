#include "compiler.h"

#define  CAT        (1 << 0)
#define  PERTREAT   (1 << 1)
#define  LEXICAL    (1 << 2)
#define  GRAMMAR    (1 << 3)

#define  add(Option, opt)     ((Option) |= (opt))

#define  isNone(opt)        ((opt) == 0)
#define  isCat(opt)         ((opt) & CAT)
#define  isPretreat(opt)    ((opt) & PERTREAT)
#define  isLexical(opt)     ((opt) & LEXICAL)
#define  isGrammar(opt)     ((opt) & GRAMMAR)


const int BUFLEN = 1024;

int main(int argc, char *argv[])
{
    auto prompt = []() {
        std::cout << "Usage: compiler [-option]... [file]\nPlease enter \"compiler -h\" for more help.\n";
        return 1;
    };
    if(argc == 1) {
        argv[1] = (char*)malloc(sizeof(char) * BUFLEN);
        argv[2] = (char*)malloc(sizeof(char) * BUFLEN);
        std::cin >> argv[1] >> argv[2];
        argc = 3;
    }
    if(argc == 2 && strcmp(argv[1], "-h") == 0) {
        std::cout << "用法: compiler [选项]... [文件]\n\
参数选项有如下5个：\n\
  -h      后面不可再接任何东西，包括文件名，向屏幕输出帮助信息\n\
  -c      查看文件内容\n\
  -p      输出对文本进行编译前的预处理结果\n\
  -l      输出对文本进行词法分析后的结果\n\
  -g      输出对文本进行文法分析后的结果\n\
除了 -h 外后4个参数选项可任意组合，如下面都是合法的：\n\
    compiler -h                    \n\
    compiler -c [file]             \n\
    compiler -c -p [file]          \n\
    compiler -p -l -g [file]       \n\
    compiler -cp -l [file]         \n\
    compiler -glp [file]           \n\
    ...... \n";
        return 0;
    }
    if(argv[argc - 1][0] == '-')    return prompt();

    int option = 0;
    For(i, 1, argc - 1) {
        if(argv[i][0] != '-')   return prompt();
        for(int j = 1; argv[i][j] != '\0'; ++j) {
            if(argv[i][j] == 'c')       add(option, CAT);
            else if(argv[i][j] == 'p')   add(option, PERTREAT);
            else if(argv[i][j] == 'l')   add(option, LEXICAL);
            else if(argv[i][j] == 'g')   add(option, GRAMMAR);
            else    return prompt();
        }
    }
    if(isNone(option))   add(option, GRAMMAR);

    Compiler cp(argv[argc - 1]);
    if(isCat(option)) {
        if(cp.cat() != 0)   return 2;
    }
    if(isPretreat(option))    std::cout << cp.pretreatment() << "\n";
    if(isLexical(option)) {
        const auto &lex = cp.lexicalAnalysis();
        std::cout << "词法正确！\n单词二元式如下：(code, value)\n";
        for(const auto &it: lex)
            std::cout << it << "\n";
    }
    if(isGrammar(option)) {
        if(cp.predictiveAnalysis() == true)   std::cout << "文法正确！\n";
    }
    return 0;
}
