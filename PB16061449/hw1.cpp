
#include<io.h>
#include<stdio.h>
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>
#include <ctype.h>

/*数据结构
权重分布：1，0.5，0.3，0.2，0.1
根
a	b	...	y	z
(a...z)	(a...z)	...	(a...z)	(a...z)
26^4=456976=446KB
实际上，我们只需要第四层的元素，其位置location的计算公式location=（first-26)*(second-26)*(third-26)*(forth-24);

位置上记录一个指针，指向以相同四个字符开头的所有单词。
具体结构：
int（核心字段位数）		string（核心字符串）

可边长数组
int核心码长度		int个数		string字符串

词频答案数据结构
int当前最小值	十个单元：（int 长度）（pointer 字符串地址值）
*/
/*整体思路
1、字符数：将文件以二进制形式读入内存中，以文件末为标识即可获得字符数

创建基准指针base，指向正在处理的字符

2、单词总数：	1）以字节为单位进行读取首先判断是否为合法单词：创建偏移指针，对base~base+3区域进行判定，
若不符合	（1）读入字符为'\n'，则行数line++;
（2）读入字符不为'\n'	（可能读入的是间隔符，可能不是）
base移动到偏移处的前一个地方，一直执行base++
倘若下一个字符是间隔符则flag=1，
倘若下一个字符是字母且flag=1，则执行最后一次base++，然后执行1）
若符合则进行2）（为至少以长度为4的字符开头的单词）
2）单词数word++，执行3）
3）算出location值，取出其中地址值
（1）若为NULL，则创建可边长结构体数组（初始化为NULL），然后填入
（2）若不为NULL，则遍历是否存在相同字符串，
（a）若有则个数++
（b）否则在空缺处填入
比如长度只有4如read，那么填入read
3、行数：getline
4、单词频：构建答案数据结构
遍历每一个树叶单元，在不为NULL处进行搜索，若搜索到比当前十个最长字符串最小值更大的，则更改、计入。
5、词组频率：哈希表：两个单词，单独前5位ascii码相加后的乘积
（int 频率）（string 前）（string 后）
*/


//数据结构区

typedef struct _nextWord NextWord;
typedef struct wordNode WordNode;
typedef struct topPhraseStruc TopPhraseStruc;
typedef struct topWordStruc TopWordStruc;

#define nullptr NULL 

struct wordNode
{
	int wordCount = 0;
	char *coreString = nullptr;
	char *suffixString = nullptr;
	int phraseNum = 0;
	int maxPhraseNum = 0;
	NextWord *nextWord = nullptr;		//数组形式	构成词组的下一次单词
	WordNode *next = nullptr;
};

struct _nextWord
{

	WordNode *nextWord = nullptr;
	int nextWordCount = 0;
};

struct topPhraseStruc
{
	int phraseCount = 0;	//出现次数
	WordNode *firstWord = nullptr;	//第一个单词
	WordNode *secondWord = nullptr;	//第二个单词
};

struct topWordStruc
{
	int wordCount = 0;
	WordNode *word = nullptr;
};

#define isALetter(temp)	((temp >= 'A'&&temp<='Z') || (temp>='a'&&temp <= 'z'))
#define isLetterOrNum(temp) ((temp >='A'&&temp<='Z') || (temp>='a'&&temp <= 'z')||(temp>='0'&&temp<='9'))
#define MAXLENGTH 1024
#define NW_INITSIZE 15
#define INCREAMENT 15	//nextWord数组增量
//全局变量区
int Characters = 0;	//统计总共的字符数
int NumOfWords = 0; //统计总共的单词书
WordNode *HashSet[456976];		//
char coreWordTemp[MAXLENGTH];
char suffixWordTemp[MAXLENGTH];
WordNode *FirstWord;		//临时词组指针，第一个单词信息
WordNode *LastWord;			//同上，第二个单词信息
TopWordStruc topTenWordList[10];
TopPhraseStruc topTenPhraseList[10];
char CompareTemp1[1024];
char CompareTemp2[1024];		//用于比较两字符串是否相同的临时空间

