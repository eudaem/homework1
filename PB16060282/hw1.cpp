#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "dirent.h"
#include "unistd.h"
#include<string>
using namespace std;

#define WORD_MAX_LENGTH 250
#define isLetter(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isNumber(c) ((c) >= '0' && (c) <= '9')
#define HASHMOD 22248619

int characterNum = 0;
int wordNum = 0;
int lineNum = 0;

typedef struct wordNode 
{
	char word[WORD_MAX_LENGTH];
	char wordPreLow[WORD_MAX_LENGTH];//word excluding numbers in the end
	int time;
	struct wordNode *next;
}wordNode, *wordList;

typedef struct phraseNode
{
	int time;
	struct phraseNode *next;
	wordList word1;
	wordList word2;
}phraseNode, *phraseList;

wordList headList[HASHMOD];
phraseList headList_Phrase[HASHMOD];
wordNode wordTop10[11];
phraseNode phraseTop10[11];

void Mystrncpy_lwr(char* &s, char *t, int k)
{
	//copy and change to lower case
	int i;
	for(i = 0; i < k; i++)
	{	
		s[i] = t[i];
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += 32;
	}
	s[i] = '\0';
}

int ELFhash(char* key) 
{
	//Hash function
	unsigned long h = 0, g;
	while (*key)
	{
		h = (h << 4) + *key++;
		g = h & 0xF0000000L;
		if (g)
			h ^= g >> 24;
		h &= ~g;
	}
	return(h % HASHMOD);
}

int ELFhash_Phrase(char* key1, char* key2)
{	
	//Hash function for two words
	unsigned long h = 0, g;
	while (*key1)
	{
		h = (h << 4) + *key1++;
		g = h & 0xF0000000L;
		if (g)
			h ^= g >> 24;
		h &= ~g;
	}
	while (*key2)
	{
		h = (h << 4) + *key2++;
		g = h & 0xF0000000L;
		if (g)
			h ^= g >> 24;
		h &= ~g;
	}
	return(h % HASHMOD);
}

wordList CountFrequency(char word[], int lengthPre) 
{	
	//Count the frequency of words
	char* wordPreLow = new char[lengthPre + 1];
	int h;
	wordList wNode = NULL, p, q = NULL;

	Mystrncpy_lwr(wordPreLow, word, lengthPre);

	h = ELFhash(wordPreLow);

	if(headList[h] == NULL)
	{
		wNode = new wordNode;
		wNode->time = 1;
		strcpy(wNode->word, word);
		strcpy(wNode->wordPreLow, wordPreLow);
		wNode->next = NULL;
		headList[h] = wNode;
		headList[h]->next = NULL;
	}
	else
	{
		p = headList[h];
		while (p != NULL)
		{
			if (strcmp(p->wordPreLow, wordPreLow) == 0)
			{
				p->time++;
				if (strcmp(word, p->word) < 0)
				{
					strcpy(p->word, word);
				}
				return p;
			}
			q = p;
			p = p->next;
		}
		if (p == NULL)
		{
			wNode = new wordNode;
			wNode->time = 1;
			strcpy(wNode->word, word);
			strcpy(wNode->wordPreLow, wordPreLow);
			wNode->next = NULL;
			q->next = wNode;
		}
	}
	return wNode;
}

void CountFrequency_Phrase(wordList word1, wordList word2) 
{
	//Count the frequency of phrases
	if (word1 == NULL || word2 == NULL)
	{
		printf("Empty Pointer\n");
		return;
	}

	int h;
	phraseList pNode, p, q = NULL;
	h = ELFhash_Phrase(word1->wordPreLow, word2->wordPreLow);

	if (headList_Phrase[h] == NULL)
	{
		pNode = new phraseNode;
		pNode->time = 1;
		pNode->word1 = word1;
		pNode->word2 = word2;
		headList_Phrase[h] = pNode;
		headList_Phrase[h]->next = NULL;
	}
	else
	{
		p = headList_Phrase[h];
		while (p != NULL)
		{
			if (strcmp(p->word1->wordPreLow, word1->wordPreLow) == 0 && strcmp(p->word2->wordPreLow, word2->wordPreLow) == 0)
			{
				p->time++;
				break;
			}
			q = p;
			p = p->next;
		}
		if (p == NULL)
		{
			pNode = new phraseNode;
			pNode->time = 1;
			pNode->word1 = word1;
			pNode->word2 = word2;
			pNode->next = NULL;
			q->next = pNode;
		}
	}
}

