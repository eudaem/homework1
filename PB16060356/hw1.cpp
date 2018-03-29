#include<io.h>
#include<stdio.h>
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>
#include <ctype.h>

/*************************
	  Data Structure
*************************/

typedef struct _nextWord NextWord;
typedef struct wordNode WordNode;
typedef struct topPhraseStruc TopPhraseStruc;
typedef struct topWordStruc TopWordStruc;

#define nullptr NULL 
#define isALetter(temp)	((temp >= 'A'&&temp<='Z') || (temp>='a'&&temp <= 'z'))
#define isLetterOrNum(temp) ((temp >='A'&&temp<='Z') || (temp>='a'&&temp <= 'z')||(temp>='0'&&temp<='9'))
#define MAXLENGTH 1024
#define NW_INITSIZE 15
#define INCREAMENT 15

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


/*************************
	  Global Variable
*************************/

int Characters = 0;
int enterNum = 0;
int NumOfWords = 0;
WordNode *HashSet[456976];
char coreWordTemp[MAXLENGTH];
char suffixWordTemp[MAXLENGTH];
WordNode *FirstWord;
WordNode *LastWord;
TopWordStruc topTenWordList[10];
TopPhraseStruc topTenPhraseList[10];
char CompareTemp1[1024];
char CompareTemp2[1024];


/*************************
  Function Declaration
*************************/

int isInputLegal(int argc);
int isFolder(char** argv);
void CalAllFile(char filename[]);
char isNotAWord(char *offset);
void SolveTheWord(char *word);
void PushWordIntoList(WordNode *p, char* word);
void DevideWord(char *word);
char isWordEqual(char *string1, char *string2);
void PushPhraseIntoList();
void FrequencyTopTen();
void WordTen(WordNode *wordPointer);
void PhraseTen(WordNode *wordPointer);
char compareWord(WordNode *node1, WordNode *node2);
char *MakeString(char *string);
void anothermain(char *fileString);
bool is_dir(const char* path);
char *testFileRead(char *filename);
char isSameWord(char *string1, char *string2);
short compareSameWordAsc(char *string1, char *string2);
void printIntoText();
void ReadFolder(char *folderName);
WordNode *WordNodeInit();
WordNode *WordNodeInit(char *coreWord, char *suffixWord);