//函数声明区
int isInputLegal(int argc);		//判断输入参数是否合法，根据要求只要2个参数
int isFolder(char** argv);		//根据输入参数判断是否为文件夹，是则返回1，否则返回0
void CalAllFile(char filename[]);
char isNotAWord(char *offset);
void SolveTheWord(char *word);
WordNode *WordNodeInit();
void PushWordIntoList(WordNode *p, char* word);
void DevideWord(char *word);
char isWordEqual(char *string1, char *string2);
void PushPhraseIntoList();
void FrequencyTopTen();
void WordTen(WordNode *wordPointer);
void PhraseTen(WordNode *wordPointer);
char compareWord(WordNode *node1, WordNode *node2);
char *MakeString(char *string);
void printWordList();
void printPhraseList();
WordNode *WordNodeInit(char *coreWord, char *suffixWord);
void anothermain(char *fileString);
bool is_dir(const char* path);
char *testFileRead(char *filename);
char isSameWord(char *string1, char *string2);
short compareSameWordAsc(char *string1, char *string2);
void printIntoText();
//int isInputLegal(int argc)		//判断输入参数是否合法,合法返回1，不合法返回0
//{
//	if (argc == 2)
//		return 1;
//	else
//		return 0;
//}

/*
struct topPhraseStruc{
	int phraseCount = 0;	//出现次数
	WordNode *firstWord = nullptr;	//第一个单词
	WordNode *secondWord = nullptr;	//第二个单词
};
struct topWordStruc{
int wordCount = 0;
WordNode *word = nullptr;
};

*/

void ReadFolder(char *folderName);

int enterNum = 0;

int main(int argc, char** argv)
{
	//初始化
	for(int count = 0; count < 10; count++)
	{
		topTenPhraseList[count].phraseCount = 0;
		topTenWordList[count].wordCount = 0;
	}
	long handle;
	struct _finddata_t fileinfo;

	if (argc != 2)		//输入是否合法？
		exit(0);


	//在filepath上进行修饰

	/*若要一个byte不漏地读入整个文件，只能采用二进制方式打开*/
	if(is_dir(argv[1]))				//如果输入的是文件夹
	{
		ReadFolder(argv[1]);
	}
	else
	{
		printf("%s\n", argv[1]);
		enterNum++;



		char *fileStream = testFileRead(argv[1]);
		anothermain(fileStream);
		free(fileStream);

	}
	FrequencyTopTen();
	printIntoText();
	
	return 0;
}

void ReadFolder(char *folderName)
{
	char filename[256];
	long handle;
	struct _finddata_t fileinfo;

	filename[0] = '\0';
	strcat_s(filename, 256, folderName);
	strcat_s(filename, 256, "\\*");
	if((handle = _findfirst(filename, &fileinfo)) == -1l)
	{

	}
	else
	{
		if(*fileinfo.name != '.')
		{
			filename[0] = '\0';			//置零
			strcat_s(filename, 256, folderName);
			strcat_s(filename, 256, "\\");
			strcat_s(filename, 256, fileinfo.name);
			if(is_dir(filename))
			{
				ReadFolder(filename);
			}
			else
			{
				printf("%s\n", filename);

				enterNum++;
				char *fileStream = testFileRead(filename);
				anothermain(fileStream);
				free(fileStream);
			}

		}
		while(_findnext(handle, &fileinfo) == 0)
		{
			if(*fileinfo.name != '.')
			{
				filename[0] = '\0';
				strcat_s(filename, 256, folderName);
				strcat_s(filename, 256, "\\");
				strcat_s(filename, 256, fileinfo.name);

				if(is_dir(filename))
				{
					ReadFolder(filename);
				}
				else
				{
					enterNum++;
					printf("%s\n", filename);

					char *fileStream = testFileRead(filename);
					anothermain(fileStream);
					free(fileStream);
				}
			}

		}
		_findclose(handle);
		return;
	}
}

bool is_dir(const char* path)
{
	struct _stat buf = {0};
	_stat(path, &buf);
	return buf.st_mode & _S_IFDIR;
}
char *testFileRead(char *filename)
{
	char *text;
	FILE *pf;
	fopen_s(&pf, filename, "r");
	fseek(pf, 0, SEEK_END);
	long lSize = ftell(pf);
	//用完后需要将内存free掉
	text = (char *)malloc(lSize + 1);
	rewind(pf);
	fread(text, 1, lSize, pf);
	text[lSize] = '\0';
	fclose(pf);
	return text;
}
NextWord *NextWordArrayInit()
{
	NextWord *wordArray = (NextWord *)malloc(NW_INITSIZE * sizeof(NextWord));		//先初始化十个的信息											优化sizeof
	wordArray->nextWord = nullptr;
	wordArray->nextWordCount = 0;
	return wordArray;
}


