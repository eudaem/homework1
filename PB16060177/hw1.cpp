// ConsoleApplication4.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "process.h"
#include "string"
#include "cstring"
#include "iostream"
#include "io.h"
#include "stdlib.h"
#define strcasecmp _stricmp
#define MAX 20000000
int characternum = 0,wordnum=0,line=0;
using namespace std;
FILE *fin, *fout;
char *words[MAX], *former[MAX], *latter[MAX];
int cont[MAX], area[MAX], prnum[MAX], loc[MAX], num = 0, num1 = 0;
void listFiles(const char * dir);

void init()
{
	for (int i = 0; i < MAX; i++)
	{
		cont[i] = 0;
		prnum[i] = 0;
	}
}

unsigned int hashindex(char *str)
{
	int n, i;
	int seed = 131;  
	unsigned int hash = 0;
	char copy[1024];
	n = strlen(str);
	n--;
	while (str[n] > 47 && str[n] < 58) {
		n--;
	}
	for (i = 0; i < n + 1; i++)
		copy[i] = str[i];
	copy[n + 1] = '\0';  
	n = 0;
	while (copy[n] != '\0')
	{
		if (copy[n] >= 65 && copy[n] <= 90 || copy[n] >= 48 && copy[n] <= 57)
			hash = hash * seed + copy[n];
		else
			hash = hash * seed + copy[n] - 32;
		n++;
	}
	return ((hash & 0x7FFFFFFF) % MAX);
}

bool compare(int n, char *str)
{
	char c[1024], b[1024];
	int k;
	strcpy_s(c,strlen(str)+1, str);
	strcpy_s(b,strlen(words[n])+1, words[n]);
	k = strlen(c);
	k--;
	while (c[k] > 47 && c[k] < 58) {
		k--;
	}
	c[k + 1] = '\0';
	k = strlen(b);
	k--;
	while (b[k] > 47 && b[k] < 58) {
		k--;
	}
	b[k + 1] = '\0';
	if (strcasecmp(b, c) == 0)
		return true;
	return false;
}

void hashe(char *str)
{
	int i;
	unsigned int index;
	bool flag;
	index = hashindex(str);
	if (cont[index] == 0)
	{
		cont[index]++;
		area[index] = num;
		num++;
	}
	else
	{
		flag = compare(area[index], str);
		if (flag == true)
		{
			if (strcmp(words[area[index]], str) > 0)
			{
				delete(words[area[index]]);
				words[area[index]] = str;
			}
			else
				delete(str);
			cont[index]++;
			return;
		}
		for (i = 1;; i++)
		{
			if (cont[(index + i) % MAX] == 0)
			{
				cont[(index + i) % MAX]++;
				area[(index + i) % MAX] = num;
				num++;
				break;
			}
			flag = compare(area[(index + i) % MAX], str);
			if (flag == true)
			{
				if (strcmp(words[area[(index + i) % MAX]], str) > 0)
				{
					delete(words[area[(index + i) % MAX]]);
					words[area[(index + i) % MAX]] = str;
				}
				else
					delete(str);
				cont[(index + i) % MAX]++;
				break;
			}
		}
	}
}


void characterandlinecollect(void)
{
	
	char c;
	while ((c = fgetc(fin)) != EOF)
	{
		if (c >= 32 && c <= 126)
			characternum++;    
		if (c == '\n')
		{
			line++;           
		}
	}
	if (characternum != 0)
		line++;        
}

bool compare1(int n, char *str)
{
	char c[1024], b[1024];
	int k;
	strcpy_s(c, strlen(str)+1,str);
	strcpy_s(b, strlen(former[n])+1,former[n]);
	k = strlen(c);
	k--;
	while (c[k] > 47 && c[k] < 58)
	{
		k--;
	}
	c[k + 1] = '\0';
	k = strlen(b);
	k--;
	while (b[k] > 47 && b[k] < 58) {
		k--;
	}
	b[k + 1] = '\0';
	if (strcasecmp(b, c) == 0)
		return true;
	return false;
}

bool compare2(int n, char *str)
{
	char c[1024], b[1024];
	int k;
	strcpy_s(c,strlen(str)+1, str);
	strcpy_s(b,strlen(latter[n])+1, latter[n]);
	k = strlen(c);
	k--;
	while (c[k] > 47 && c[k] < 58) {
		k--;
	}
	c[k + 1] = '\0';
	k = strlen(b);
	k--;
	while (b[k] > 47 && b[k] < 58) {
		k--;
	}
	b[k + 1] = '\0';
	if (strcasecmp(b, c) == 0)
		return true;
	return false;
}

