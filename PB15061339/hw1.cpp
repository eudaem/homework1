// char count.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include <cstring>
#include <string>
#include <windows.h>
#include <unordered_map>
#include <time.h>
#include <fstream>  

using namespace std;

int getcharnum(char *filename);

void display(unordered_map<string, double> mmp, string str); //unorder_map输出

void Result(); //输出

void BubbleSort();	//冒泡排序

void listFiles(const char * dir);	//遍历文件夹

int total_char_num = 0;

int total_line_num = 0;

int total_word_num = 0;

int tmp_num = 0;

int i = 0;	//单词数

struct dictionary          
{
	string topword;
	int num = 0;
}*d, *p;

unordered_map<string, double> dic;
unordered_map<string, double> phr;



int main(int argc, char *argv[])
{
	clock_t start = clock();
	int num;
	listFiles(argv[1]);
	BubbleSort();
	clock_t finish = clock();
	Result();
	double consumeTime = (double)(finish - start) / CLOCKS_PER_SEC;//注意转换为double的位置
	cout << "\nTime:" << consumeTime << "second！" << endl;
	system("pause");
	return 0;
}

void Result()
{
	ofstream outfile("Result.txt");//声明一个写文件流对象outfile并打开文件
	if (!outfile)
	{
		cout << "Failed to create file...\n";
		exit(0);
	}
	outfile << "char_number:" << total_char_num << endl;
	outfile << "line_number:" << total_line_num << endl;
	outfile << "word_number:" << total_word_num << endl;
	outfile << "\nthe top ten frequency of word :\n";
	for (int k = 0; k < 10; k++)
	{
		outfile << (d + k)->topword << "\t" << (d + k)->num << endl;
	}
	outfile << "\n\nthe top ten frequency of phrase :\n";
	for (int k = 0; k < 10; k++)
	{
		outfile << (p + k)->topword << "\t" << (p + k)->num << endl;
	}
	outfile.close();
	cout << "char_num:" << total_char_num << endl;
	cout << "line_num:" << total_line_num << endl;
	cout << "word_num:" << total_word_num << endl;
	cout << "the top ten frequency of word :" << endl;
	for (int k = 0; k < 10; k++)
	{
		cout << (d + k)->topword << ":" << (d + k)->num << endl;
	}
	cout << "the top ten frequency of phrase :" << endl;
	for (int k = 0; k < 10; k++)
	{
		cout << (p + k)->topword << ":" << (p + k)->num << endl;
	}
}

void BubbleSort()
{
	d = new dictionary[10];
	p = new dictionary[10];
	if (d == NULL)
	{
		cout << "Error: out of memory." << endl;
		exit(1);
	}
	if (p == NULL)
	{
		cout << "Error: out of memory." << endl;
		exit(1);
	}
	int k;
	for (auto& x : dic)
	{
		if (x.second < (d + 9)->num)
			continue;
		for (k = 8; k >= 0; k--)
		{
			if (x.second >(d + k)->num)
			{
				(d + k + 1)->num = (d + k)->num;
				(d + k + 1)->topword = (d + k)->topword;
				if (k == 0)
				{
					(d + k)->num = x.second;
					(d + k)->topword = x.first;
				}
			}
			else
			{
				(d + k + 1)->num = x.second;
				(d + k + 1)->topword = x.first;
				break;
			}
		}
	}
	for (auto& x : phr)
	{
		if (x.second < (p + 9)->num)
			continue;
		for (k = 8; k >= 0; k--)
		{
			if (x.second >(p + k)->num)
			{
				(p + k + 1)->num = (p + k)->num;
				(p + k + 1)->topword = (p + k)->topword;
				if (k == 0)
				{
					(p + k)->num = x.second;
					(p + k)->topword = x.first;
				}
			}
			else
			{
				(p + k + 1)->num = x.second;
				(p + k + 1)->topword = x.first;
				break;
			}
		}
	}
}


void listFiles(const char * dir)
{
	using namespace std;
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	LARGE_INTEGER size;
	hFind = FindFirstFile(dir, &findData);
	if (findData.dwFileAttributes != 16)
	{
		cout << "非目录路径" << endl;
		system("pause");
		exit(0);
	}
	char dirNew[100];

	// 向目录加通配符，用于搜索第一个文件 
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");

	hFind = FindFirstFile(dirNew, &findData);
	do
	{
		// 是否是文件夹，并且名称不为"."或".." 
		if ((findData.dwFileAttributes == 16)
			&& (strcmp(findData.cFileName, ".") != 0)
			&& (strcmp(findData.cFileName, "..") != 0)
			)
		{
			// 将dirNew设置为搜索到的目录，并进行下一轮搜索 
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			listFiles(dirNew);
		}
		else if ((strcmp(findData.cFileName, ".") != 0)
			&& (strcmp(findData.cFileName, "..") != 0)
			)
		{
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.cFileName);
			tmp_num = getcharnum(dirNew);
			total_char_num += tmp_num;
		}
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
}





void display(unordered_map<string, double> mmp, string str)
{
	cout << str << endl;
	for (auto& x : mmp)
		cout << x.first << ": " << "\t" << x.second << endl;
	cout << endl;
}

int getcharnum(char *filename)
{
	FILE *fp = NULL;
	string buf;
	string buff;
	int p = 0;
	int j = 0;
	int m; //word最后一个字母位置
	int flag = 1;
	char c;
	int num = 0;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		perror(filename);
		return NULL;
	}
	c = fgetc(fp);
	while (c != EOF)
	{
		if (c == '\n')
		{
			total_line_num++;
		}
		if (c > 31)
			if (c < 127)
				num++;
		if (c >= 65 && c <= 90 || c >= 97 && c <= 122)
		{
			if (c >= 97 && c <= 122)
				c -= 32;
			buf += c;
			j++;
			m = j;
			flag = 0;
		}
		else if (!(c >= 65 && c <= 90 || c >= 97 && c <= 122) && flag == 0)
		{
			if (j > 3)
			{
				if (!(c >= 48 && c <= 57))   // 单词完结
				{
					buf.assign(buf, 0, m);
					if (p == 0)
					{
						buff = buf;  //放入词组第一个
						buff += " ";
						p = 1;
					}
					else
					{
						buff += buf;
						unordered_map<string, double>::const_iterator got = phr.find(buff);
						if (got == phr.end())
						{
							pair<string, double> mybuf(buff, 1);
							phr.insert(mybuf);
						}
						else
						{
							int tempc = got->second + 1;
							phr.at(buff) = tempc;
						}
						buff.clear();
						buff = buf;  //放入词组第一个
						buff += " ";
					}
					unordered_map<string, double>::const_iterator got = dic.find(buf);
					if (got == dic.end())
					{
						pair<string, double> mybuf(buf, 1);
						dic.insert(mybuf);
						i++;
					}
					else
					{
						int tempc = got->second + 1;
						dic.at(buf) = tempc;
					}
					total_word_num++;
					j = 0;
					buf.clear();
					flag = 1;
				}
				else
				{
					buf += c;
					j++;
					flag = 0;
				}
			}
			else
			{
				j = 0;
				buf.clear();
				flag = 1;
			}
		}
		c = fgetc(fp);
	}
	fclose(fp);
	total_line_num++;
	return num;
}

