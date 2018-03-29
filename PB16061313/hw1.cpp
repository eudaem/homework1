#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "io.h"
#include "vector"  
#include "cstring"
#include "string"
#include "ctype.h"

#define WORD_LIMIT 4
#define HASH_CAPACITY 456976
#define DIGIT_FIRST 17576
#define DIGIT_SECOND 676
#define DIGIT_THIRD 26
#define MOST_FREQUENT_NUM 10

using namespace std;

typedef struct My_Word_Class
{
	string word;
	string trimmed;		//word without digital suffix and capital character
	intptr_t number;
	struct My_Word_Class *next;
}MyWC;

typedef struct My_Group_Class
{
	MyWC *firstWord, *secondWord;
	intptr_t number;
	struct My_Group_Class *next;
}MyGC;

long charactNum = 0, wordNum = 0, textLine = 0;
MyWC *(ptrOfWord[HASH_CAPACITY]);
MyGC *(ptrOfGroup[HASH_CAPACITY]);

vector<char *>  getFilesList(const char *dir);
bool Statistics_of_File(char *filePath);
MyWC *putIntoWordList(string getWord);
void putIntoGroupList(MyWC *getFirst, MyWC *getSecond);

int main(int argc, char *argv[])
{
	vector<char *> allPath;
	size_t pathNum;
	string input = argv[1];// = "C:\\Users\\CZT\\Desktop\\测试集与参考结果\\newsample";

	if (argc != 2)		//input checking
		cout << "input error!" << endl;
	else
	{
		allPath = getFilesList(input.c_str());
		pathNum = allPath.size();		//number of files
	}

	for (size_t i = 0; i < pathNum; i++)
		Statistics_of_File(allPath[i]);

	ofstream outputFile("result.txt");

	if (outputFile.is_open())
	{
		outputFile << "characters: " << charactNum << endl;
		outputFile << "words: " << wordNum << endl;
		outputFile << "lines: " << textLine << endl << endl;

		outputFile << "the top ten frequency of word :" << endl;
		for (size_t i = 0; i < MOST_FREQUENT_NUM; i++)
		{
			intptr_t max = 0;
			string printWord;
			MyWC *ptrTemp = NULL;
			MyWC *clear = NULL;
			for (intptr_t j = 0; j < HASH_CAPACITY; j++)
			{
				ptrTemp = ptrOfWord[j];
				while (ptrTemp != NULL)
				{
					if (ptrTemp->number > max)
					{
						max = ptrTemp->number;
						printWord = ptrTemp->word;
						clear = ptrTemp;
					}

					ptrTemp = ptrTemp->next;
				}
			}

			clear->number = -1;
			outputFile << printWord << "   " << max << endl;
		}

		outputFile << endl << "the top ten frequency of phrase :" << endl;
		for (size_t i = 0; i < MOST_FREQUENT_NUM; i++)
		{
			intptr_t max = 0;
			MyWC *printPhraseL = NULL, *printPhraseN = NULL;
			MyGC *ptrTemp = NULL;
			MyGC *clear = NULL;
			for (size_t j = 0; j < HASH_CAPACITY; j++)
			{
				ptrTemp = ptrOfGroup[j];
				while (ptrTemp != NULL)
				{
					if (ptrTemp->number > max)
					{
						max = ptrTemp->number;
						printPhraseL = ptrTemp->firstWord;
						printPhraseN = ptrTemp->secondWord;
						clear = ptrTemp;
					}
					ptrTemp = ptrTemp->next;
				}
			}

			clear->number = -1;
			outputFile << printPhraseL->word << " " << printPhraseN->word << "   " << max << endl;
		}
	}

	cout << "success!" << endl;

	return 0;
}

vector<char *> getFilesList(const char *dir)
{
	vector<char *> allPath;

	string dirNew;
	dirNew = dir;
	dirNew += "\\*.*";		//add "\*.*" to our directory for the first search  

	intptr_t handle;		//file's handle
	_finddata_t findData;		//file's structure

	handle = _findfirst(dirNew.c_str(), &findData);		//get file's handle
	if (-1 == handle) {
		dirNew = dir;
		handle = _findfirst(dirNew.c_str(), &findData);
		if (-1 == handle)
		{
			cout << "can not find the file : " << dir << endl;
			return allPath;
		}
		else
		{
			allPath.push_back(const_cast<char *>(dir));
			return allPath;
		}
	}

	do
	{
		if (findData.attrib & _A_SUBDIR)		//check of subdirectory  
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			//add "\" to our directory for next search
			dirNew = dir;
			dirNew += "\\";
			dirNew += findData.name;
			vector<char *> tempPath = getFilesList(dirNew.c_str());		//recursive search for subdirectory
			allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());		//add files in subdirectory to the end of our list 
		}
		else
		{
			string filePath;
			filePath = dir;
			filePath += '\\';
			filePath += findData.name;

			char *cString_filePath = new char[filePath.size()];		//change to cString type
			strcpy_s(cString_filePath, filePath.size() + 1, filePath.c_str());
			allPath.push_back(cString_filePath);		//add a file to the end of our list 
		}
	} while (0 == _findnext(handle, &findData));
	_findclose(handle);    //close handle search  
	return allPath;
}

