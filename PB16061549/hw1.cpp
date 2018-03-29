#include <iostream>
#include <vector>
#include <cstring>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>

#pragma warning(disable : 4996)
using namespace std;


int char_number = 0;
int word_number = 0;
int line_number = 0;


typedef struct Node
{
	char wordroot[50];
	char word[50];
	int num;
	Node *next;
}Node;

Node  *prenode = NULL;


typedef struct Dword
{
	char *firstword;
	char *nextword;
	int num;
}Dword;

Dword HashTable[20000000];

Node  *Linkhead[26][26][26][26] = { NULL };
Node *Topword[15] = { NULL };

Dword *topphrase[15] = { NULL };



vector<char*>  getFilesList(const char * dir);
void DwordHandler(Node *s);

int isZimu(char ch)
{
	if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
		return 1;
	return 0;
}


int isOperator(char ch) {
	if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9'))
		return 1;
	return 0;
}


void getRoot(char *s, char* root) {
	int i = 0, leng;
	leng = strlen(s);
	for (i = leng - 1; s[i] >= '0'&&s[i] <= '9'; i--)
		leng--;
	for (i = 0; i < leng; i++)
		root[i] = s[i];
	root[i] = '\0';

	for (i = 0; i<leng; i++)
		if ('a' <= root[i] && root[i] <= 'z')
			root[i] = root[i] - 32;
}


void wordHandler(char *s)
{
	char root[50] = { 0 };
	getRoot(s, root);
	Node *p = NULL, *q = NULL;
	p = Linkhead[root[0] - 'A'][root[1] - 'A'][root[2] - 'A'][root[3] - 'A'];
	for (p; p &&strcmp(p->wordroot, root); p = p->next);
	if (!p)
	{
		q = new Node;
		q->num = 1; strcpy(q->word, s); strcpy(q->wordroot, root);
		q->next = Linkhead[root[0] - 'A'][root[1] - 'A'][root[2] - 'A'][root[3] - 'A'];
		Linkhead[root[0] - 'A'][root[1] - 'A'][root[2] - 'A'][root[3] - 'A'] = q;
		DwordHandler(q);
	}
	else
	{
		p->num++;
		if (strcmp(s, p->word) < 0)
			strcpy(p->word, s);
		DwordHandler(p);
	}
}


void charCounter(FILE *fp) {
	char ch;
	rewind(fp);
	while (EOF != (ch = fgetc(fp)))
		if (32 <= ch && ch <= 126)
			char_number++;
}


void lineCounter(FILE *fp) {
	char ch;
	rewind(fp);
	while (EOF != (ch = fgetc(fp)))
		if (ch == '\n')
			line_number++;
	line_number++;
}


void wordCounter(FILE *fp) {
	int flag = 0, i = 1, j = 0;
	char a[50] = { 0 };
	char ch;
	rewind(fp);
	if (!fp) return;
	while (1) {
		for (i = 0; i <= 3 && EOF != (ch = fgetc(fp)); i++)
		{
			if (!isZimu(ch)) break;
			a[i] = ch;
		}
		if (i == 4) {
			word_number++;
			while (EOF != (ch = fgetc(fp)) && isOperator(ch))
			{
				if (i <= 45)   a[i++] = ch;
			}
			a[i] = '\0';
			if (strlen(a) <= 40)
				wordHandler(a);
		}

		if (EOF == ch) break;
	}
}


unsigned int ELFHash(char *str)
{
	unsigned int hash = 0;
	unsigned int x = 0;

	while (*str)
	{
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xF0000000L) != 0)
		{

			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	return (hash & 0x7FFFFFFF);
}


