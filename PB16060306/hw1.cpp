#include "stdafx.h"
#include<stdio.h>
#include<vector>
#include<string>
#include<fstream>
#include <iostream>
#include <string.h>        // for strcpy(), strcat()
#include <io.h>

using namespace std;

typedef struct BiTNode {
	char best[58];
	int times;
	int isin = 0;
	struct BiTNode *lchild, *rchild;
}BiTNode, *BiTree;

typedef struct most {
	char name[58];
	int times;
}most;

most word[10];

int char_number = 0; int line_number = 0; int word_number = 0;	//字符数 行数 单词数

void listFiles(const char * dir);


bool isenglish(char x);


bool keycmp(char *s1, char *s2);

void change(char *s);

void addtree(BiTree s, BiTree &T);


BiTree T=NULL;

int main(int argc, char **argv)
{
	for (int i = 0; i < 10; i++)
	{
		word[i].times = 0;
	}
	char dir[200];

	strcpy(dir, argv[1]);
	char dirname[200];
	strcpy(dirname, dir);
	strcat(dirname, "\\Result.txt");
	ofstream mycout(dirname);
	listFiles(dir);
	mycout << "char_number :" << char_number << endl;
//	printf("%d\n", char_number);
	mycout << "line_number :" << line_number << endl;
//	printf("%d\n", line_number);
	mycout << "word_number :" << word_number << endl;
//	cout << word_number << endl;
	for (int i = 9; i >= 0; i--)
	{
		mycout << word[i].name << '\t' << word[i].times << endl;
		//		cout << word[i].name << '\t' << word[i].times << endl;
	}
//	while (L->next != NULL)
//	{
//		cout << L->next->key << endl;
//		L = L->next;
//	}
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
						}
						line_number++;
						int flag = 0;
						for (int j = 0; temp[j] != NULL; j++)
						{
							if (isenglish(temp[j]))
							{
								char p[58];
								p[flag] = temp[j];
								flag++;
								int t = j+1;
								for (;; t++)
								{
									if (isenglish(temp[t]))
									{
										p[flag] = temp[t];
										flag++;
									}
									else
									{
										for (; temp[t] >= 48 && temp[t] <= 57; t++);
										if (isenglish(temp[t]))
											word_number++;
										p[flag] = '\0';
										break;
									}
								}
								if (flag >= 4)	//说明是英语单词
								{
									//cout << p << endl;
									char s1[58];
									strcpy(s1, p);
									BiTree n;
									n = new BiTNode;
									strcpy(n->best, s1);
									n->times = 1;
									n->lchild = NULL;
									n->rchild = NULL;
									addtree(n, T);
									for (int i = 0; p[i] != '\0'; i++)
									{
										p[i] = '\0';
									}
									j = t-1;
									flag = 0;
									continue;
								}
								else  //说明不是英语单词
								{
									for (int i = 0; p[i] != '\0'; i++)
									{
										p[i] = '\0';
									}
									j = t-1;
									flag = 0;
									continue;
								}
							}
						}
			}
			myfile.close();
		}	

	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
}
bool isenglish(char x)
{
	if (x <= 90 && x >= 65) return 1;
	else if (x >= 97 && x <= 122) return 1;
	else return 0;
}

void change(char *s)
{
	for (int i = 0; s[i] != NULL; i++)
	{
		if (s[i] >= 65 && s[i] <= 90)
			s[i] = s[i] + 32;
	}
}
bool keycmp(char *s1, char *s2)
{
	for (int i = 0; s1[i] != NULL; i++)
	{
		if (s1[i]>s2[i])
		{
			return 1;
		}
		if (s1[i] == s2[i])
		{
			continue;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}	//比较两单词的顺序，best是s1则返回0，否则返回1； 

void order(most *word)
{
	most n;
	for (int i = 0; i < 10; i++)
	{
		for (int j = i; j < 9; j++)
		{
			if (word[j].times > word[j + 1].times)
			{
				n = word[j + 1];
				word[j + 1] = word[j];
				word[j] = n;
			}
		}
	}
}

void addtree(BiTree n, BiTree &T)
{
	char key1[58];
	char key2[58];
	strcpy(key2, n->best);
	change(key2);
	int i;
	if (T == NULL)
	{
		T = n;
		if (T->times > word[0].times)
		{
				word[0].times = T->times;
				strcpy(word[0].name, T->best);
				order(word);
		}
		word_number++;
	}
	else
	{
		strcpy(key1, T->best);
		change(key1);
		if (strcmp(key1, key2) > 0)
			addtree(n, T->lchild);
		else if (strcmp(key1, key2) < 0)
			addtree(n, T->rchild);
		else
		{
			T->times++;
			if (T->times > word[0].times)
			{
				for (i = 0; i < 10; i++)
				{
					if (strcmp(word[i].name, T->best) == 0)
					{
						word[i].times++;
						order(word);
						break;
					}
				}
				if (i == 10)
				{
					word[0].times = T->times;
					strcpy(word[0].name, T->best);
					order(word);
				}
			}
			word_number++;
			return;
		}
	}
}