bool Statistics_of_File(char *filePath)
{
	ifstream operateFile;
	operateFile.open(filePath, ios::in);
	if (!operateFile.is_open())		//file open checking
	{
		cout << "can not open this file : " << filePath << endl;
		return(false);
	}

	string getWordL;
	string getWordN;
	MyWC *getFirst = NULL, *getSecond = NULL;
	intptr_t counter = 0;
	char singleCharact = 0;
	string singleLine;
	intptr_t flag = 1;		//flag of word group

	while (!operateFile.eof())
	{
		getline(operateFile, singleLine);
		textLine++;
		if (operateFile.eof() == NULL)
			singleLine += " ";

		for (size_t i = 0; i < singleLine.size(); i++)
		{
			singleCharact = singleLine[i];

			if (singleCharact >= 32 && singleCharact <= 126)
				charactNum++;
			else
				singleCharact = ' ';

			if (isalpha(singleCharact) || (counter >= WORD_LIMIT && isdigit(singleCharact)))
			{
				counter++;
				if (flag)
					getWordL += singleCharact;
				else
					getWordN += singleCharact;
			}
			else
			{
				if (counter >= WORD_LIMIT)
				{
					if (flag)
					{
						getFirst = putIntoWordList(getWordL);
						flag = 0;
						counter = 0;
						getWordL = "";
					}
					else
					{
						getSecond = putIntoWordList(getWordN);
						putIntoGroupList(getFirst, getSecond);
						getFirst = getSecond;
						counter = 0;
						getWordN = "";
					}
				}
				else
				{
					counter = 0;
					getWordL = "";
					getWordN = "";
				}
			}
		}
	}

	operateFile.close();

	return(true);
}

MyWC *putIntoWordList(string getWord)
{
	size_t offset = 0;
	string trimmedWord;
	MyWC *ptrTemp = NULL;

	offset = (tolower(getWord[0]) - 97) * DIGIT_FIRST;
	offset += (tolower(getWord[1]) - 97) * DIGIT_SECOND;
	offset += (tolower(getWord[2]) - 97) * DIGIT_THIRD;
	offset += tolower(getWord[3]) - 97;

	wordNum++;

	for (size_t i = getWord.size() - 1; i >= 0; i--)
	{
		if (isalpha(getWord[i]))
		{
			trimmedWord = "";
			for (size_t j = 0; j <= i; j++)
			{
				trimmedWord += tolower(getWord[j]);
			}

			break;
		}
	}

	if (ptrOfWord[offset] == NULL)
	{
		ptrOfWord[offset] = new MyWC;
		ptrOfWord[offset]->word = getWord;
		ptrOfWord[offset]->trimmed = trimmedWord;
		ptrOfWord[offset]->number = 1;
		ptrOfWord[offset]->next = NULL;
		ptrTemp = ptrOfWord[offset];
	}
	else
	{
		ptrTemp = ptrOfWord[offset];
		while (1)
		{
			if (ptrTemp->trimmed == trimmedWord)
			{
				if (strcmp(ptrTemp->word.c_str(), getWord.c_str()) == 1)
					ptrTemp->word = getWord;
				ptrTemp->number++;

				break;
			}

			if (ptrTemp->next == NULL)
			{
				ptrTemp->next = new MyWC;
				ptrTemp = ptrTemp->next;
				ptrTemp->word = getWord;
				ptrTemp->trimmed = trimmedWord;
				ptrTemp->number = 1;
				ptrTemp->next = NULL;

				break;
			}
			else
			{
				ptrTemp = ptrTemp->next;
			}
		}
	}

	return ptrTemp;
}

void putIntoGroupList(MyWC *getFirst, MyWC *getSecond)
{
	size_t offset = 0;

	string wordF, wordS;
	wordF = getFirst->trimmed;
	wordS = getSecond->trimmed;

	MyGC *ptrTemp = NULL;

	offset = (tolower(getFirst->word[0]) - 97) * DIGIT_FIRST;
	offset += (tolower(getFirst->word[1]) - 97) * DIGIT_SECOND;
	offset += (tolower(getSecond->word[0]) - 97) * DIGIT_THIRD;
	offset += tolower(getSecond->word[1]) - 97;

	if (!ptrOfGroup[offset])
	{
		ptrOfGroup[offset] = new MyGC;
		ptrOfGroup[offset]->firstWord = getFirst;
		ptrOfGroup[offset]->secondWord = getSecond;
		ptrOfGroup[offset]->number = 1;
		ptrOfGroup[offset]->next = NULL;

		ptrTemp = ptrOfGroup[offset];
	}
	else
	{
		ptrTemp = ptrOfGroup[offset];

		while (1)
		{
			if ((ptrTemp->firstWord)->trimmed == wordF && (ptrTemp->secondWord)->trimmed == wordS)
			{
				ptrTemp->number++;

				break;
			}

			if (!ptrTemp->next)
			{
				ptrTemp->next = new MyGC;
				ptrTemp = ptrTemp->next;
				ptrTemp->firstWord = getFirst;
				ptrTemp->secondWord = getSecond;
				ptrTemp->number = 1;
				ptrTemp->next = NULL;

				break;
			}
			else
			{
				ptrTemp = ptrTemp->next;
			}
		}
	}
}