char word[MAXLENGTH];	//暂存单词
/*struct wordNode{
	int wordCount = 0;
	char *coreString = nullptr;
	char *suffixString = nullptr;
	int phraseNum = 0;
	int maxPhraseNum = 0;
	NextWord *nextWord = nullptr;		//数组形式	构成词组的下一次单词
	WordNode *next = nullptr;
};

struct _nextWord{

	WordNode *nextWord = nullptr;
	int nextWordCount = 0;
};

struct topPhraseStruc{
	int phraseCount = 0;	//出现次数
	WordNode *firstWord = nullptr;	//第一个单词
	WordNode *secondWord = nullptr;	//第二个单词
};

struct topWordStruc{
	int wordCount = 0;
	WordNode *word = nullptr;
};*/
void anothermain(char *fileString)
{
	char *offset = fileString;		//偏移量
	char move;			//应该移动
	char temp;
	
	int count;			//计数器
	do
	{

		if(temp = *offset)
		{
			if(temp >= 32 && temp <= 126)
			{
				Characters++;					//是字符，相加
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{		//是分隔符
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[0] = temp;					//记录
					}
				}
				else if(temp >= 49 && temp <= 57)
				{		//是数字
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{		//读取掉所有数字
						Characters++;
						offset++;
					}
					while((*offset >= 49 && *offset <= 57) || (*offset >= 65 && *offset <= 90) || (*offset >= 97 && *offset <= 122))
					{
						Characters++;
						offset++;
					}
					continue;
				}
				else
				{//是分隔符
					offset++;
					continue;
				}
			}
			else
			{
				if(*offset == 10)
					enterNum++;
				offset++;
				continue;
			}
		}
		else
			break;
		if(temp = *offset)
		{
			if(temp >= 32 && temp <= 126)
			{
				Characters++;					//是字符，相加
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{		//是分隔符
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[1] = temp;					//记录
					}
				}
				else if(temp >= 49 && temp <= 57)
				{		//是数字
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{		//读取掉所有数字
						Characters++;
						offset++;
					}
					while((*offset >= 49 && *offset <= 57) || (*offset >= 65 && *offset <= 90) || (*offset >= 97 && *offset <= 122))
					{
						Characters++;
						offset++;
					}
					continue;
				}
				else
				{//是分隔符
					
					offset++;
					continue;
				}
			}
			else
			{
				if(*offset == 10)
					enterNum++;
				offset++;
				continue;
			}
		}
		else
			break;
		if(temp = *offset)
		{
			if(temp >= 32 && temp <= 126)
			{
				Characters++;					//是字符，相加
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{		//是分隔符
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[2] = temp;					//记录
					}
				}
				else if(temp >= 49 && temp <= 57)
				{		//是数字
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{		//读取掉所有数字
						Characters++;
						offset++;
					}
					while((*offset >= 49 && *offset <= 57) || (*offset >= 65 && *offset <= 90) || (*offset >= 97 && *offset <= 122))
					{
						Characters++;
						offset++;
					}
					continue;
				}
				else
				{//是分隔符
					offset++;
					continue;
				}
			}
			else
			{
				if(*offset == 10)
					enterNum++;
				offset++;
				continue;
			}
		}
		else
			break;

		if(temp = *offset)
		{
			if(temp >= 32 && temp <= 126)
			{
				Characters++;					//是字符，相加
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{		//是分隔符
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[3] = temp;					//记录
						for(count = 4; isLetterOrNum(*offset); offset++, count++)//有重复操作指针的嫌疑，优化
						{//只要是字母或者数字就一直写入临时数组word
							Characters++;
							word[count] = *offset;
						}
						word[count] = '\0';		//末尾加上字符串结束符
						NumOfWords++;
						SolveTheWord(word);
					}
				}
				else if(temp >= 49 && temp <= 57)
				{		//是数字
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{		//读取掉所有数字
						Characters++;
						offset++;
					}
					while((*offset >= 49 && *offset <= 57) || (*offset >= 65 && *offset <= 90) || (*offset >= 97 && *offset <= 122))
					{
						Characters++;
						offset++;
					}
					continue;
				}

				else
				{//是分隔符
					offset++;
					continue;
				}
			}
			else
			{
				if(*offset == 10)
					enterNum++;
				offset++;
				continue;
			}
		}
		else
			break;


	}
	while(*offset != '\0');
}


