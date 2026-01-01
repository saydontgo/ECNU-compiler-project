# ECNU编译原理大作业

## 环境配置
- linux ubuntu24.04
- g++版本13.3.0
- GNU Make 版本 4.3

## 如何编译
- 在项目文件夹下执行`cmake -S . -B build; cmake --build build`, 等待编译完成即可

## 10大亮点的验证方式如下：

- 1. 进入build文件夹，运行`./compiler 1`, 具体的测试输入可以查看test_data/project1/my_test3_所有错误形式.in, 里面给出了详细的错误解答
- 2. 进入build文件夹，运行`./compiler 2`, 具体的测试输入可以查看test_data/project1/my_test1_单引号与转义符.in, 里面给出了详细的错误解答
- 3. 分别查看src/下的token.cpp, error_reporter.cpp, symbol_table.cpp 的具体实现，include/下面有对应的头文件
- 4. 额外的测试用例可以在test_data的project1文件夹和project2文件夹中查看（my_test开头）
- 5. 进入build文件夹，运行`./compiler 5`, 具体的代码见src/ll1_analyzer.cpp中的BuildTable方法，运行后会打印分析表的结果。
- 6. 可以查看ll1_parser.h和slr1_parser.h查看各自的类定义和成员方法，其中使用了词法分析器的各个组件
- 7. 进入build文件夹，运行`./compiler 7 ../test_data/project2/my_test[指定的文件]`, 结果会打印抽象化前和抽象化后的语法树，用于参考。 
抽象语法树示例：  
如a = b + c;  
语法树化简后（没有哪些stmt之类的非终结符了）变为：  
=  
&emsp;a  
&emsp;+  
&emsp;&emsp;b  
&emsp;&emsp;c  
一个缩进表示一个层级，“=” 表示整个式子，而“+”号表示 “b + c”。  

- 8. 查看slr1_analzyer.cpp中的ComputeFirst,CreateSLRTable等方法。
- 9. 进入build文件夹，运行`./compiler 9`, 运行的结果就是可视化的结果（表相当的大，需要调整终端字体大小，不然格式可能不清晰）
- 10. 本项目的github仓库为[链接](https://github.com/saydontgo/ECNU-compiler-project), 上面可以查看开课以来的所有commit。