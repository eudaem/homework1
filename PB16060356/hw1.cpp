#include "stdafx.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <io.h>
using namespace std;

struct PointerInformation
{
	short InsertOrReplace;  // 0->insert  |  1->replace  |  2->no action
};

struct Word
{
	long fre = 0;
	char word[100] = {0};
	struct Word *next;
};

struct Phrase
{
	int fre = 0;
	char word1[100] = {0};
	char word2[100] = {0};
	struct Phrase *next;
};

struct PointerInformation PI;
struct Phrase *tempPhrase;
struct Word MaxWordFre[11];
struct Phrase MaxPhraseFre[11];
struct Word *headWord = NULL;
struct Phrase *headPhrase = NULL;
char tempWord[100];
long characterNum = 0;
long wordNum = 0;
long lineNum = 0;


void ClearTemp();
bool IsLetter(char ch);
bool IsSeparator(char ch);
char LetterConvert(char ch);
void PrintIntoText();
void StoreWord();
void StorePhrase();
void WordCheck(FILE *fp);
void OpenFile(char *filePath);
void getAllFiles(string path, vector<string>& files);
char WordCompare(char *ch1, char *ch2);
short PhraseCompare(struct Phrase *phrase1, struct Phrase *phrase2);
struct Word *WordSearch(struct Word *pointer);
struct Phrase *PhraseSearch(struct Phrase *pointer);



void PrintIntoText()
{
	FILE *fpOut = NULL;
	errno_t err;
	err = fopen_s(&fpOut, "result.txt", "w");

	if(err == 0)  // if it is not a false location
	{
		fprintf(fpOut, "<characterNum>\t%d\n", characterNum);
		fprintf(fpOut, "<lineNum>\t%d\n", lineNum);
		fprintf(fpOut, "<wordNum>\t%d\n", wordNum);

		fprintf(fpOut, "\nMax Frequency Words\n");
		for(int i = 0; i < 10; i++)
			if(MaxWordFre[i].fre)
				fprintf(fpOut, "%s\t\t%d\n", MaxWordFre[i].word, MaxWordFre[i].fre);
	}

	fclose(fpOut);
}

void StoreMaxFreWords(struct Word *p)
{
	short position;
	short move;

	while(p != NULL)
	{
		if(MaxWordFre[0].fre == 0)
		{
			strcpy(MaxWordFre[0].word, p->word);
			MaxWordFre[0].fre = p->fre;
		}

		else
		{
			position = 0;

			while(p->fre < MaxWordFre[position].fre)  // find the right position to store
				position++;

			if(position < 10)
			{
				if(p->fre == MaxWordFre[position].fre && WordCompare(p->word, MaxWordFre[position].word) == -1)   // ready to store after the position
				{
					for(move = 9; move > (position + 1); move--)
					{
						strcpy(MaxWordFre[move].word, MaxWordFre[move - 1].word);
						MaxWordFre[move].fre = MaxWordFre[move - 1].fre;
					}
					strcpy(MaxWordFre[position + 1].word, p->word);
					MaxWordFre[position + 1].fre = p->fre;
				}

				else  // ready to store right in the position
				{
					for(move = 9; move > position; move--)  // clear up the space to store
					{
						strcpy(MaxWordFre[move].word, MaxWordFre[move - 1].word);
						MaxWordFre[move].fre = MaxWordFre[move - 1].fre;
					}
					strcpy(MaxWordFre[position].word, p->word);  // store
					MaxWordFre[position].fre = p->fre;
				}
			}
		}

		p = p->next;
	}
}

void ClearTemp()
{
	for(int i = 0; tempWord[i] != '\0'; i++)
		tempWord[i] = '\0';
}