//单词定义：至少以4个英文字母开头，跟上字母数字符号，单词以分隔符分割，不区分大小写。
char isNotAWord(char *offset)		//判断以offset起始的字符串是否为一个单词，因为如果不是一个单词，就可以忽略不只一个字符，所以返回应该移动的步数
{
	char temp;			//避免重复的指针操作
	temp = *offset;
	if(temp >= 32 && temp <= 126)
	{
		Characters++;
	}
	if(!isALetter(temp))
		return 1;
	temp = *(offset + 1);
	if(temp >= 32 && temp <= 126)
	{
		Characters++;
	}
	if(!isALetter(temp))
		return 2;
	temp = *(offset + 2);
	if(temp >= 32 && temp <= 126)
	{
		Characters++;
	}
	if(!isALetter(temp))
		return 3;
	temp = *(offset + 3);
	if(temp >= 32 && temp <= 126)
	{
		Characters++;
	}
	if(!isALetter(temp))
		return 4;
	return 0;
}

void SolveTheWord(char *word)
{
	int location = 0;
	location += (*(word) > 96 ? *(word)-97 : *(word)-65) * 17576;
	location += (*(word + 1) > 96 ? *(word + 1) - 97 : *(word + 1) - 65) * 676;
	location += (*(word + 2) > 96 ? *(word + 2) - 97 : *(word + 2) - 65) * 26;
	location += *(word + 3) > 96 ? *(word + 3) - 97 : *(word + 3) - 65;
	if(location == 136896)
		int a = 0;
	if((HashSet[location]) == NULL)
	{		//此处为空
		HashSet[location] = WordNodeInit();		//则创建头结点
	}
	PushWordIntoList(HashSet[location], word);		//将单词写入链表
	PushPhraseIntoList();
}

WordNode *WordNodeInit()
{		//初始化空位
	WordNode *p = (WordNode *)malloc(sizeof(WordNode));	//优化sizeof		28 sizeof(WordNode)
	p->next = nullptr;
	p->nextWord = nullptr;
	return p;
}
/*struct wordNode{
	int wordCount = 0;
	char *coreString = nullptr;
	char *suffixString = nullptr;
	int phraseNum = 0;
	int maxPhraseNum = 0;
	NextWord *nextWord = nullptr;		//数组形式	构成词组的下一次单词
	WordNode *next = nullptr;
};

struct _nextWord{

	WordNode *nextWord = nullptr;
	int nextWordCount = 0;
};

struct topPhraseStruc{
	int phraseCount = 0;	//出现次数
	WordNode *firstWord = nullptr;	//第一个单词
	WordNode *secondWord = nullptr;	//第二个单词
};

struct topWordStruc{
	int wordCount = 0;
	WordNode *word = nullptr;
};*/
void PushWordIntoList(WordNode *p, char* word)
{//将word填入
	DevideWord(word);						//将单词分为主体core和后缀部分，分别写入全局变量coreWordTemp，和suffixWordTemp
	int judge;
	for(; p->next; p = p->next)
	{				//带头结点
		if(!isSameWord(p->next->coreString, coreWordTemp))
		{		//如果是同一个单词，只需要增加出现次数记录
			p->next->wordCount++;
			if((judge = compareSameWordAsc(p->next->coreString, coreWordTemp) > 0))
			{//则存储后者
				free(p->next->coreString);
				p->next->coreString = MakeString(coreWordTemp);
			}
			if((isWordEqual(p->next->suffixString, suffixWordTemp)) > 0)
			{
				free(p->next->suffixString);
				p->next->suffixString = MakeString(suffixWordTemp);
			}
			if(LastWord)//组成词组的第二个单词已经写入过，说明至少已经录过最开始两个单词，此处用于词组
			{
				FirstWord = LastWord;		//向前移动
				LastWord = p->next;
			}
			else
			{//如果LastWord还没有写入
				if(FirstWord)
				{//第一个已经写入了，但是第二个还没有
					LastWord = p->next;
				}
				else
				{//说明这是第一个单词
					FirstWord = p->next;
				}
			}
			return;
		}
	}//到此处说明没有重复的，则重新开辟空间
	p->next = WordNodeInit(coreWordTemp, suffixWordTemp);
	p->next->next = NULL;
	if(LastWord)//第二个单词已经存在，说明至少已经录过最开始两个单词
	{
		FirstWord = LastWord;
		LastWord = p->next;
	}
	else
	{//如果LastWord还没有写入
		if(FirstWord)
		{//第一个已经写入了，但是第二个还没有
			LastWord = p->next;
		}
		else
		{//说明这是第一个单词
			FirstWord = p->next;
		}
	}
	return;
}

