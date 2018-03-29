// wordFrequencyStatistics.cpp: 
//

#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "sys/types.h"
#include "fstream"
#include "dirent.h"
using namespace std;

unsigned long characterNum;
unsigned long wordNum;
unsigned long lineNum;

#define wordArrayLength 10000
#define wordStrLength   1024
#define filePathLength  260
#define alphabet		26
#define nextWordNum		100
#define topFrequencyWordNum 10
#define topFrequencyPhaseNum 10

struct wordInfo {
	char* wordStr;
	char**  nextWordPoint;
	int*  nextWordFrequency;
	int	  presentNextWordNum;
	int   frequency;
	int   strlength;
	int   wordLength;//not include the final digits
};
struct alphaArray {
	wordInfo* wordArray;
	int       presentWordArrayLength;
};

struct wordStatisticsResult {
	char* wordStr;
	int   wordFrequency;
};
struct phaseStatisticsResult {
	char* firstStr;
	char* secondStr;
	int   phaseFrequency;
};

void dictionaryInit(struct alphaArray* dictionary);
void getFilePath(char* filePath);
void traverseFileandCount(char* path, struct alphaArray* dictionary);
void count(char* path, struct alphaArray* dictionary);
void storePhaseInfo(struct wordInfo* lastWordInfo, struct wordInfo* presentWordInfo);
void characterNumandLineNum(char ch);
int isDigitorAlpha(char ch);
int isAlpha(char ch);
int isWord(char* tempWordArray);
unsigned long storeTempWord(struct alphaArray* dictionary, char* tempWordArray, int lastAlphaPosition);
unsigned long ELFHash(char* tempWordArray, int lastAlphaPosition);
void getOffset(int &offset, char ch);
int isEmpty(char* str);
int isDifferent(wordInfo* dictionaryPoint, char* tempWordArray, int lastAlphaPosition);
void enlargeWordArrayLength(struct alphaArray* page);
void initNewWordStr(wordInfo* dictionaryPoint, int j);
void enlargeStrLength(struct alphaArray* page, int hash);
void topFrequencyWordStatistics(struct alphaArray* dictionary, struct wordStatisticsResult* topFrequencyWord);
void updateTopFrequencyWord(struct wordStatisticsResult* topFrequencyWord, struct wordInfo* dictionary_i_j, int &minWordFrequency);
void sortTopFrequencyWord(struct wordStatisticsResult* topFrequencyWord);
void topFrequencyPhaseStatistics(struct alphaArray* dictionary, struct phaseStatisticsResult* topFrequencyPhase);
void updateTopFrequencyPhase(struct phaseStatisticsResult* topFrequencyPhase, wordInfo* dictionary_i_j, int offset, int &minPhaseFrequency);
void sortTopFrequencyPhase(struct phaseStatisticsResult* topFrequencyPhase);
void outputResult(struct alphaArray* dictionary);
void outputToFile(char*path,struct wordStatisticsResult* topFrequencyWord, struct phaseStatisticsResult* topFrequencyPhase);
void dictionaryDestroy(struct alphaArray* dictionary);

int main(int argc, char *argv[])
{
	char* filePath;
	struct alphaArray dictionary[alphabet];
	struct wordStatisticsResult topFrequencyWord[topFrequencyWordNum];
	struct phaseStatisticsResult topFrequencyPhase[topFrequencyPhaseNum];
	dictionaryInit(dictionary);
	filePath=*(argv+1);
	//getFilePath(filePath);
	traverseFileandCount(filePath, dictionary);
	outputResult(dictionary);
	topFrequencyWordStatistics(dictionary, topFrequencyWord);
	topFrequencyPhaseStatistics(dictionary, topFrequencyPhase);
	outputToFile(filePath,topFrequencyWord, topFrequencyPhase);
	dictionaryDestroy(dictionary);
	return 0;
}