void CountQuantity(const char* fileName) 
{	
	//Count the total number of characters/lines/words
	FILE *fp = NULL;
	char ch;
	char tempWord[WORD_MAX_LENGTH];//for preserving word temprorily
	wordList wNode, wNodePre = NULL;
	int i = 0, j = 0, flag = 0;

	if ((fp = fopen(fileName, "r")) == NULL) {
		printf("cannot open this file %s\n", fileName);
		exit(0);
	}

	while ( (ch = fgetc(fp)) != EOF)
	{	
		if(ch >= 32 && ch <= 126)
			characterNum++;
		if (ch == '\n') 
		{
			lineNum++;
		}
		if (isLetter(ch) || isNumber(ch))
		{
			i = 0;
			tempWord[i++] = ch;
			while ((ch = fgetc(fp)) != EOF && (isLetter(ch) || isNumber(ch)))
			{
				characterNum++;
				tempWord[i++] = ch;
			}
			tempWord[i] = '\0';

			if (i >= 4 && isLetter(tempWord[0]) && isLetter(tempWord[1]) && isLetter(tempWord[2]) && isLetter(tempWord[3]))
			{
				// it's a word
				wordNum++;
				for (j = i - 1; isNumber(tempWord[j]); j--)
				{
				}
				wNode = CountFrequency(tempWord, j + 1);
				if (flag == 1)
				{
					CountFrequency_Phrase(wNodePre,wNode);
				}
				wNodePre = wNode;
				flag = 1;
			}
			if (ch >= 32 && ch <= 126)
				characterNum++;
			if (ch == '\n')
			{
				lineNum++;
			}
			if (ch == EOF)
			{
				break;
			}
		}
	}

	fclose(fp);
	lineNum++;
	return;
}

void TraverseFolder(string folderPath)
{
	//Traverse a folder using Depth First Search Traversal
	if (folderPath.empty())
	{
		printf("The folder path is empty!\n");
		exit(0);
	}
	
	DIR *dir;
	struct dirent *ptr;

	if ((dir = opendir(folderPath.c_str())) == NULL)
	{
		printf("cannot match the folder path %s\n", folderPath.c_str());
		return;
	}
	while((ptr = readdir(dir)) != NULL)
	{
		//Judge if there are subdirectories
		if (ptr->d_type & DT_DIR)
		{
			//Exclude two cases
			if ((strcmp(ptr->d_name, ".") != 0) && (strcmp(ptr->d_name, "..") != 0))
			{
				//Traverse subdirectories
				string newPath;
				newPath = folderPath + "/" + ptr->d_name;
				TraverseFolder(newPath);

			}
		}
		else
		{
			string fullName;
			fullName = folderPath + "/" + ptr->d_name;
			CountQuantity(fullName.c_str());
		}
	}

	closedir(dir);
}

void Top10WordPhrase()
{
	int i, j;
	wordNode *p;
	phraseNode *q;

	for (i = 0; i < HASHMOD; i++)
	{
		//find top 10 words
		if (headList[i] != NULL)
		{
			p = headList[i];
			while (p != NULL)
			{
				for (j = 9; j >= 0 && p->time > wordTop10[j].time || ((p->time == wordTop10[j].time) && (strcmp(p->word, wordTop10[j].word) < 0)); j--)
				{
					strcpy(wordTop10[j + 1].word, wordTop10[j].word);
					wordTop10[j + 1].time = wordTop10[j].time;
				}
				if (j < 9)
				{
					strcpy(wordTop10[j + 1].word, p->word);
					wordTop10[j + 1].time = p->time;
				}
				p = p->next;
			}
		}
	}
	for (i = 0; i < HASHMOD; i++)
	{
		//find top 10 phrases

		if (headList_Phrase[i] != NULL)
		{
			q = headList_Phrase[i];
			while (q != NULL)
			{
				for (j = 9; j >= 0 && q->time > phraseTop10[j].time; j--)
				{
					phraseTop10[j + 1].word1 = phraseTop10[j].word1;
					phraseTop10[j + 1].word2 = phraseTop10[j].word2;
					phraseTop10[j + 1].time = phraseTop10[j].time;
				}
				if (j < 9)
				{
					phraseTop10[j + 1].word1 = q->word1;
					phraseTop10[j + 1].word2 = q->word2;
					phraseTop10[j + 1].time = q->time;
				}
				q = q->next;
			}
		}
	}
}

int main(int argc, char *argv[]) 
{
	int i;
	FILE *fout;

	if (argc != 2)
	{
		printf("one dir required!\n");
		exit(1);
	}
	TraverseFolder(argv[1]);

	Top10WordPhrase();

	if ((fout = fopen("result.txt", "w")) == NULL) {
		printf("cannot open this file result.txt\n");
		exit(0);
	}

	fprintf(fout, "char_number : %d\n", characterNum);
	fprintf(fout, "line_number : %d\n", lineNum);
	fprintf(fout, "word_number : %d\n", wordNum);
	fprintf(fout, "\n");

	for (i = 0; i <= 9; i++)
	{
		fprintf(fout, "%s: %d\n", wordTop10[i].word, wordTop10[i].time);
	}
	fprintf(fout, "\n");

	for (i = 0; i <= 9; i++)
	{
		fprintf(fout, "%s %s: %d\n", phraseTop10[i].word1->word, phraseTop10[i].word2->word, phraseTop10[i].time);
	}
	
	return 0;
}