void DwordHandler(Node *s)
{
	unsigned int key = 0;
	int i = 0, j = 0;
	if (!prenode)
	{
		prenode = s;
		return;
	}
	char a[100] = { 0 }, b[100] = { 0 }, c[50] = { 0 }, d[50] = { 0 };
	strcpy(a, prenode->wordroot);
	strcat(a, " ");
	strcat(a, s->wordroot);
	key = ELFHash(a) % 19000000;
	for (j = 0, i = key; j<19000000 && HashTable[i].num != 0; j++)
	{
		getRoot(HashTable[i].firstword, c);
		strcpy(b, c);
		strcat(b, " ");
		getRoot(HashTable[i].nextword, d);
		strcat(b, d);
		if (!strcmp(a, b))
		{
			HashTable[i].num++;
			if (strcmp(prenode->word, HashTable[i].firstword)<0)
				HashTable[i].firstword = prenode->word;
			if (strcmp(s->word, HashTable[i].nextword)<0)
				HashTable[i].nextword = s->word;
			break;
		}
		i = (i + 1) % 19000000;
	}

	if (HashTable[i].num == 0)
	{
		HashTable[i].firstword = prenode->word;
		HashTable[i].nextword = s->word;
		HashTable[i].num = 1;
	}

	prenode = s;
}



void CountQuantity(const char *fileName)
{
	FILE *fp;
	fp = fopen(fileName, "r");
	if (!fp) printf("fail to open\n");
	charCounter(fp);
	lineCounter(fp);
	wordCounter(fp);
	fclose(fp);
	return;
}



void getTopWord()
{
	int i = 0, j = 0, k = 0, l = 0, n = 0, m = 0, r = 0;
	Node *p;
	for (i = 0; i<26; i++)
		for (j = 0; j<26; j++)
			for (k = 0; k<26; k++)
				for (l = 0; l < 26; l++)
					for (p = Linkhead[i][j][k][l]; p; p = p->next)
					{
						for (m = 0; m < 10 && Topword[m]; m++);

						if (m <= 9)
						{
							for (n = m - 1; n >= 0 && Topword[n]->num < p->num; n--)
								Topword[n + 1] = Topword[n];
							Topword[n + 1] = p;
						}
						else
						{
							for (n = 9; n >= 0 && Topword[n]->num < p->num; n--)
								Topword[n + 1] = Topword[n];
							Topword[n + 1] = p;
						}
					}
}


void getTopDword()
{
	int i = 0, j = 0, m = 0;
	while (HashTable[i].num == 0 && i <= 19000000) i++;
	for (; i < 19000000; i++)
	{


		for (m = 0; m < 10 && topphrase[m]; m++);

		if (m <= 9)
		{
			for (j = m - 1; j >= 0 && topphrase[j]->num < HashTable[i].num; j--)
				topphrase[j + 1] = topphrase[j];
			topphrase[j + 1] = &HashTable[i];
		}
		else
		{
			for (j = 9; j >= 0 && topphrase[j]->num < HashTable[i].num; j--)
				topphrase[j + 1] = topphrase[j];
			topphrase[j + 1] = &HashTable[i];
		}
	}


}

int main(int argc, char *argv[])
{
	int k = 0;
	char dir[200];
	FILE *fp = NULL;

	strcpy(dir, argv[1]);

	vector<char*>allPath = getFilesList(dir);


	for (size_t i = 0; i < allPath.size(); i++)
	{
		char *perPath = allPath.at(i);
		CountQuantity(perPath);
	}

	getTopWord();
	getTopDword();

	fp = fopen("result.txt", "w");

	fprintf(fp, "char_number=%d\nline_number=%d\nword_number=%d\n\n", char_number, line_number, word_number);
	fprintf(fp, "the top ten frequency of word :\n");
	for (k = 0; k < 10; k++)
		fprintf(fp, "%-6s     %-6d\n", Topword[k]->word, Topword[k]->num);
	fprintf(fp, "\n");
	fprintf(fp, "the top ten frequency of phrase : \n");
	for (k = 0; k < 10; k++)
		fprintf(fp, "%-10s%-10s %-10d\n", topphrase[k]->firstword, topphrase[k]->nextword, topphrase[k]->num);
	fprintf(fp, "\n");

	fclose(fp);

	return 0;
}


vector<char*> getFilesList(const char * dir)
{
	vector<char*> allPath;
	char dirNew[200];
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
		if (findData.attrib & _A_SUBDIR)
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
