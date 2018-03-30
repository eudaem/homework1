#include"stdio.h"
#include"string.h"
#define puts(x)  fputs(x, stdout)
#include <iostream>  
#include <vector>  
#include <cstring>        // for strcat()  
#include <io.h>
#include <stdlib.h>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#define speed 5
using namespace std;
long int linenum = 0, charnum = 0;
long int num = 0;
long int thatnum = 0;
typedef struct wordnode {
	char keyword[500];
	unsigned int wtime;
	char word[500];
	struct wordnode*next;
}wordnode, *wordlist;
wordlist head[26][26][26][26][speed], head2[26][26][26][26][speed];
struct stat st;
void initwordnode()
{
	int i, j, k, l, o;
	for (i = 0; i<26; i++) 
	{ 
		for (j = 0; j<26; j++)
		{
			for (k = 0; k<26; k++)
			{
				for (l = 0; l<26; l++) 
				{
					for (o = 0; o<speed; o++) 
					{
						head[i][j][k][l][o] = new wordnode;
						head2[i][j][k][l][o] = new wordnode;
						head[i][j][k][l][o]->wtime = 0;
						head2[i][j][k][l][o]->wtime = 0;
						head[i][j][k][l][o]->next = NULL;
						head2[i][j][k][l][o]->next = NULL;
					}
				}
			}
		}
	}

}
int judge(wordlist p, wordlist order3[], int i)
{
	int j;
	for (j = 0; j <= i; j++)
	{
		if (strcmp(p->keyword, order3[j]->keyword) == 0)
			return(1);
	}
	return(0);
}
int ischar(char a)
{
	if ((a >= 'a'&&a <= 'z') || (a >= 'A'&&a <= 'Z'))
		return(1);
	else return(0);
}
int isnum(char a)
{
	if (a >= '0'&&a <= '9')
		return(1);
	else
		return(0);
}
void wordnum(char a[], int length)
{
	int i = 0, j = 0, k = 0, m, e, f;
	int l = 0;
	int flag = 0;
	wordnode *n, *p;
	char b[1024], c[1024], d[1024],ppp[1024];
	d[0] = '\0';
	for (i = 0; i<length - 3; i++)
	{
		j = i;
		l = i;
		if (ischar(a[i]) && ischar(a[i + 1]) && ischar(a[i + 2]) && ischar(a[i + 3]))
		{
			num++;
			i = i + 4;
			while (ischar(a[i]) || isnum(a[i]))
			{
				if (isnum(a[i]))
					flag++;
				if (ischar(a[i]))
					flag = 0;
				i++;
			}
			//跳出循环 储存单词
			for (k = 0; j<i; j++, k++)
			{
				b[k] = a[j];
				b[k + 1] = '\0';
			}
			for (k = 0; l<i - flag; l++, k++)
			{
				c[k] = a[l];
				if (c[k] <= 'Z'&&c[k] >= 'A')
					c[k] = c[k] - 'A' + 'a';
				c[k + 1] = '\0';
				//变小写
			}
			for (p = head[c[0] - 97][c[1] - 97][c[2] - 97][c[3] - 97][strlen(c) % speed]; p != NULL; p = p->next)
			{
				if (strcmp(p->keyword, c) == 0)
				{
					if (strcmp(p->word, b)>0)
					{
						strcpy(p->word, b);
					}
					p->wtime++; break;
				}
			}
			if (p == NULL)
			{
				n = new wordnode;
				strcpy(n->keyword, c);
				strcpy(n->word, b);
				n->wtime = 1;
				n->next = NULL;
				p = head[c[0] - 97][c[1] - 97][c[2] - 97][c[3] - 97][strlen(c) % speed]->next;
				head[c[0] - 97][c[1] - 97][c[2] - 97][c[3] - 97][strlen(c) % speed]->next = n;
				n->next = p;
			}
			if (strlen(d) != 0)
			{
				e = strlen(d); f = strlen(c);
				d[e] = ' ';
				for (m = e + 1; m <= e + f + 1; m++)
				{
					d[m] = c[m - e - 1];
				}
				d[m] = '\0';
				e = strlen(ppp); f = strlen(b);
				ppp[e] = ' ';
				for (m = e + 1; m <= e + f + 1; m++)
				{
					ppp[m] = b[m - e - 1];
				}
				ppp[m] = '\0';
				for (p = head2[d[0] - 97][d[1] - 97][d[2] - 97][d[3] - 97][strlen(d) % speed]->next; p != NULL; p = p->next)
				{
					if (strcmp(p->keyword, d) == 0)
					{
						if (strcmp(ppp, p->word) < 0) 
						{
							strcpy(p->word, ppp);
						}
						p->wtime++;
						break;
					}
				}
				if (p == NULL)
				{
					n = new wordnode;
					strcpy(n->keyword, d);
					strcpy(n->word, ppp);
					n->wtime = 1;
					n->next = NULL;
					p = head2[d[0] - 97][d[1] - 97][d[2] - 97][d[3] - 97][strlen(d) % speed]->next;
					head2[d[0] - 97][d[1] - 97][d[2] - 97][d[3] - 97][strlen(d) % speed]->next = n;
					n->next = p;
				}
			}
			strcpy(d, c);
			strcpy(ppp, b);
		}
		else if (isnum(a[i]))
		{
			while (ischar(a[i] || isnum(a[i])))
				i++;
		}
		flag = 0;
	}
}
vector<char*> getFilesList(const char * dir)
{
	vector<char*> allPath;
	char dirNew[1024];
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");
	intptr_t handle;
	_finddata_t findData;
	handle = _findfirst(dirNew, &findData);
	if (handle == -1)
	{
		strcpy(dirNew, dir);
		handle = _findfirst(dirNew, &findData);
		if (-1 == handle)
		{
			cout << "can not found the file ... " << endl;
			return allPath;
		}
		else
		{
			allPath.push_back(const_cast <char*>(dir));
			return  allPath;
		}
	}
	do
	{
		if (findData.attrib & _A_SUBDIR)//// ???????  
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);
			vector<char*> tempPath = getFilesList(dirNew);
			allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());
		}
		else
		{
			char *filePath = new char[200];
			strcpy(filePath, dir);
			strcat(filePath, "\\");
			strcat(filePath, findData.name);
			allPath.push_back(filePath);


		}
	} while (_findnext(handle, &findData) == 0);
	_findclose(handle);
	return allPath;
}
int main(int argc,char *argv[])
{
	char *a;
	char *perPath;
	int lsize, e, b, c, d, j = 0;
	char dir[1024];
	FILE *fin, *pFile,*fout;
	fout = fopen("C:\\Users\\asus\\Desktop\\haha\\result.txt", "w");
	int i = 0;
	wordlist order[10], order2[10];
	for (i = 0; i<10; i++)
	{
		order[i] = new wordnode;
		order[i]->wtime = 0;
		order2[i] = new wordnode;
		order2[i]->wtime = 0;
	}
	i = 0;
	int length = 0;
	initwordnode();
	wordlist p, q; 
	p = new wordnode;
	p->next = NULL;
	q = new wordnode;
	q->next = NULL;
	/*cout << "Enter a directory: ";
	cin.getline(dir, 1024);*/
	strcpy(dir, argv[1]);
	vector<char*>allPath = getFilesList(dir);
	for (size_t i = 0; i < allPath.size(); i++)
	{
		perPath = allPath.at(i);
		fin = fopen(perPath, "r");
		stat(perPath, &st);
		lsize = st.st_size;
		a = new char[lsize + 1];
		for (j = 0; j<lsize; j++)
		{
			a[j] = fgetc(fin);
			if (a[j] >= 32 && a[j] <= 126)
				charnum++;
			if (a[j] == '\n')
				linenum++;
		}
		rewind(fin);
		a[j] = '\0';
		length = strlen(a);
		wordnum(a, length);
		delete(a);
		fclose(fin);
	}
	for (i = 0; i<10; i++)
	{
		for (e = 0; e<26; e++) 
		{
			for (b = 0; b<26; b++) 
			{
				for (c = 0; c<26; c++) 
				{
					for (d = 0; d<26; d++) 
					{
						for (int o = 0; o<speed; o++)
						{
							p = head[e][b][c][d][o];
							while (p != NULL)
							{
								if ((judge(p, order, i) == 0) && p->wtime >= order[i]->wtime)
									order[i] = p;
								p = p->next;
							}
						}
					}
				}
			}
		}
	}
	for (i = 0; i<10; i++)
	{
		for (e = 0; e<26; e++)
		{
			for (b = 0; b<26; b++) 
			{
				for (c = 0; c<26; c++) 
				{
					for (d = 0; d<26; d++) 
					{
						for (int o = 0; o<speed; o++)
						{
							p = head2[e][b][c][d][o];
							while (p != NULL)
							{
								if ((judge(p, order2, i) == 0) && p->wtime >= order2[i]->wtime)
									order2[i] = p;
								p = p->next;
							}
						}
					}
				}
			}
		}
	}
	fprintf(fout,"characters:%d\n", charnum);
	fprintf(fout,"lines:%d\n", linenum);
	fprintf(fout,"words:%d\n", num);
	for (i = 0; i<10; i++)
	{
		fputs(order[i]->word,fout);
		fprintf(fout," %d\n", order[i]->wtime);
	}
	for (i = 0; i<10; i++)
	{
		fputs(order2[i]->word,fout);
		fprintf(fout," %d\n", order2[i]->wtime);
	}
	fclose(fout);
	return(0);
}