void DevideWord(char *word)
{
	char *temp1, *temp2;			//temp1指向suffix的最后一位, temp2指向core的最后一位
	int count;
	for(temp1 = word; *temp1 != '\0'; temp1++);
	temp1--;
	for(temp2 = temp1; *temp2 < 58; temp2--);
	for(count = 0; word <= temp2; word++, count++)
		coreWordTemp[count] = *word;
	coreWordTemp[count] = '\0';														//添加结束符
	for(count = 0; word <= temp1; word++, count++)
		suffixWordTemp[count] = *word;
	suffixWordTemp[count] = '\0';													//添加结束符
	return;
}

char isWordEqual(char *string1, char *string2)		//比较string1和string2的ascii码值，1小于2则返回负数，1大于2则返回正数
{
	for(; *string1 && *string2; string1++, string2++)
	{
		if(*string1 != *string2)
			return (*string1 - *string2);
	}
	if(*string1)//string1还有
		return 1;
	else if(*string2)
		return -1;
	return 0;

}
char isSameWord(char *string1, char *string2)
{//相同则返回1，不相同则返回0
	while(*string1 && *string2)
	{
		if(*string1 == *string2 || *string1 == *string2 - 32 || *string1 - 32 == *string2)
		{
			string1++;
			string2++;
		}
		else
			return 0;
		if(*string1 || *string2)
			return 0;
		return 1;

	}
}

short compareSameWordAsc(char *string1, char *string2)
{//1串ascii码更大返回1，2串更大返回-1，完全相同返回0
	while(*string1)
	{
		if(*string1 > *string2)
			return 1;
		else if(*string1 < *string2)
			return -1;
		string1++;
		string2++;
	}
	return 0;
}

WordNode *WordNodeInit(char *coreWord, char *suffixWord)
{
	WordNode *p = (WordNode *)malloc(sizeof(WordNode));
	p->coreString = MakeString(coreWord);
	p->suffixString = MakeString(suffixWord);
	p->wordCount = 1;
	p->phraseNum = 0;
	p->next = nullptr;
	p->nextWord = nullptr;
	return p;
}

void PushPhraseIntoList()
{
	if(!LastWord)//还没有凑齐一个词组
		return;

	WordNode *temp;
	NextWord *temp1;
	int newSize;
	temp = FirstWord;
	int count;


	if(temp->nextWord == NULL)
	{//那么还没有建表
		temp->nextWord = NextWordArrayInit();
		temp->maxPhraseNum = NW_INITSIZE;
	}
	if((temp->phraseNum) == temp->maxPhraseNum)//已经达到上限
	{
		newSize = temp->maxPhraseNum + INCREAMENT;
		temp->maxPhraseNum = newSize;
		temp->nextWord = (NextWord *)realloc(temp->nextWord, (newSize) * sizeof(NextWord));						//优化sizeof
	}
	for(temp1 = temp->nextWord, count = temp->phraseNum; count > 0; temp1++, count--)
	{
		if(!isSameWord(temp1->nextWord->coreString, LastWord->coreString))
		{
			temp1->nextWordCount++;
			return;
			
		}
	}
	temp1->nextWordCount++;
	temp1->nextWord = LastWord;
	FirstWord->phraseNum++;
	return;
}

