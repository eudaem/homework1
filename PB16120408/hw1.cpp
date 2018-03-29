#pragma once
#include"stdio.h"
#include"malloc.h"
#include"string.h"
#include <iostream>
#include <cstring>        // for strcpy(), strcat()
#include <io.h>

#define WORDNUM 50331553
#define PRIME 50331553
#define MAX 10
#define LIMIT 1025
#define WORDLETTER 4

long countcode = 0;//字符数
long countword = 0;//单词数
long countline = 0;//行数
long wordletter = false;
FILE *fin = NULL;
bool fileflag = false;//文件是否为空标识，true空，false不空
char *dir;
FILE *fout = NULL;//输出文件指针

char *str = NULL;

char *wd = NULL;
char *ph1 = NULL, *ph2 = NULL;
char *word[WORDNUM] = { NULL };
unsigned int hashtime[WORDNUM] = { 0 };//hash数组，存储频数

char *ph = NULL;
unsigned int phrasetime[WORDNUM];
char *phf[WORDNUM];//词组前
char *phb[WORDNUM];//词组后

int wordmax[MAX] = { 0 };
int phrasemax[MAX] = { 0 };

void testme(char *file);
void listFiles(const char * dir);
void count_code(char c);
void count_line(char c);
void count_word(char c);
bool if_code(char c);
bool if_split(char c);
bool if_number(char c);
bool if_letter(char c);
bool if_line(char c);
bool if_word(char c);
int if_wordorder(char *s1, char *s2);
bool if_sameword(char *s1, char *s2);
void if_lettercase(char &c);
bool input_onefile_open(void);
void input_onefile_close(void);
bool input_file_open(char *file);
bool output_onefile_open(void);
void output_onefile_close(void);
bool output_file_open(char *file);
void output_print_code(long num);
void output_print_line(long num);
void output_print_word(long num);
char store_word(void);
unsigned int store_BKDRHash(char *str);
void store_hashword(void);
void store_wordfind(void);
char store_phrase(void);
void store_hashphrase(void);
void store_phrasefind(void);
/*testpart:用来测试各文件写好的独立函数*/

void testpart(void) {
	listFiles(dir);//
}

void testme(char *file)
{
	int i;
	char c;
//	printf("%s\n", file);


	input_file_open(file);

	fileflag = false;
	while ((c = fgetc(fin)) != EOF) {
		if (fileflag == false)
		{
			countline++;
			fileflag = true;
		}
		count_code(c);
		count_line(c);
	}//printf("ahah\n");
	input_onefile_close();

	//printf("hehe\n");
	input_file_open(file);
	while (true) {
		c = fgetc(fin);
		count_word(c);
		if (c == EOF)
		{
			//printf("hh\n");
			break;
		}
	}
	input_onefile_close();

	input_file_open(file);
	wordletter = false;
	for (i = 0; i<WORDNUM; i++)
	{
		c = store_word();
		store_hashword();
		if (c == 0)
			break;
	}

	input_onefile_close();

//	printf("phrase start\n");
	input_file_open(file);

	ph1 = (char*)malloc(LIMIT * sizeof(char));
	ph2 = (char*)malloc(LIMIT * sizeof(char));
	ph1[0] = '\0';
	ph2[0] = '\0';

	for (i = 0; i<WORDNUM; i++)
	{
		c = store_phrase();
		if (ph != NULL) {
			strcpy_s(ph2, strlen(ph) + 1, ph);
			free(ph);
			store_hashphrase();

			free(ph1);
			ph1 = (char*)malloc(LIMIT * sizeof(char));
			ph1[0] = '\0';
			strcpy_s(ph1, strlen(ph2) + 1, ph2);

			free(ph2);
			ph2 = (char*)malloc(LIMIT * sizeof(char));
			ph2[0] = '\0';
		}
		if (c == 0)
			break;
	}

//	printf("phrase end\n");
	input_onefile_close();

}

