
#include "stdafx.h"
#include <io.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <cctype>
using namespace std;

#define ASCNUMB 128 //number of ASCII
#define linefeed 10
#define carriagereturn 13
#define space 32
bool transfer(string fileName, int exeNum); //useless
void dfsFolder(string folderPath, ofstream &fout); //useless
void countascword(string inputfilename,int* asccount,int* linecount,unordered_map<string, size_t> word_count);
//count the number of ASCII and word,use map

int main()
{
	_finddata_t file;
	string line;
	int asccount[1] = { 0 };
	int linecount[1] = { 0 };
	unordered_map<string, size_t> word_count;
	//string fileName = "D:\\TestingSamples\\newsample\\*.txt";
	//long handle = _findfirst(fileName.c_str(), &file);
	ifstream fstrm("D:\\TestingSamples\\newsample\\01.txt"); //test path
	if (fstrm)
	{
		countascword("D:\\TestingSamples\\newsample\\01.txt",asccount,linecount,word_count);
		cout << *asccount << " " << *linecount << endl;
		/*while (getline(fstrm, line))
		{// line中不包括每行的换行符  
			for (int index = 0;line[index]; index++)
			{
				word = line[index];
				++word_count[word];
			}
		}
		for (const auto &w : word_count)
		{
			cout << w.first << ":" << w.second << endl;
		}*/
	}
	else cout << "fail" << endl;
	_findclose(handle);

	//transfer("D:\\TestingSamples\\*.txt", 0);
	//ofstream o_fstream;

	//dfsFolder("D:\\TestingSamples", o_fstream);


	return 0;
}

//_findfirst 函数返回的是匹配到文件的句柄，数据类型为long。
//遍历过程可以指定文件类型，这通过FileName的赋值来实现，例如要遍历C : \WINDOWS下的所有.exe文件

bool transfer(string fileName, int exeNum)
{
	_finddata_t fileInfo;
	long handle = _findfirst(fileName.c_str(), &fileInfo);

	if (handle == -1L)
	{
		cerr << "failed to transfer files" << endl;
		return false;
	}

	do
	{
		exeNum++;
		cout << fileInfo.name << endl;
	} while (_findnext(handle, &fileInfo) == 0);
	cout << " .exe files' number:  " << exeNum << endl;

	return true;
}

//遍历文件夹及其子文件夹下所有文件。操作系统中文件夹目录是树状结构，使用深度搜索策略遍历所有文件。用到_A_SUBDIR属性


//在判断有无子目录的if分支中，由于系统在进入一个子目录时，匹配到的头两个文件(夹)是"."(当前目录)，".."(上一层目录)。
//需要忽略掉这两种情况。当需要对遍历到的文件做处理时，在else分支中添加相应的代码就好

void dfsFolder(string folderPath, ofstream &fout)
{
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do {
		//判断是否有子目录  
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//这个语句很重要  
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath, fout);
			}
		}
		else
		{
			fout << folderPath.c_str() << "\\" << FileInfo.name << " ";
			cout << folderPath.c_str() << "\\" << FileInfo.name << endl;
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);
	fout.close();
}

void countascword(string inputfilename, int* asccount,int* linecount,unordered_map<string, size_t> word_count)
{
	int countasc = 0;
	int countword = 0;
	int countline = 0;
	int ascint = 0; //ASCII to int
	int start = 0; //a word begin
	int end = 0; // a word end
	int index;
	string mostoccur[10] = { 0 };
	int mosttimes[10] = { 0 };
	ifstream inputfile(inputfilename);
	string fileline;
	if (inputfile)
	{
		while (getline(inputfile, fileline))
		{
			string tempfileline = fileline + " ";
			end = start = 0;
			for (index = 0; ascint = tempfileline[index]; index++)
			{
				++countasc;
				if (!isalnum(ascint))
				{
					end = index;
					int temp = end - start;
					if (temp > 0)
					{
						string word(tempfileline, start, end - start);
						++word_count[word];
					}
					start = end + 1;
				}
			}
			string word(tempfileline, start);
			++word_count[word];
			countasc += 2;
			countline++;
		}
		*asccount = countasc;
		*linecount = countline;
		for (const auto &w : word_count)
		{
			if (w.second > mosttimes[9]) 
			{//BubbleSort
				mosttimes[9] = w.second;
				for (int sort = 8; sort >= 0; sort++)
				{
					if (mosttimes[sort] < mosttimes[sort + 1])
					{
						int temp = mosttimes[sort + 1];
						string tempstring;
						strcpy(tempstring, mostoccur[sort]);
						mosttimes[sort + 1] = mosttimes[sort];
						mosttimes[sort] = temp;
					}
				}
			}
		}
	}
	else cout << "read fail" << endl;
	return;
}
