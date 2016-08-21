#compiler
一个小型编译器的前端部分，包含**词法分析**和**文法分析**<br>
词法分析使用的是经典的nfa转dfa算法<br>
文法分析使用的是LL(1)文法<br>
使用的是标准C++，没有用到任何外部库，因此在linux和windows下都可进行编译。<br>
运行结果截图：<br><br>
**编译和获取帮助信息：**
<a href="" target="_blank"><img src="./pictures/help.png"/></a><br>
**查看文件内容：**
<a href="" target="_blank"><img src="./pictures/cat.png"/></a><br>
**预处理程序：**
<a href="" target="_blank"><img src="./pictures/pretreatment.png"/></a><br>
**对程序进行词法分析后的结果：（部分截图）**
<a href="" target="_blank"><img src="./pictures/lexicalAnalysis.png"/></a><br>
**文法分析的LL(1)各种表信息：**
<a href="" target="_blank"><img src="./pictures/predictiveAnalysis_1.png"/></a><br>
<a href="" target="_blank"><img src="./pictures/predictiveAnalysis_2.png"/></a><br>
<a href="" target="_blank"><img src="./pictures/predictiveAnalysis_3.png"/></a><br><br>
输出格式可能稍微有点乱，已经尽可能调整过的了～