bool IsLetter(char ch)
{
	if((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
		return true;
	else
		return false;
}

bool IsSeparator(char ch)
{
	if((ch >= 0 && ch <= 47) || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 127))
		return true;
	else
		return false;
}

char LetterConvert(char ch)
{
	if(ch >= 97 && ch <= 122)
		return (ch -= 32);
	else
		return ch;
}

void WordConvert(char *ch1, char *ch2)
{
	for(int i = 0; ch1[i] != '\0'; i++)
		ch1[i] = LetterConvert(ch1[i]);

	for(int i = 0; tempWord[i] != '\0'; i++)
		ch2[i] = LetterConvert(ch2[i]);
}

void StoreWord()
{
	struct Word *insertPointer;  // insertPointer points to the position to insert
	struct Word *tempPointer;  // tempPointer puts the new word into the list

	tempPointer = (struct Word*)malloc(sizeof(struct Word));  // new space
	tempPointer->fre = 1;
	strcpy(tempPointer->word, tempWord);

	insertPointer = WordSearch(headWord);  // to see whether we should insert, replace or take no action

	if(PI.InsertOrReplace == 0)  // insert at the head
	{
		tempPointer->next = headWord;
		headWord = tempPointer;
	}

	else if(PI.InsertOrReplace == 1)  // replace and increase frequency
	{
		strcpy((insertPointer)->word, tempPointer->word);  // copy
		insertPointer->fre++;
	}

	else if(PI.InsertOrReplace == 2)  // increase frequency only
		insertPointer->fre++;
}

void StorePhrase()
{

}

void WordCheck(FILE *fp)
{
	bool stop = false;
	bool isEmpty = false;
	char ch;  // ch gets the character one by one
	short check = 0;  // to check if the first four characters are letters
	short i = 0;

	ch = fgetc(fp);
	if(ch == EOF)
		isEmpty = true;

	for(; !stop; ch = fgetc(fp))  // if it is not the end of the text
	{
		if(ch >= 32 && ch <= 126)
			characterNum++;
		if(ch == '\n')
			lineNum++;

		if(check < 4)  // to check the first four characters
		{
			if(ch == EOF)
			{
				stop = true;
				if(isEmpty == false)
					lineNum++;
			}

			else if(IsLetter(ch) == true)
			{
				++check;
				tempWord[i] = ch;
				++i;  // search for the next
			}
			else
			{
				i = 0;
				check = 0;
				ClearTemp();
			}
		}
		else  // first four characters are all letters, ready to store
		{
			if(IsSeparator(ch) || ch == EOF)  // have met a separator, store the word
			{
				i = 0;  // roll back to the beginning in the next search
				check = 0;  // roll back to the beginning in the next search

				wordNum++;  // have found another word
				StoreWord();  // store the word
				ClearTemp();  // prepare for the next search

				if(ch == EOF)
				{
					stop = true;
					if(isEmpty == false)
						lineNum++;
				}
			}

			else  // have not met a separator, keep searching
			{
				tempWord[i] = ch;
				++i;  // search for the next
			}
		}
	}
}

void OpenFile(char *filePath)
{
	FILE* fp = NULL;
	errno_t err;
	err = fopen_s(&fp, filePath, "r");

	if(err == 0)  // if it is not a false location
		WordCheck(fp);

	fclose(fp);
}

void getAllFiles(string path, vector<string>& files)
{
	char * location;  // location converts char * into string
	long  handle = 0;  // File Handle
	struct _finddata_t fileinfo;  // File Information
	string p;

	if((handle = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if((fileinfo.attrib & _A_SUBDIR))  // to check whether it is a folder or a file
			{
				if(strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				location = (char *)p.data();
				OpenFile(location);
			}
		}
		while(_findnext(handle, &fileinfo) == 0);
		_findclose(handle);  // close the handle
	}
}

char WordCompare(char *ch1, char *ch2)
{
	int i = 0;

	char chTemp1[100] = {0};
	char chTemp2[100] = {0};

	for(int i = 0; ch1[i] != '\0'; i++)
		chTemp1[i] = ch1[i];
	for(int i = 0; ch2[i] != '\0'; i++)
		chTemp2[i] = ch2[i];

	WordConvert(chTemp1, chTemp2);  // convert all small letters to big letters

	while(chTemp1[i] != '\0' || chTemp2[i] != '\0')
	{
		if(IsLetter(chTemp1[i])&IsLetter(chTemp2[i]))
		{
			if(chTemp1[i] < chTemp2[i])
				return -1;
			else if(chTemp1[i] > chTemp2[i])
				return 1;
			else
				i++;
		}
		else if(IsLetter(chTemp1[i]) & !IsLetter(chTemp2[i]))
			return 1;
		else	if(!IsLetter(chTemp1[i]) & IsLetter(chTemp2[i]))
			return -1;
		else
			i++;
	}
	return 0;
}

struct Word *WordSearch(struct Word *pointer)
{
	if(pointer == NULL)  // if the list is empty
	{
		PI.InsertOrReplace = 0;  // insert
	}

	else  // if the list is not empty
	{
		while((pointer != NULL) && WordCompare(pointer->word, tempWord) != 0)
			pointer = pointer->next;

		if(pointer == NULL)
		{
			PI.InsertOrReplace = 0;  // insert at the head
		}

		else  // replace or take no action
		{
			if(strcmp(pointer->word, tempWord) > 0)
				PI.InsertOrReplace = 1;  // replace

			else if(strcmp(pointer->word, tempWord) <= 0)
				PI.InsertOrReplace = 2;  // no action
		}
	}
	return pointer;  // return the right position
}

struct Phrase *PhraseSearch(struct Phrase *pointer)
{
	return pointer;
}

short PhraseCompare(struct Phrase *phrase1, struct Phrase *phrase2)
{
	if(WordCompare(phrase1->word1, phrase2->word1) == -1)
		return -1;

	else if(WordCompare(phrase1->word1, phrase2->word1) == 1)
		return 1;

	else
	{
		if(WordCompare(phrase1->word2, phrase2->word2) == -1)
			return -1;
		if(WordCompare(phrase1->word2, phrase2->word2) == 1)
			return 1;
		else
			return 0;
	}

}

int main(int argc, char *argv[])
{
	const char * filePath = argv[1];
	vector<string> files;

	getAllFiles(filePath, files);  // search for all the files
	StoreMaxFreWords(headWord);
	PrintIntoText();
}