void FrequencyTopTen()
{
	for(int count1 = 0; count1 < 456976; count1++)
	{		//遍历哈希表
		WordNode *p = HashSet[count1];				//带头结点，深层一个个查询
		if(!p || !(p->next))
			continue;
		else
		{
			p = p->next;									//最后一个单词没有后一个
			while(p)
			{
				WordTen(p);		//输入WordNode节点，读取其中信息
				PhraseTen(p);		//输入WordNode节点，读取其中信息
				p = p->next;
			}
		}

	}
}
/*
struct TopWordStruc{
int wordCount = 0;
WordNode *word = NULL;
};
*/
void WordTen(WordNode *wordPointer)
{//数组

	int shortest = topTenWordList[9].wordCount;
	int wordCount = wordPointer->wordCount;
	if(wordCount < shortest)
		return;
	if(wordCount == shortest)
	{
		if(compareWord(wordPointer, topTenWordList[9].word) > 0)	//如果比最后一个的ascii码小，则可以抛弃最后一个,反之如果大，则抛弃wordPointer
			return;
	}
	//现在末尾一个肯定消除
	for(int count = 8; count >= 0; count--)
	{
		if(wordPointer->wordCount > topTenWordList[count].wordCount)
		{
			//则将此处的往后移动
			topTenWordList[count + 1].word = topTenWordList[count].word;
			topTenWordList[count + 1].wordCount = topTenWordList[count].wordCount;
		}
		else if(wordPointer->wordCount < topTenWordList[count].wordCount)
		{
			//则应存在下一位
			topTenWordList[count + 1].word = wordPointer;
			topTenWordList[count + 1].wordCount = wordPointer->wordCount;
			return;
		}
		else
		{//次数相同则比较ASCII码，小的进行存储
			if((compareWord(wordPointer, topTenWordList[count].word)) > 0)
			{
				//如果比这个ascii码大，则存在下一个
				topTenWordList[count + 1].word = wordPointer;
				topTenWordList[count + 1].wordCount = wordPointer->wordCount;
				return;
			}
			else
			{//只可能比它小
			//但前面可能还有次数相同，但ASCII码更小的，向后移动，继续循环
				topTenWordList[count + 1].word = topTenWordList[count].word;
				topTenWordList[count + 1].wordCount = topTenWordList[count].wordCount;
			}
		}
	}
	//如果能达到这里，说明它是最大的
	topTenWordList[0].word = wordPointer;
	topTenWordList[0].wordCount = wordPointer->wordCount;
	return;
}

char compareWord(WordNode *node1, WordNode *node2)
{//比较两个单词的核心部分，node1比node2的ascii大则返回正实数
	char *temp1 = node1->coreString;
	char *temp2 = node2->coreString;
	for(; *temp1 && *temp2; temp1++, temp2++)
	{//当还未到结尾时
		if(*temp1 != *temp2)
			return (*temp1 - *temp2);
	}
	if(*temp1)
		return 1;
	else if(*temp2)
		return -1;
	else
		return 0;

}