void dictionaryInit(struct alphaArray* dictionary)
{
	int i, j, k;
	characterNum = 0;
	wordNum = 0;
	lineNum = 0;
	for (i = 0; i < alphabet; i++)
	{
		(dictionary + i)->wordArray = (wordInfo*)malloc(sizeof(wordInfo)*wordArrayLength);
		(dictionary + i)->presentWordArrayLength = wordArrayLength;
		if ((dictionary + i)->wordArray == NULL) exit(-1);
		for (j = 0; j < (dictionary + i)->presentWordArrayLength; j++)
		{
			((dictionary + i)->wordArray + j)->wordStr = (char*)malloc(sizeof(char)*wordStrLength);
			if (((dictionary + i)->wordArray + j)->wordStr == NULL) exit(-1);
			*(((dictionary + i)->wordArray + j)->wordStr) = '\0';
			((dictionary + i)->wordArray + j)->frequency = 0;
			((dictionary + i)->wordArray + j)->strlength = wordStrLength;
			((dictionary + i)->wordArray + j)->wordLength = 0;
			((dictionary + i)->wordArray + j)->nextWordPoint = (char**)malloc(sizeof(char*)*nextWordNum);
			if (((dictionary + i)->wordArray + j)->nextWordPoint == NULL) exit(-1);
			((dictionary + i)->wordArray + j)->nextWordFrequency = (int*)malloc(sizeof(int)*nextWordNum);
			if (((dictionary + i)->wordArray + j)->nextWordFrequency == NULL) exit(-1);
			for (k = 0; k < nextWordNum; k++)
			{
				*(((dictionary + i)->wordArray + j)->nextWordPoint + k) = NULL;
				*(((dictionary + i)->wordArray + j)->nextWordFrequency + k) = 0;
			}
			((dictionary + i)->wordArray + j)->presentNextWordNum = nextWordNum;
		}
	}
}

void getFilePath(char* filePath)
{
	printf("Please input filepath:");
	scanf("%s", filePath);
}

void traverseFileandCount(char* path, struct alphaArray* dictionary)
{
	DIR *pDir; 
	struct dirent *ent=NULL; 
	int i = 0;
	char childpath[512]; 
	pDir = opendir(path);
	memset(childpath, 0, sizeof(childpath)); 
	while ((ent = readdir(pDir)) != NULL)
	{
		if (ent->d_type&DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;
			sprintf(childpath, "%s/%s", path, ent->d_name);
			traverseFileandCount(childpath, dictionary);
		}
		else
		{
			sprintf(childpath, "%s/%s", path, ent->d_name);
			count(childpath, dictionary);
		}
	}
}


void count(char* path, struct alphaArray* dictionary)
{
	FILE* fp;
	bool firstWordSign = 1;
	int i = 0;
	int finalAlphaPosition = 0;
	int tempWordStrLength = wordStrLength;
	int presentWordOffset;
	char ch, *tempWordStr;
	unsigned long hash;
	struct wordInfo* lastWordInfo = NULL, *presentWordInfo = NULL;
	tempWordStr = (char*)malloc(sizeof(char)*wordStrLength);
	if (tempWordStr == NULL) exit(-1);
	fp = fopen(path, "r");
	if (fp==NULL) exit(-1);
	do
	{
		ch = fgetc(fp);
		characterNumandLineNum(ch);
		if (!isDigitorAlpha(ch))
		{
			tempWordStr[i] = '\0';
			if (isWord(tempWordStr))
			{
				hash = storeTempWord(dictionary, tempWordStr, finalAlphaPosition);
				getOffset(presentWordOffset, tempWordStr[0]);
				presentWordInfo = ((dictionary + presentWordOffset)->wordArray + hash);
				if (!firstWordSign)
					storePhaseInfo(lastWordInfo, presentWordInfo);
				lastWordInfo = presentWordInfo;
				firstWordSign = 0;
			}
			i = 0;
			finalAlphaPosition = 0;
			tempWordStr[0] = '\0';
		}
		else
		{
			if (i < wordStrLength)
			{
				if (isAlpha(ch)) finalAlphaPosition = i;
				tempWordStr[i++] = ch;
			}
			/*if (i >= tempWordStrLength)
			{
			tempWordStrLength *= 2;
			tempWordStr = (char*)realloc(tempWordStr, sizeof(char)*tempWordStrLength);
			if (tempWordStr == NULL) exit(-1);
			}*/
		}
	} while (ch != EOF);
	free(tempWordStr);
	lineNum++;
	fclose(fp);
}

void characterNumandLineNum(char ch)
{
	if (ch >= 32 && ch <= 126)
		characterNum++;
	if (ch == '\n' || ch == '\r')
		lineNum++;
}