void hhhh() {
	str = (char*)malloc(40 * sizeof(char));
	str[0] = '0';
	str[1] = '\0';
}

void listFiles(const char * dir)
{
	char dirNew[200];
	char file[200];
	char sb[200];

	int len;
//	printf("haha\n");
	strcpy_s(dirNew, strlen(dir) + 1, dir);
	strcat_s(dirNew, 200, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

	int handle;
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

			//printf("%s\n", findData.name);

			strcpy_s(sb, strlen(dir) + 1, dir);
			strcat_s(sb, 200, "\\");
			strcat_s(sb, 200, findData.name);
			listFiles(sb);
		}
		else {
			strcpy_s(file, strlen(dirNew) + 1, dirNew);
			len = strlen(file);
			file[len - 3] = '\0';
			strcat_s(file, 200, findData.name);
			//printf("%s\n", file);
			testme(file);//
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
}

/*count_code:计数字符个数*/

void count_code(char c) {
	if (if_code(c))
		countcode++;
	return;
}

/*count_code:计数单词个数*/

void count_word(char c) {
	if (if_word(c))
		countword++;
	return;
}

/*count_line：计数行数*/

void count_line(char c) {
	if (if_line(c))
		countline++;
	return;
}

/*if_letter:判断输入字符是否是字母*/

bool if_letter(char c) {
	if ((c >= 'a'&&c <= 'z') || (c >= 'A'&&c <= 'Z'))
		return(true);
	else
		return(false);
}

/*if_number:判断输入字符是否是数字*/

bool if_number(char c) {
	if (c >= '0'&&c <= '9')
		return(true);
	else
		return(false);
}

/*if_split:判断输入字符是否是分隔符*/

bool if_split(char c) {
	if (if_letter(c) || if_number(c))
		return(false);
	else
		return(true);
}

/*if_code:判断出入是否是字符*/

bool if_code(char c) {
	if (c >= 32 && c <= 126)
		return(true);
	else
		return(false);
}

/*if_line:判断出入是否是换行符*/

bool if_line(char c) {
	if (c == '\n')
		return(true);
	else
		return(false);
}

/*if_word:判断是否为单词*/

bool if_word(char c) {
	if (c == EOF&&wordletter >= WORDLETTER)
	{
		wordletter = false;
		return(true);
	}
	if (if_split(c)) {//若为分隔符
		if (wordletter >= WORDLETTER)//已成单词，返回为真，清空计数
		{
			wordletter = false;
			return(true);
		}
		else
		{
			wordletter = false;
			return(false);
		}//未成单词，返回为假，清空计数
	}
	else {//若为字母数字
		if (if_letter(c))//若为字母，计数增加
			wordletter++;
		else if (wordletter >= WORDLETTER)//若为数字，已成单词
			;
		else//若为数字，未成单词，返回为假
			wordletter = false;
		return(false);
	}
}

/*if_wordorder：判断两字符串的字典序，返回1==s1在前，返回-1==s2在前，返回0==相等*/

int if_wordorder(char *s1, char *s2) {
	int i;
	for (i = 0; *(s1 + i) != '\0'&&*(s2 + i) != '\0'; i++) {
		if (*(s1 + i) < *(s2 + i))//s1字典序在前
			return(1);
		else if (*(s1 + i) > *(s2 + i))//s2字典序在前
			return(-1);
		else
			;
	}
	if (*(s1 + i) == '\0'&&*(s2 + i) != '\0')//s2还有多,s1字典序在前
		return(1);
	if (*(s1 + i) != '\0'&&*(s2 + i) == '\0')//s1还有多，s2字典序在前
		return(-1);
	return(0);
}

/*if_sameword:判断两个单词是否相同*/

bool if_sameword(char *s1, char *s2) {
	if (s1 == NULL || s2 == NULL)
		return(false);
	int i, j;
	bool flag = false;//判断中间有否出现数字不同的标志
	for (i = 0; *(s1 + i) != '\0'&&*(s2 + i) != '\0'; i++) {
		if (if_letter(*(s1 + i)) && if_letter(*(s2 + i))) {//若该位置两者均为字母
			if (*(s1 + i) != *(s2 + i) && abs(*(s1 + i) - *(s2 + i)) != 32)//若不相等，返回为假
				return(false);
		}
		else {//若该位置两者均为数字
			if (*(s1 + i) != *(s2 + i))//若不相等，终止循环，检验是否为数字结尾
			{
				flag = true;
				break;
			}
		}
	}
	if (flag || (*(s1 + i) != '\0' || *(s2 + i) != '\0')) {
		for (j = i; *(s1 + j) != '\0'; j++) {//从不同处起步
			if (if_number(*(s1 + j)))//若同为数字，过
				;
			else//不为数字结尾，返回为假
				return(false);
		}
		for (j = i; *(s2 + j) != '\0'; j++) {//从不同处起步
			if (if_number(*(s2 + j)))//若同为数字，过
				;
			else//不为数字结尾，返回为假
				return(false);
		}
	}
	return(true);
}

void if_lettercase(char &c)
{
	if (c <= 'z'&&c >= 'a')
		c = c - 32;
	return;
}

bool input_onefile_open(void) {
	errno_t err;
	err = fopen_s(&fin, "sample.txt", "r");
	if (err == 1)
	{
		printf("can't open the file\n");
		fclose(fin);
		return(false);
	}
	else
		return(true);
}

bool input_file_open(char *file) {
	errno_t err;
	err = fopen_s(&fin, file, "r");
	if (err == 1)
	{
		printf("can't open the file\n");
		fclose(fin);
		return(false);
	}
	else
		return(true);
}

void input_onefile_close(void) {
	fclose(fin);
}

/*output_onefile_open：打开输出的单个文件*/

bool output_onefile_open(void) {
	errno_t err;
	err = fopen_s(&fout, "result.txt", "w");
	if (err == 1)
	{
		printf("can't open the file\n");
		fclose(fout);
		return(false);
	}
	else
		return(true);
}

bool output_file_open(char *file) {
	errno_t err;
	err = fopen_s(&fout, file, "w");
	if (err == 1)
	{
		printf("can't open the file\n");
		fclose(fout);
		return(false);
	}
	else
		return(true);
}

/*output_onefile_close:关闭输出单文件*/

void output_onefile_close(void) {
	fclose(fout);
	return;
}

/*output_print_code：输出字符数*/

void output_print_code(long num) {
	fprintf(fout, "char_number:%ld\n", num);
	return;
}

/*output_print_line：输出行数*/

void output_print_line(long num) {
	fprintf(fout, "line_number:%ld\n", num);
	return;
}

/*output_print_word：输出单词数*/

void output_print_word(long num) {
	fprintf(fout, "word_number:%ld\n", num);
	return;
}

char store_word(void) {
	int i = 0;
	char c;
	wd = (char *)malloc(LIMIT * sizeof(char));

	while (true) {
		c = fgetc(fin);

		if (c == EOF&&wordletter >= WORDLETTER)
		{
			wd[wordletter] = '\0';
			wordletter = false;
			return(0);
		}
		if (c == EOF&&wordletter < WORDLETTER)
		{
			free(wd);
			wd = NULL;
			return(0);
		}
		if (if_split(c)) {//若为分隔符
			if (wordletter >= WORDLETTER)//已成单词，返回为真，清空计数
			{
				wd[wordletter] = '\0';
				//				printf("%s\n", word);
				wordletter = false;
				return(1);
			}
			else
			{
				//				printf("aa\n");
				free(wd);
				wd = (char *)malloc(LIMIT * sizeof(char));
				wordletter = false;
			}//未成单词，返回为假，清空计数
		}
		else {//若为字母数字
			  //			printf("bb\n");
			if (if_letter(c))//若为字母，计数增加
			{
				wd[wordletter] = c;
				wordletter++;
			}
			else if (wordletter >= WORDLETTER)//若为数字，已成单词
			{
				wd[wordletter] = c;
				wordletter++;
			}
			else//若为数字，未成单词，返回为假
			{
				free(wd);
				wd = (char *)malloc(LIMIT * sizeof(char));
				wordletter = false;
			}
		}
	}
}

char store_phrase(void) {
	int i = 0;
	char c;
	ph = (char *)malloc(LIMIT * sizeof(char));
	if (ph == NULL)
	{
		printf("all\n");
		return(1);
	}

	while (true) {
		c = fgetc(fin);
		//		printf("%c\n",c);
		//		printf("%ld\n", wordletter);
		//		printf("hehe\n");
		if (c == EOF&&wordletter >= WORDLETTER)
		{
			ph[wordletter] = '\0';
			wordletter = false;
			return(0);
		}
		if (c == EOF&&wordletter < WORDLETTER)
		{
			free(ph);
			ph = NULL;
			return(0);
		}
		if (if_split(c)) {//若为分隔符
			if (wordletter >= WORDLETTER)//已成单词，返回为真，清空计数
			{
				ph[wordletter] = '\0';
				//				printf("%s\n", word);
				wordletter = false;
				return(1);
			}
			else
			{
				//				printf("aa\n");
				free(ph);
				ph = (char *)malloc(LIMIT * sizeof(char));
				wordletter = false;
			}//未成单词，返回为假，清空计数
		}
		else {//若为字母数字
			  //			printf("bb\n");
			if (if_letter(c))//若为字母，计数增加
			{
				ph[wordletter] = c;
				wordletter++;
			}
			else if (wordletter >= WORDLETTER)//若为数字，已成单词
			{
				ph[wordletter] = c;
				wordletter++;
			}
			else//若为数字，未成单词，返回为假
			{
				free(ph);
				ph = (char *)malloc(LIMIT * sizeof(char));
				wordletter = false;
			}
		}
	}
}

unsigned int store_BKDRHash(char *str)
{
	int n = 0, i;
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..  
	unsigned int hash = 0;
	char copy[LIMIT] = { '\0' };
	n = strlen(str);
	n--;
	while ((int)str[n] > 47 && (int)str[n] < 58) {
		n--;
	}
	for (i = 0; i < n + 1; i++)
		copy[i] = str[i];
	copy[n + 1] = '\0';  //去掉数字结尾
	n = 0;
	while (copy[n] != '\0')
	{
		if_lettercase(copy[n]);
		hash = hash * seed + copy[n];
		n++;
	}
	return ((hash & 0x7FFFFFFF) % PRIME);//prime为一个大质数
}

void store_hashword(void) {
	if (wd == NULL)
		return;
	unsigned int i = 0;
	unsigned int flag = store_BKDRHash(wd);
	//	printf("%u\n", flag);
	if (hashtime[flag] == 0)//如果无词就入住
	{
		hashtime[flag]++;//频数+1
		word[flag] = wd;//记录所在字符数组的下标
	}
	else {
		if (if_sameword(word[flag], wd))//相同
		{
			hashtime[flag]++;//频数+1
			if (strcmp(word[flag], wd) > 0)//
			{
				//				printf("%s,%s\n", word[flag], wd);
				free(word[flag]);
				word[flag] = wd;//
			}
			else
				free(wd);
		}
		else {
			for (i = 0; i<WORDNUM; i++)
			{
				if (hashtime[(flag + i) % WORDNUM] == 0)//如果无词就入住
				{
					hashtime[(flag + i) % WORDNUM]++;//频数+1
					word[(flag + i) % WORDNUM] = wd;//记录所在字符数组的下标
					break;
				}
				else {
					if (if_sameword(word[(flag + i) % WORDNUM], wd))//相同
					{
						hashtime[(flag + i) % WORDNUM]++;//频数+1
						if (strcmp(word[(flag + i) % WORDNUM], wd) > 0)//	
						{
							free(word[(flag + i) % WORDNUM]);
							word[(flag + i) % WORDNUM] = wd;//
						}
						else
							free(wd);
						break;
					}
				}
			}
		}
	}
}

void store_wordfind(void) {
	int i, j, l;
	int top = 0, bot = MAX - 1;
	int mid = (top + bot) / 2;
	for (j = 0; j < MAX; j++) {
		for (i = 0; i < WORDNUM; i++) {//wordmax内存储hashtime数组的下标，0最大，9最小		
			if (hashtime[i] == 0)
				continue;
			if (hashtime[i] >= hashtime[wordmax[j]]) {
				if (j == 0)
					wordmax[j] = i;
				else {
					for (l = 0; l < j; l++) {
						if (i == wordmax[l])
							goto loop;
					}
					wordmax[j] = i;
				}
			loop:;
			}
		}
	}
}

void store_hashphrase(void) {
	if (ph1 == NULL || ph2 == NULL || ph1[0] == '\0' || ph2[0] == '\0')
		return;
	//	printf("hehe\n");
	unsigned int i = 0;
	unsigned int flag = (store_BKDRHash(ph1) + store_BKDRHash(ph2)) % PRIME;
	if (phrasetime[flag] == 0)//如果无词就入住
	{
		phrasetime[flag]++;//频数+1
		phf[flag] = (char*)malloc((strlen(ph1) + 1) * sizeof(char));
		phb[flag] = (char*)malloc((strlen(ph2) + 1) * sizeof(char));
		phf[flag][0] = '\0';
		phb[flag][0] = '\0';
		strcpy_s(phf[flag], strlen(ph1) + 1, ph1);//
		strcpy_s(phb[flag], strlen(ph2) + 1, ph2);//记录所在字符数组的下标
	}
	else {
		if (if_sameword(phf[flag], ph1) && if_sameword(phb[flag], ph2)) {
			//	if (if_sameword(phf[flag], ph1) && if_sameword(phb[flag], ph2))
			phrasetime[flag]++;//频数+1
			if (strcmp(phf[flag], ph1) > 0)//
			{
				//	printf("%s %s\n", ph1, phf[flag]);
				//			printf("%s,%s\n", word[flag], wd);
				free(phf[flag]);
				phf[flag] = (char*)malloc((strlen(ph1) + 1) * sizeof(char));
				phf[flag][0] = '\0';
				strcpy_s(phf[flag], strlen(ph1) + 1, ph1);//
			}
			else
				;
			if (strcmp(phb[flag], ph2) > 0)//
			{
				//	printf("%s %s\n", ph2, phb[flag]);
				free(phb[flag]);
				phb[flag] = (char*)malloc((strlen(ph2) + 1) * sizeof(char));
				phb[flag][0] = '\0';
				strcpy_s(phb[flag], strlen(ph2) + 1, ph2);
			}
			else
				;//ph2 not free
		}
		else {
			for (i = 0; i<WORDNUM; i++)
			{
				if (phrasetime[(flag + i) % WORDNUM] == 0)//如果无词就入住
				{
					phrasetime[(flag + i) % WORDNUM]++;//频数+1
					phf[(flag + i) % WORDNUM] = (char*)malloc((strlen(ph1) + 1) * sizeof(char));
					phb[(flag + i) % WORDNUM] = (char*)malloc((strlen(ph2) + 1) * sizeof(char));
					phf[(flag + i) % WORDNUM][0] = '\0';
					phb[(flag + i) % WORDNUM][0] = '\0';
					strcpy_s(phf[(flag + i) % WORDNUM], strlen(ph1) + 1, ph1);//
					strcpy_s(phb[(flag + i) % WORDNUM], strlen(ph2) + 1, ph2);//记录所在字符数组的下标
					break;
				}
				else {
					if (if_sameword(phf[(flag + i) % WORDNUM], ph1) && if_sameword(phb[(flag + i) % WORDNUM], ph2))//相同
					{
						//if (if_sameword(phf[(flag + i) % WORDNUM], ph1) && if_sameword(phb[(flag + i) % WORDNUM], ph2))
						phrasetime[(flag + i) % WORDNUM]++;//频数+1
						if (strcmp(phf[(flag + i) % WORDNUM], ph1) > 0)//	
						{
							//printf("%s %s\n", ph1, phf[(flag + i) % WORDNUM]);
							free(phf[(flag + i) % WORDNUM]);
							phf[(flag + i) % WORDNUM] = (char*)malloc((strlen(ph1) + 1) * sizeof(char));
							phf[(flag + i) % WORDNUM][0] = '\0';
							strcpy_s(phf[(flag + i) % WORDNUM], strlen(ph1) + 1, ph1);//
						}
						else
							;
						if (strcmp(phb[(flag + i) % WORDNUM], ph2) > 0)//	
						{
							//printf("%s %s\n", ph2, phb[(flag + i) % WORDNUM]);
							free(phb[(flag + i) % WORDNUM]);
							phb[(flag + i) % WORDNUM] = (char*)malloc((strlen(ph2) + 1) * sizeof(char));
							phb[(flag + i) % WORDNUM][0] = '\0';
							strcpy_s(phb[(flag + i) % WORDNUM], strlen(ph2) + 1, ph2);
						}
						else
							;
						break;
					}
				}
			}
		}
	}
}

void store_phrasefind(void) {
	int i, j, l;
	int top = 0, bot = MAX - 1;
	int mid = (top + bot) / 2;
	for (j = 0; j < MAX; j++) {
		for (i = 0; i < WORDNUM; i++) {//wordmax内存储hashtime数组的下标，0最大，9最小		
			if (phrasetime[i] == 0)
				continue;
			if (phrasetime[i] >= phrasetime[phrasemax[j]]) {
				if (j == 0)
					phrasemax[j] = i;
				else {
					for (l = 0; l < j; l++) {
						if (i == phrasemax[l])
							goto loop;
					}
					phrasemax[j] = i;
				}
			loop:;
			}
		}
	}
	for (i = 0; i < MAX; i++) {
		for (j = 0; j < WORDNUM; j++)
		{
			if (word[j] != NULL&&if_sameword(phf[phrasemax[i]], word[j]))//&& strcmp(phf[phrasetime[i]], word[j]) > 0
			{
				strcpy_s(phf[phrasemax[i]], strlen(word[j]) + 1, word[j]);
			}
			if (word[j] != NULL&&if_sameword(phb[phrasemax[i]], word[j]))//&& strcmp(phb[phrasetime[i]], word[j]) > 0
			{
				strcpy_s(phb[phrasemax[i]], strlen(word[j]) + 1, word[j]);
			}
		}
	}/**/
}


int main(int argc, char *argv[]) {
	int i;
	printf("%s\n", argv[0]);//C:\users\asus\documents\visual studio 2015\Projects\ConsoleApplication7\Release\ConsoleApplication7.exe
	dir = "C:\\Users\\asus\\Desktop\\123\\newsample";
	if (argc == 2)
		dir = argv[1];
	
	listFiles(dir);

	output_onefile_open();
	output_print_code(countcode);
	output_print_line(countline);
	output_print_word(countword);

	store_wordfind();
	store_phrasefind();

	fprintf(fout, "the top ten frequency of word:\n");
	for (i = 0; i < MAX; i++) {
		fprintf(fout, "%s\t%u\n", word[wordmax[i]], hashtime[wordmax[i]]);
	}

	fprintf(fout, "the top ten frequency of phrase:\n");
	for (i = 0; i < MAX; i++) {
		fprintf(fout, "%s %s\t%u\n", phf[phrasemax[i]], phb[phrasemax[i]], phrasetime[phrasemax[i]]);
	}
	output_onefile_close();/**/

	printf("The program ends\n");
	getchar();//让VS别一闪而过
	return(0);
}