/*
struct TopPhraseStruc{
int phraseCount = 0;	//出现次数
WordNode *firstWord = NULL;	//第一个单词
WordNode *secondWord = NULL;	//第二个单词
};


struct NextWord{
int nextWordCount = 0;
WordNode *nextWord = NULL;
};

*/
void PhraseTen(WordNode *wordPointer)
{//数组
	int shortest = topTenPhraseList[9].phraseCount;
	int judge1, judge2;
	NextWord *nextWordTemp = wordPointer->nextWord;
	for(int count = 0; count < wordPointer->phraseNum; count++)
	{//遍历所有的词组
		if(nextWordTemp[count].nextWordCount < shortest)//若比最小值还小，不用再多考虑
			return;
		else
		{
			if(nextWordTemp[count].nextWordCount == shortest)
			{

				if((judge2 = compareWord(wordPointer, topTenPhraseList[9].firstWord)) > 0)//先比较第一个单词
					return;
				else if(judge2 == 0)
				{
					if(compareWord(nextWordTemp[count].nextWord, topTenPhraseList[9].secondWord) > 0)//再比较第二个单词
						return;
				}
				//现在最后一个肯定得替换掉
			}
			for(int count1 = 8; count1 >= 0; count1--)
			{
				if((judge1 = nextWordTemp[count].nextWordCount - topTenPhraseList[count1].phraseCount) < 0)
				{//这个较小，停止移动
					topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
					topTenPhraseList[count1 + 1].firstWord = wordPointer;
					topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
					return;
				}
				else if(judge1 == 0)
				{
					if((judge2 = compareWord(wordPointer, topTenPhraseList[count1].firstWord)) > 0)
					{//先比较第一个单词
						topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
						topTenPhraseList[count1 + 1].firstWord = wordPointer;
						topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
						return;
					}
					else if(judge2 == 0)
					{
						if(compareWord(nextWordTemp[count].nextWord, topTenPhraseList[9].secondWord) > 0)
						{//再比较第二个单词
							topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
							topTenPhraseList[count1 + 1].firstWord = wordPointer;
							topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
							return;
						}
						else
						{//需要与前面的词组进行比较
							topTenPhraseList[count1 + 1].phraseCount = topTenPhraseList[count1].phraseCount;
							topTenPhraseList[count1 + 1].firstWord = topTenPhraseList[count1].firstWord;
							topTenPhraseList[count1 + 1].secondWord = topTenPhraseList[count1].secondWord;

						}
					}
					else
					{
						topTenPhraseList[count1 + 1].phraseCount = topTenPhraseList[count1].phraseCount;
						topTenPhraseList[count1 + 1].firstWord = topTenPhraseList[count1].firstWord;
						topTenPhraseList[count1 + 1].secondWord = topTenPhraseList[count1].secondWord;
					}
				}
				else
				{//这个更大，向左移动
					topTenPhraseList[count1 + 1].phraseCount = topTenPhraseList[count1].phraseCount;
					topTenPhraseList[count1 + 1].firstWord = topTenPhraseList[count1].firstWord;
					topTenPhraseList[count1 + 1].secondWord = topTenPhraseList[count1].secondWord;

				}
			}
			//如果能达到这里，说明这个词组是最大的
			topTenPhraseList[0].phraseCount = nextWordTemp->nextWordCount;
			topTenPhraseList[0].firstWord = wordPointer;
			topTenPhraseList[0].secondWord = nextWordTemp[count].nextWord;
			return;
		}
	}

}

char *MakeString(char *string)
{

	char *p;
	short length = strlen(string);
	p = (char *)malloc(length + 1);//sizeof优化																													用maxlength分配，还是根据长度分配 lengthof
	strcpy_s(p, length + 1, string);
	return p;
}

/*struct TopWordStruc{
int wordCount = 0;
WordNode *word = NULL;
};*/
void printIntoText()
{
	FILE *fpOut = NULL;
	errno_t err;
	err = fopen_s(&fpOut, "result.txt", "w");

	if(err == 0)  // if it is not a false location
	{
		fprintf(fpOut, "<characterNum>\t%d\n", Characters);
		fprintf(fpOut, "<lineNum>\t%d\n", enterNum);
		fprintf(fpOut, "<wordNum>\t%d\n", NumOfWords);

		fprintf(fpOut, "\nMax Frequency Words\n");
		
		for(int count = 0; count < 10; count++)
		{
			if (topTenWordList[count].wordCount == 0)
				break;
			fprintf(fpOut,"%d\t%s\t%s\n", topTenWordList[count].wordCount, topTenWordList[count].word->coreString, topTenWordList[count].word->suffixString);
		}

		fprintf(fpOut, "\nMax Frequency Phrases\n");

		for(int count = 0; count < 10; count++)
		{
			if (topTenPhraseList[count].phraseCount == 0)
				break;
			fprintf(fpOut,"%d\t%s%s\t%s%s\n", topTenPhraseList[count].phraseCount, topTenPhraseList[count].firstWord->coreString, topTenPhraseList[count].firstWord->suffixString, topTenPhraseList[count].secondWord->coreString, topTenPhraseList[count].secondWord->suffixString);
		}
	}

	fclose(fpOut);
	
	
}
/*struct TopPhraseStruc{
int phraseCount = 0;	//出现次数
WordNode *firstWord = NULL;	//第一个单词
WordNode *secondWord = NULL;	//第二个单词
};
*/
void printPhraseList()
{
	for(int count = 0; count < 10; count++)
	{
		if(topTenPhraseList[count].phraseCount == 0)
			return;
		printf("%d\t%s%s\t%s%s\n", topTenPhraseList[count].phraseCount, topTenPhraseList[count].firstWord->coreString, topTenPhraseList[count].firstWord->suffixString, topTenPhraseList[count].secondWord->coreString, topTenPhraseList[count].secondWord->suffixString);
	}
}