int put(char *str1, char *str2, int n)
{
	if (n == 0)
		return 0;
	int hashnum;
	int p, a, b, length;
	hashnum = (hashindex(str1) + hashindex(str2)) % MAX;
	while (prnum[hashnum]>0) {
		p = loc[hashnum];
		a = compare1(p, str1);
		b = compare2(p, str2);
		if (a == 1 && b == 1) {
			prnum[hashnum]++;
			return 1;
		}
		else
			hashnum = (hashnum + 1) % MAX;
	}

	loc[hashnum] = n;
	prnum[hashnum] = 1;
	return 0;
}


int everywordcollect()
{
	int wordnum1 = 0, count = 0, k, j, t,signP=0;
	char c, w[1024];
	rewind(fin);
	init();
	former[0]= (char *)malloc(sizeof(char) * 10);
	former[0][0] = '\0';
	while ((c = fgetc(fin)) != EOF)            
	{
		if (c >= 65 && c <= 90 || c >= 97 && c <= 122)
		{
			count++;
			w[count - 1] = c;
			for (int i = 0; i<3; i++)
			{
				c = fgetc(fin);
				if (c >= 65 && c <= 90 || c >= 97 && c <= 122)
				{
					count++;
					w[count - 1] = c;
				}
				else
					break;
			}
			if (count == 4)
			{
				wordnum1++;
				c = fgetc(fin);
				for (k = count; c >= 65 && c <= 90 || c >= 97 && c <= 122 || c >= 48 && c <= 57; k++)
				{
					w[k] = c;
					c = fgetc(fin);
				}
				w[k] = '\0';
				words[num] = (char *)malloc(sizeof(char) * (strlen(w) + 1));
				former[num1+1]= (char *)malloc(sizeof(char) * (strlen(w)+1));
				latter[num1]= (char *)malloc(sizeof(char) * (strlen(w) + 1));
				strcpy_s(words[num],strlen(w)+1,w);
				strcpy_s(former[num1+1], strlen(w)+1 , w);
				strcpy_s(latter[num1], strlen(w)+1, w);
				hashe(words[num]);
				signP = put(former[num1], latter[num1], num1);
				if (signP == 1) {

					free(former[num1]);
					free(latter[num1]);
					former[num1] = (char *)malloc((strlen(w)+1)* sizeof(char));
					strcpy_s(former[num1], (strlen(w)+1),w);
					free(former[num1 + 1]);
				}
				else num1++;
			}
			count = 0;
		}
		else;
	}
	for (k = 0; k < 10; k++)
		for (j = k + 1; j < MAX; j++)
			if (cont[k] < cont[j])
			{
				t = cont[k];
				cont[k] = cont[j];
				cont[j] = t;
				t = area[k];
				area[k] = area[j];
				area[j] = t;
			}
for (k = 0; k < 10; k++)
	for (j = k + 1; j < MAX; j++)
		if (prnum[k] < prnum[j])
		{
			t = prnum[k];
			prnum[k] = prnum[j];
			prnum[j] = t;
			t = loc[k];
			loc[k] = loc[j];
			loc[j] = t;
		}
	return wordnum1;
}

int main(int argc, char *argv[])
{
	int k;
	char dir[200];
	strcpy_s(dir,200,argv[1]);
	listFiles(dir);
	errno_t err;           
	err = fopen_s(&fout, "result.out", "w");  
	if (err == 1)
	{
		printf("can't open file!\n");
		exit(0);
	}
	fprintf(fout, "characters:%d\n", characternum);  
	fprintf(fout, "line:%d\n", line);
	fprintf(fout, "word:%d\n", wordnum);
	fprintf(fout, "\nthe top ten frequency of word:\n");
	for (k = 0; k < 10; k++)
	{
		fprintf(fout, "%s:%d\n", words[area[k]], cont[k]);
	}
	fprintf(fout, "\nthe top ten frequency of phrase:\n");
	for (k = 0; k < 10; k++)
	{
		fprintf(fout, "%s %s:%d\n", former[loc[k]],latter[loc[k]], prnum[k]);
	}
	fclose(fout);       //close files
	system("pause");
	return 0;
}

void listFiles(const char * dir)
{
	char dirNew[200];
	char copy[200];
	char c[200];
	strcpy_s(dirNew, 200, dir);
	strcat_s(dirNew, 200, "\\*.*");    
	int length;
	int handle;
	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1)        
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
			strcpy_s(c, 200, dir);
			strcat_s(c, 200, "\\");
			strcat_s(c, 200, findData.name);
			listFiles(c);
		}
		else {
			int i;
			errno_t err;
			strcpy_s(copy, 200, dirNew);
			length = strlen(copy);
			copy[length - 3] = '\0';
			strcat_s(copy, 200, findData.name);
			err = fopen_s(&fin, copy, "r");
		    characterandlinecollect();
			wordnum += everywordcollect();
			fclose(fin);
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);   
}