int main(int argc, char** argv)
{
	for(int count = 0; count < 10; count++)
	{
		topTenPhraseList[count].phraseCount = 0;
		topTenWordList[count].wordCount = 0;
	}
	long handle;
	struct _finddata_t fileinfo;

	char *file = (char *)"D:\\newsample";
	if(argc != 2)
		exit(0);

	if(is_dir(argv[1]))  // if it is a folder
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

	if((handle = _findfirst(filename, &fileinfo)) != -1)
	{
		if(*fileinfo.name != '.')
		{
			filename[0] = '\0';
			strcat_s(filename, 256, folderName);
			strcat_s(filename, 256, "\\");
			strcat_s(filename, 256, fileinfo.name);

			if(is_dir(filename))
				ReadFolder(filename);
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
	text = (char *)malloc(lSize + 1);
	rewind(pf);
	fread(text, 1, lSize, pf);
	text[lSize] = '\0';
	fclose(pf);
	return text;
}
NextWord *NextWordArrayInit()
{
	NextWord *wordArray = (NextWord *)malloc(NW_INITSIZE * sizeof(NextWord));
	wordArray->nextWord = nullptr;
	wordArray->nextWordCount = 0;
	return wordArray;
}

char word[MAXLENGTH];

void anothermain(char *fileString)
{
	char *offset = fileString;
	char move;
	char temp;

	int count;
	do
	{
		if(temp = *offset)
		{
			if(temp >= 32 && temp <= 126)
			{
				Characters++;
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[0] = temp;
					}
				}
				else if(temp >= 49 && temp <= 57)
				{
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{
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
				{
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
				Characters++;
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[1] = temp;
					}
				}
				else if(temp >= 49 && temp <= 57)
				{
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{
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
				{

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
				Characters++;
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[2] = temp;
					}
				}
				else if(temp >= 49 && temp <= 57)
				{
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{
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
				{
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
				Characters++;
				if(temp >= 65 && temp <= 122)
				{
					if(temp >= 91 && temp <= 96)
					{
						offset++;
						continue;
					}
					else
					{
						offset++;
						word[3] = temp;
						for(count = 4; isLetterOrNum(*offset); offset++, count++)
						{
							Characters++;
							word[count] = *offset;
						}
						word[count] = '\0';
						NumOfWords++;
						SolveTheWord(word);
					}
				}
				else if(temp >= 49 && temp <= 57)
				{
					offset++;
					while(*offset >= 49 && *offset <= 57)
					{
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
				{
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

char isNotAWord(char *offset)
{
	char temp;
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
	{
		HashSet[location] = WordNodeInit();
	}
	PushWordIntoList(HashSet[location], word);
	PushPhraseIntoList();
}

WordNode *WordNodeInit()
{
	WordNode *p = (WordNode *)malloc(sizeof(WordNode));
	p->next = nullptr;
	p->nextWord = nullptr;
	return p;
}

void PushWordIntoList(WordNode *p, char* word)
{
	DevideWord(word);
	int judge;
	for(; p->next; p = p->next)
	{
		if(!isSameWord(p->next->coreString, coreWordTemp))
		{
			p->next->wordCount++;
			if((judge = compareSameWordAsc(p->next->coreString, coreWordTemp) > 0))
			{
				free(p->next->coreString);
				p->next->coreString = MakeString(coreWordTemp);
			}
			if((isWordEqual(p->next->suffixString, suffixWordTemp)) > 0)
			{
				free(p->next->suffixString);
				p->next->suffixString = MakeString(suffixWordTemp);
			}
			if(LastWord)
			{
				FirstWord = LastWord;
				LastWord = p->next;
			}
			else
			{
				if(FirstWord)
					LastWord = p->next;
				else
					FirstWord = p->next;
			}
			return;
		}
	}
	p->next = WordNodeInit(coreWordTemp, suffixWordTemp);
	p->next->next = NULL;
	if(LastWord)
	{
		FirstWord = LastWord;
		LastWord = p->next;
	}
	else
	{
		if(FirstWord)
		{
			LastWord = p->next;
		}
		else
		{
			FirstWord = p->next;
		}
	}
	return;
}

void DevideWord(char *word)
{
	char *temp1, *temp2;
	int count;
	for(temp1 = word; *temp1 != '\0'; temp1++);
	temp1--;
	for(temp2 = temp1; *temp2 < 58; temp2--);
	for(count = 0; word <= temp2; word++, count++)
		coreWordTemp[count] = *word;
	coreWordTemp[count] = '\0';
	for(count = 0; word <= temp1; word++, count++)
		suffixWordTemp[count] = *word;
	suffixWordTemp[count] = '\0';
	return;
}

char isWordEqual(char *string1, char *string2)
{
	for(; *string1 && *string2; string1++, string2++)
	{
		if(*string1 != *string2)
			return (*string1 - *string2);
	}

	if(*string1)
		return 1;
	else if(*string2)
		return -1;
	return 0;

}
char isSameWord(char *string1, char *string2)
{
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
{
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
	if(!LastWord)
		return;

	WordNode *temp;
	NextWord *temp1;
	int newSize;
	temp = FirstWord;
	int count;

	if(temp->nextWord == NULL)
	{
		temp->nextWord = NextWordArrayInit();
		temp->maxPhraseNum = NW_INITSIZE;
	}
	if((temp->phraseNum) == temp->maxPhraseNum)
	{
		newSize = temp->maxPhraseNum + INCREAMENT;
		temp->maxPhraseNum = newSize;
		temp->nextWord = (NextWord *)realloc(temp->nextWord, (newSize) * sizeof(NextWord));
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
	{
		WordNode *p = HashSet[count1];
		if(!p || !(p->next))
			continue;
		else
		{
			p = p->next;
			while(p)
			{
				WordTen(p);
				PhraseTen(p);
				p = p->next;
			}
		}
	}
}

void WordTen(WordNode *wordPointer)
{
	int shortest = topTenWordList[9].wordCount;
	int wordCount = wordPointer->wordCount;
	if(wordCount < shortest)
		return;
	if(wordCount == shortest)
	{
		if(compareWord(wordPointer, topTenWordList[9].word) > 0)
			return;
	}

	for(int count = 8; count >= 0; count--)
	{
		if(wordPointer->wordCount > topTenWordList[count].wordCount)
		{
			topTenWordList[count + 1].word = topTenWordList[count].word;
			topTenWordList[count + 1].wordCount = topTenWordList[count].wordCount;
		}
		else if(wordPointer->wordCount < topTenWordList[count].wordCount)
		{
			topTenWordList[count + 1].word = wordPointer;
			topTenWordList[count + 1].wordCount = wordPointer->wordCount;
			return;
		}
		else
		{
			if((compareWord(wordPointer, topTenWordList[count].word)) > 0)
			{
				topTenWordList[count + 1].word = wordPointer;
				topTenWordList[count + 1].wordCount = wordPointer->wordCount;
				return;
			}
			else
			{
				topTenWordList[count + 1].word = topTenWordList[count].word;
				topTenWordList[count + 1].wordCount = topTenWordList[count].wordCount;
			}
		}
	}
	topTenWordList[0].word = wordPointer;
	topTenWordList[0].wordCount = wordPointer->wordCount;
	return;
}

char compareWord(WordNode *node1, WordNode *node2)
{
	char *temp1 = node1->coreString;
	char *temp2 = node2->coreString;
	for(; *temp1 && *temp2; temp1++, temp2++)
	{
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

void PhraseTen(WordNode *wordPointer)
{
	int shortest = topTenPhraseList[9].phraseCount;
	int judge1, judge2;
	NextWord *nextWordTemp = wordPointer->nextWord;
	for(int count = 0; count < wordPointer->phraseNum; count++)
	{
		if(nextWordTemp[count].nextWordCount < shortest)
			return;

		else
		{
			if(nextWordTemp[count].nextWordCount == shortest)
			{

				if((judge2 = compareWord(wordPointer, topTenPhraseList[9].firstWord)) > 0)
					return;
				else if(judge2 == 0)
				{
					if(compareWord(nextWordTemp[count].nextWord, topTenPhraseList[9].secondWord) > 0)
						return;
				}
			}
			for(int count1 = 8; count1 >= 0; count1--)
			{
				if((judge1 = nextWordTemp[count].nextWordCount - topTenPhraseList[count1].phraseCount) < 0)
				{
					topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
					topTenPhraseList[count1 + 1].firstWord = wordPointer;
					topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
					return;
				}
				else if(judge1 == 0)
				{
					if((judge2 = compareWord(wordPointer, topTenPhraseList[count1].firstWord)) > 0)
					{
						topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
						topTenPhraseList[count1 + 1].firstWord = wordPointer;
						topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
						return;
					}
					else if(judge2 == 0)
					{
						if(compareWord(nextWordTemp[count].nextWord, topTenPhraseList[9].secondWord) > 0)
						{
							topTenPhraseList[count1 + 1].phraseCount = nextWordTemp->nextWordCount;
							topTenPhraseList[count1 + 1].firstWord = wordPointer;
							topTenPhraseList[count1 + 1].secondWord = nextWordTemp[count].nextWord;
							return;
						}
						else
						{
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
				{
					topTenPhraseList[count1 + 1].phraseCount = topTenPhraseList[count1].phraseCount;
					topTenPhraseList[count1 + 1].firstWord = topTenPhraseList[count1].firstWord;
					topTenPhraseList[count1 + 1].secondWord = topTenPhraseList[count1].secondWord;
				}
			}

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
	p = (char *)malloc(length + 1);
	strcpy_s(p, length + 1, string);
	return p;
}

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
			if(topTenWordList[count].wordCount == 0)
				break;
			fprintf(fpOut, "%s\t%s\t%d\n", topTenWordList[count].word->coreString, topTenWordList[count].word->suffixString, topTenWordList[count].wordCount);
		}

		fprintf(fpOut, "\nMax Frequency Phrases\n");

		for(int count = 0; count < 10; count++)
		{
			if(topTenPhraseList[count].phraseCount == 0)
				break;
			fprintf(fpOut, "%s%s\t%s%s\t%d\n",topTenPhraseList[count].firstWord->coreString, topTenPhraseList[count].firstWord->suffixString, topTenPhraseList[count].secondWord->coreString, topTenPhraseList[count].secondWord->suffixString, topTenPhraseList[count].phraseCount);
		}
	}

	fclose(fpOut);
}
