//

#include "stdafx.h"
#include<stdio.h>
#include<vector>
#include<string>
#include<fstream>
#include <iostream>
#include <string.h>        // for strcpy(), strcat()
#include <io.h>

using namespace std;

int char_number = 0; int line_number = 0; int word_number = 0;	//字符数 行数 单词数

void listFiles(const char * dir);

bool isenglish(char x);

typedef struct node {
	string key;
	int num;
	string best;
}Node;


int main()
{
	Node word[10000000];
	char dir[200];
	cout << "Enter a directory: ";
	cin.getline(dir, 200);

	listFiles(dir);
	printf("%d\n", char_number);
	printf("%d\n", line_number);
	system("pause");
	return 0;
}
void listFiles(const char * dir)
{
	char dirNew[200];
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1)        // 检查是否成功
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			//cout << findData.name << "\t<dir>\n";

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);

			listFiles(dirNew);
		}
		else
		{
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);
			ifstream myfile(dirNew);
			string temp;
			while (getline(myfile, temp))
			{
				for (int i = 0; temp[i] != NULL; i++)
						{
							if (temp[i] >= 32 && temp[i] <= 126)
								char_number++;
							if (temp[i] == 13 || temp[i] == 10)
								line_number++;
						//	if(isenglish(temp[i]))
						}
						line_number++;
			}
			myfile.close();
		}	

	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
}
bool isenglish(char x)
{
	if (x <= 122 && x >= 41) return 1;
	else return 0;
}