int isDigitorAlpha(char ch)
{
	if (ch >= '0'&&ch <= '9' || ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z')
		return 1;
	return 0;
}

int isAlpha(char ch)
{
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z') return 1;
	return 0;
}

int isWord(char* tempWordArray)
{
	int i;
	for (i = 0; i < 4; i++)
		if (!isAlpha(tempWordArray[i])) return 0;
	return 1;
}

unsigned long storeTempWord(struct alphaArray* dictionary, char* tempWordArray, int lastAlphaPosition)
{
	unsigned long hash = 0;
	int i = 0, j = 0, offset;
	char* wordstrPoint;
	struct alphaArray* page;
	hash = ELFHash(tempWordArray, lastAlphaPosition);
	getOffset(offset, tempWordArray[0]);
	page = dictionary + offset;
	hash = hash % (page->presentWordArrayLength);
	//hash=hash%wordArrayLength;
	wordstrPoint = (page->wordArray + hash)->wordStr;
	while (!isEmpty(wordstrPoint) && isDifferent(page->wordArray + hash, tempWordArray, lastAlphaPosition))
	{
		i++;
		if (i > (page->presentWordArrayLength))
		{
			enlargeWordArrayLength(page);
			i = 0;
		}
		hash += i * i;
		hash = hash % (page->presentWordArrayLength);
		wordstrPoint = (page->wordArray + hash)->wordStr;
	}
	/*while ((int)strlen(tempWordArray) >= (page->wordArray + hash)->strlength)
	enlargeStrLength(page, hash);*/
	if ((int)strlen(tempWordArray) >= (page->wordArray + hash)->strlength)
		*(tempWordArray + (page->wordArray + hash)->strlength - 1) = '\0';
	wordstrPoint = (page->wordArray + hash)->wordStr;
	if (isEmpty(wordstrPoint))
	{
		strcpy(wordstrPoint, tempWordArray);
		(page->wordArray + hash)->wordLength = lastAlphaPosition;
	}
	else
	{
		if (strcmp(wordstrPoint, tempWordArray) > 0)
			strcpy(wordstrPoint, tempWordArray);
	}
	(page->wordArray + hash)->frequency++;
	wordNum++;
	return hash;
}

void storePhaseInfo(struct wordInfo* lastWordInfo, struct wordInfo* presentWordInfo)
{
	int i = 0, k = 0;
	bool stored = 0;
	for (i = 0; i < (lastWordInfo->presentNextWordNum);)
	{
		if ((*(lastWordInfo->nextWordFrequency + i)) != 0)
		{
			if ((*(lastWordInfo->nextWordPoint + i)) == presentWordInfo->wordStr && !stored)
			{
				(*(lastWordInfo->nextWordFrequency + i))++;
				stored = 1;
			}
			else
				i++;
		}
		else
			break;
	}
	if (i == (lastWordInfo->presentNextWordNum))
	{
		lastWordInfo->nextWordPoint = (char**)realloc(lastWordInfo->nextWordPoint, sizeof(char*)*(lastWordInfo->presentNextWordNum) * 2);
		if (lastWordInfo->nextWordPoint == NULL) exit(-1);
		lastWordInfo->nextWordFrequency = (int*)realloc(lastWordInfo->nextWordFrequency, sizeof(int)*(lastWordInfo->presentNextWordNum) * 2);
		if (lastWordInfo->nextWordFrequency == NULL) exit(-1);
		for (k = (lastWordInfo->presentNextWordNum); k < (lastWordInfo->presentNextWordNum) * 2; k++)
		{
			*(lastWordInfo->nextWordPoint + k) = NULL;
			*(lastWordInfo->nextWordFrequency + k) = 0;
		}
		(lastWordInfo->presentNextWordNum) *= 2;
	}
	if (!stored)
	{
		*(lastWordInfo->nextWordPoint + i) = presentWordInfo->wordStr;
		(*(lastWordInfo->nextWordFrequency + i))++;
		stored = 1;
	}
}

unsigned long ELFHash(char* tempWordArray, int lastAlphaPosition)
{
	unsigned long hash = 0, i = 0, x = 0;
	char *hashStr;
	hashStr = (char*)malloc(sizeof(char)*(lastAlphaPosition + 1));
	if (hashStr == NULL) exit(-1);
	for (i = 0; i <= (unsigned long)lastAlphaPosition; i++)
	{
		if (tempWordArray[i] >= 'a'&&tempWordArray[i] <= 'z' || tempWordArray[i] >= '0'&&tempWordArray[i] <= '9')
			*(hashStr + i) = tempWordArray[i];
		else
			*(hashStr + i) = tempWordArray[i] - 'A' + 'a';
	}
	for (i = 0; i <= (unsigned long)lastAlphaPosition; i++)
	{
		hash = (hash << 4) + *(hashStr + i);
		if ((x = hash & 0xf0000000) != 0)
		{
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	hash &= 0x7fffffff;
	free(hashStr);
	return hash;
}

void getOffset(int &offset, char ch)
{
	if (ch >= 'a'&&ch <= 'z')
		offset = ch - 'a';
	else
		offset = ch - 'A';
}

int isEmpty(char* str)
{
	if (str[0] == '\0')
		return 1;
	return 0;
}

int isDifferent(wordInfo* dictionaryPoint, char* tempWordArray, int lastAlphaPosition)
{
	int i = 0;
	bool sameSign = 1;
	if (dictionaryPoint->wordLength != lastAlphaPosition)
		return 1;
	else
		for (i = 0; i <= lastAlphaPosition; i++)
		{
			if (*(dictionaryPoint->wordStr + i) == *(tempWordArray + i) && sameSign)
				sameSign = 1;
			else if ((*(dictionaryPoint->wordStr + i) == *(tempWordArray + i) - 'a' + 'A') && sameSign)
				sameSign = 1;
			else if ((*(dictionaryPoint->wordStr + i) == *(tempWordArray + i) - 'A' + 'a') && sameSign)
				sameSign = 1;
			else
				sameSign = 0;
		}
	return !sameSign;
}

void enlargeWordArrayLength(struct alphaArray* page)
{
	int j;
	page->wordArray = (wordInfo*)realloc(page->wordArray, sizeof(wordInfo)*(page->presentWordArrayLength) * 2);
	if (page->wordArray == NULL) exit(-1);
	for (j = (page->presentWordArrayLength); j < (page->presentWordArrayLength) * 2; j++)
		initNewWordStr(page->wordArray, j);
	page->presentWordArrayLength *= 2;
}

void initNewWordStr(wordInfo* dictionaryPoint, int j)
{
	int i;
	(dictionaryPoint + j)->wordStr = (char*)malloc(sizeof(char)*wordStrLength);
	if ((dictionaryPoint + j)->wordStr == NULL) exit(-1);
	*((dictionaryPoint + j)->wordStr) = '\0';
	(dictionaryPoint + j)->frequency = 0;
	(dictionaryPoint + j)->strlength = wordStrLength;
	(dictionaryPoint + j)->wordLength = 0;
	(dictionaryPoint + j)->nextWordPoint = (char**)malloc(sizeof(char*)*nextWordNum);
	if ((dictionaryPoint + j)->nextWordPoint == NULL) exit(-1);
	(dictionaryPoint + j)->nextWordFrequency = (int*)malloc(sizeof(int)*nextWordNum);
	if ((dictionaryPoint + j)->nextWordFrequency == NULL) exit(-1);
	for (i = 0; i < nextWordNum; i++)
	{
		*((dictionaryPoint + j)->nextWordPoint + i) = NULL;
		*((dictionaryPoint + j)->nextWordFrequency + i) = 0;
	}
	(dictionaryPoint + j)->presentNextWordNum = nextWordNum;
}

void enlargeStrLength(struct alphaArray* page, int hash)
{
	(page->wordArray + hash)->wordStr = (char*)realloc((page->wordArray + hash)->wordStr, sizeof(char)*((page->wordArray + hash)->strlength) * 2);
	if ((page->wordArray + hash)->wordStr == NULL) exit(-1);
	(page->wordArray + hash)->strlength *= 2;
}

void topFrequencyWordStatistics(struct alphaArray* dictionary, struct wordStatisticsResult* topFrequencyWord)
{
	int i = 0, j = 0;
	int minWordFrequency = 0;
	for (i = 0; i < topFrequencyWordNum; i++)
	{
		(topFrequencyWord + i)->wordStr = NULL;
		(topFrequencyWord + i)->wordFrequency = 0;
	}
	for (i = 0; i < alphabet; i++)
	{
		for (j = 0; j < (dictionary + i)->presentWordArrayLength; j++)
		{
			if (((dictionary + i)->wordArray + j)->frequency > minWordFrequency)
				updateTopFrequencyWord(topFrequencyWord, ((dictionary + i)->wordArray + j), minWordFrequency);
		}
	}
	sortTopFrequencyWord(topFrequencyWord);
	puts("Top 10 word:");
	for (i = 0; i < topFrequencyWordNum; i++)
		printf("%s\t%d\n", (topFrequencyWord + i)->wordStr, (topFrequencyWord + i)->wordFrequency);
	printf("\n");
}

void updateTopFrequencyWord(struct wordStatisticsResult* topFrequencyWord, struct wordInfo* dictionary_i_j, int &minWordFrequency)
{
	int i = 0;
	for (i = 0; i < topFrequencyWordNum; i++)
	{
		if ((topFrequencyWord + i)->wordFrequency == minWordFrequency)
		{
			(topFrequencyWord + i)->wordStr = dictionary_i_j->wordStr;
			(topFrequencyWord + i)->wordFrequency = dictionary_i_j->frequency;
			minWordFrequency = dictionary_i_j->frequency;
		}
	}
	for (i = 0; i < topFrequencyWordNum; i++)
	{
		if ((topFrequencyWord + i)->wordFrequency < minWordFrequency)
			minWordFrequency = (topFrequencyWord + i)->wordFrequency;
	}
}

void sortTopFrequencyWord(struct wordStatisticsResult* topFrequencyWord)
{
	int i = 0, j = 0;
	int minWordFrequency;
	int minWordFrequencyPosition;
	struct wordStatisticsResult tempWord;
	for (i = 0; i < topFrequencyWordNum - 1; i++)
	{
		minWordFrequency = topFrequencyWord->wordFrequency;
		minWordFrequencyPosition = 0;
		for (j = 0; j < topFrequencyWordNum - i; j++)
		{
			if ((topFrequencyWord + j)->wordFrequency < minWordFrequency)
			{
				minWordFrequency = (topFrequencyWord + j)->wordFrequency;
				minWordFrequencyPosition = j;
			}
		}
		tempWord.wordStr = (topFrequencyWord + minWordFrequencyPosition)->wordStr;
		tempWord.wordFrequency = minWordFrequency;
		(topFrequencyWord + minWordFrequencyPosition)->wordStr = (topFrequencyWord + topFrequencyWordNum - i - 1)->wordStr;
		(topFrequencyWord + minWordFrequencyPosition)->wordFrequency = (topFrequencyWord + topFrequencyWordNum - i - 1)->wordFrequency;
		(topFrequencyWord + topFrequencyWordNum - i - 1)->wordStr = tempWord.wordStr;
		(topFrequencyWord + topFrequencyWordNum - i - 1)->wordFrequency = tempWord.wordFrequency;
	}
}

void topFrequencyPhaseStatistics(struct alphaArray* dictionary, struct phaseStatisticsResult* topFrequencyPhase)
{
	int i = 0, j = 0, k = 0;
	int minPhaseFrequency = 0;
	for (i = 0; i < topFrequencyPhaseNum; i++)
	{
		(topFrequencyPhase + i)->firstStr = NULL;
		(topFrequencyPhase + i)->secondStr = NULL;
		(topFrequencyPhase + i)->phaseFrequency = 0;
	}
	for (i = 0; i < alphabet; i++)
	{
		for (j = 0; j < (dictionary + i)->presentWordArrayLength; j++)
		{
			for (k = 0; k < ((dictionary + i)->wordArray + j)->presentNextWordNum; k++)
			{
				if (*(((dictionary + i)->wordArray + j)->nextWordFrequency + k) > minPhaseFrequency)
					updateTopFrequencyPhase(topFrequencyPhase, ((dictionary + i)->wordArray + j), k, minPhaseFrequency);
			}
		}
	}
	sortTopFrequencyPhase(topFrequencyPhase);
	puts("Top 10 phase:");
	for (i = 0; i < topFrequencyPhaseNum; i++)
		printf("%s %s\t%d\n", (topFrequencyPhase + i)->firstStr, (topFrequencyPhase + i)->secondStr, (topFrequencyPhase + i)->phaseFrequency);
	printf("\n");
}

void updateTopFrequencyPhase(struct phaseStatisticsResult* topFrequencyPhase, wordInfo* dictionary_i_j, int offset, int &minPhaseFrequency)
{
	int i = 0;
	for (i = 0; i < topFrequencyPhaseNum; i++)
	{
		if ((topFrequencyPhase + i)->phaseFrequency == minPhaseFrequency)
		{
			(topFrequencyPhase + i)->firstStr = dictionary_i_j->wordStr;
			(topFrequencyPhase + i)->secondStr = *(dictionary_i_j->nextWordPoint + offset);
			(topFrequencyPhase + i)->phaseFrequency = *(dictionary_i_j->nextWordFrequency + offset);
			minPhaseFrequency = (topFrequencyPhase + i)->phaseFrequency;
		}
	}
	for (i = 0; i < topFrequencyPhaseNum; i++)
	{
		if ((topFrequencyPhase + i)->phaseFrequency < minPhaseFrequency)
			minPhaseFrequency = (topFrequencyPhase + i)->phaseFrequency;
	}
}

void sortTopFrequencyPhase(struct phaseStatisticsResult* topFrequencyPhase)
{
	int i = 0, j = 0;
	int minPhaseFrequency;
	int minPhaseFrequencyPosition;
	struct phaseStatisticsResult tempPhase;
	for (i = 0; i < topFrequencyPhaseNum - 1; i++)
	{
		minPhaseFrequency = topFrequencyPhase->phaseFrequency;
		minPhaseFrequencyPosition = 0;
		for (j = 0; j < topFrequencyPhaseNum - i; j++)
		{
			if ((topFrequencyPhase + j)->phaseFrequency < minPhaseFrequency)
			{
				minPhaseFrequency = (topFrequencyPhase + j)->phaseFrequency;
				minPhaseFrequencyPosition = j;
			}
		}
		tempPhase.firstStr = (topFrequencyPhase + minPhaseFrequencyPosition)->firstStr;
		tempPhase.secondStr = (topFrequencyPhase + minPhaseFrequencyPosition)->secondStr;
		tempPhase.phaseFrequency = minPhaseFrequency;
		(topFrequencyPhase + minPhaseFrequencyPosition)->firstStr = (topFrequencyPhase + topFrequencyPhaseNum - i - 1)->firstStr;
		(topFrequencyPhase + minPhaseFrequencyPosition)->secondStr = (topFrequencyPhase + topFrequencyPhaseNum - i - 1)->secondStr;
		(topFrequencyPhase + minPhaseFrequencyPosition)->phaseFrequency = (topFrequencyPhase + topFrequencyPhaseNum - i - 1)->phaseFrequency;
		(topFrequencyPhase + topFrequencyPhaseNum - i - 1)->firstStr = tempPhase.firstStr;
		(topFrequencyPhase + topFrequencyPhaseNum - i - 1)->secondStr = tempPhase.secondStr;
		(topFrequencyPhase + topFrequencyPhaseNum - i - 1)->phaseFrequency = tempPhase.phaseFrequency;
	}
}

void outputResult(struct alphaArray* dictionary)
{
	int i = 0, j = 0, k = 0;
	puts("Statistics result:");
	printf("characterNum:%lu\n", characterNum);
	printf("wordNum:%lu\n", wordNum);
	printf("lineNum:%lu\n\n", lineNum);
}

void outputToFile(char*path,struct wordStatisticsResult* topFrequencyWord, struct phaseStatisticsResult* topFrequencyPhase)
{
	int i = 0;
	FILE* fp;
	puts("Output file path:");
	puts(path);
	fp=fopen(path, "wb");
	if (fp == NULL) exit(-1);
	fputs("characterNum:", fp);
	fprintf(fp, "%lu\r\n", characterNum);
	fputs("wordNum:", fp);
	fprintf(fp, "%lu\r\n", wordNum);
	fputs("lineNum:", fp);
	fprintf(fp, "%lu\r\n\r\n", lineNum);
	fputs("Top 10 frequency words:\r\n", fp);
	for (i = 0; i < topFrequencyWordNum; i++)
		fprintf(fp, "%s:  %d\r\n", (topFrequencyWord + i)->wordStr, (topFrequencyWord + i)->wordFrequency);
	fputs("\r\n", fp);
	fputs("Top 10 frequency phases:\r\n", fp);
	for (i = 0; i < topFrequencyPhaseNum; i++)
		fprintf(fp, "%s %s:  %d\r\n", (topFrequencyPhase + i)->firstStr, (topFrequencyPhase + i)->secondStr, (topFrequencyPhase + i)->phaseFrequency);
	fputs("\r\n", fp);
	fclose(fp);
}

void dictionaryDestroy(struct alphaArray* dictionary)
{
	int i, j;
	for (i = 0; i < alphabet; i++)
	{
		for (j = 0; j < ((dictionary + i)->presentWordArrayLength); j++)
		{
			free(((dictionary + i)->wordArray + j)->wordStr);
			free(((dictionary + i)->wordArray + j)->nextWordPoint);
			free(((dictionary + i)->wordArray + j)->nextWordFrequency);
		}
		free((dictionary + i)->wordArray);
	}
}
