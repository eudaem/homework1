#English
##About
This code finish the task to count the number of characters, words, phrases and analysis them.
You can see a more detailed report on my blog: [http://www.cnblogs.com/Franzkfk/](http://www.cnblogs.com/Franzkfk/)
Or you can visit my github: [https://github.com/LiuzeGit/WordFrequency](https://github.com/LiuzeGit/WordFrequency)
##To Build
###Windows
In Windows, make sure you have preprocessor macro: `WIN32`,and use `C++11`
###Linux
In Linux, make sure you have preprocessor macro: `__linux__`,and use`C++11`like this:
```bash
g++ -std=c++11 WordFrequency.cpp
```
My environment: `Ubuntu 16.04LTS`,`g++ 5.40`
##To Run
###Windows
You can run like this:
```bash
WordFrequency.exe input-path [output-path]
```
###Linux
You can run like this:
```bash
WordFrequency input-path [output-path]
```
#中文
##简介
这份代码完成了对一个目录下所有文件或者单个文件中字符、单词、词组频率统计。
具体要求以及实验过程可以参考博客: [http://www.cnblogs.com/Franzkfk/](http://www.cnblogs.com/Franzkfk/)
代码参考我的github：[https://github.com/LiuzeGit/WordFrequency](https://github.com/LiuzeGit/WordFrequency)
##生成
###Windows
在Windows环境下,请确认编译器有预编译宏：`WIN32`，并使用`C++11`
###Linux
在Liunx环境下，请同时保证预编译宏：`__linux__`,以及使用`C++11`:
```bash
g++ -std=c++11 WordFrequency.cpp
```
本人的环境是：`Ubuntu 16.04LTS`,`g++ 5.40`
##运行
###Windows
假设生成可执行程序为：`WordFrequency.exe`
则运行：
```bash
WordFrequency.exe input-path [output-path]
```
###Linux
假设生成可执行程序为：`WordFrequency`
则运行：
```bash
WordFrequency input-path [output-path]
```


