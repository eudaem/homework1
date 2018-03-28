// FileWordCounts.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <io.h>
#include <unordered_map> 
using namespace std;

void SearchFiles(char* dir,string filename[],int& fileNum);
void ReadFile(FILE *fp, unordered_map<string,int>& wordValueMap,unordered_map<string, string>& wordNameMap,int& chrtCount,int& wordCount,int& lineCount);
void WriteFile(ofstream& os, unordered_map<string, int> wordValueMap, unordered_map<string, string> wordNameMap, int chrtCount, int wordCount, int lineCount);

int main(int arc,char* args[])
{
	char dir[200];
	string fileName[2000];
	int i, fileNum = 0,chrtCount = 0,wordCount=0, lineCount = 0;
	FILE *fp;

	unordered_map<string,int> wordValueMap;
	unordered_map<string,string> wordNameMap;

	strcpy(dir, args[1]);
	SearchFiles(dir,fileName,fileNum);
	cout << "File Number: " << fileNum << endl;

	for (i = 0; i < fileNum; i++)
	{
		if (!(fp = fopen(fileName[i].c_str(), "r")))
		{
			cerr << "Cannot open file " << fileName[i] << endl;
			exit(1);
		}
		ReadFile(fp, wordValueMap,wordNameMap, chrtCount,wordCount,lineCount);
		fclose(fp);
	}
	
	cout << "Finish Reading. WordCount: " << wordCount << endl;
	
	ofstream os("result.txt", ios::out);

	if (!os)
	{
		cerr << "Cannot write file result.txt!" << endl;
		exit(1);
	}

	WriteFile(os, wordValueMap, wordNameMap, chrtCount,wordCount, lineCount);
	os.close();
	cout << "Finish Writing" << endl;
	
	return 0;
}

void SearchFiles(char* dir,string fileName[],int& fileNum)
{
	intptr_t handle;
	_finddata_t findData;
	char searchdir[200],newdir[200],newfile[200];

	strcpy(searchdir, dir);
	strcat(searchdir, "\\*");
	handle = _findfirst(searchdir, &findData); 

	if (handle == -1)
	{
		cout << "Failed to find first file!\n";
		return;
	}

	do
	{
		if (strcmp(findData.name, ".") != 0 && strcmp(findData.name, "..") != 0)
		{
			if (findData.attrib == _A_SUBDIR)
			{
				strcpy(newdir, dir);
				strcat(newdir, "\\");
				strcat(newdir, findData.name);
				SearchFiles(newdir,fileName,fileNum);
			}	
			else
			{
				strcpy(newfile, dir);
				strcat(newfile, "\\");
				strcat(newfile, findData.name);
				fileName[fileNum]=newfile;
				fileNum+=1;				
			}
		}
	} while (_findnext(handle, &findData) == 0);    

	_findclose(handle);   
}


void ReadFile(FILE *fp, unordered_map<string, int>& wordValueMap, unordered_map<string, string>& wordNameMap, int& chrtCount, int& wordCount, int& lineCount)
{
	char ch;
	string word;
	unsigned int i,j;
	unordered_map<string, int>::iterator itValue;
	unordered_map<string, string>::iterator itName;

	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch >= 32 && ch <= 126)
			chrtCount++;
			
		if ((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
			word=word+ch;

		else
		{
			if (word.length() >= 4)
			{
				for (i = 0; i < 4; i++)
					if (!(((word[i] >= 65) && (word[i] <= 90)) || ((word[i] >= 97) && (word[i] <= 122))))
						break;
				if (i >= 4)
				{
					wordCount++;

					for ( j = word.length() - 1; word[j] >= 48 && word[j] <= 57; j--);
					string newWord(word, 0, j + 1);
					for (j = 0; j < newWord.length(); j++)
						if (newWord[j] >= 65 && newWord[j] <= 90)
							newWord[j] = newWord[j] + 32;

					itValue = wordValueMap.find(newWord);

					if (itValue == wordValueMap.end())
						wordValueMap.insert(pair<string, int>(newWord, 1));
					
					else
						itValue->second++;

					itName = wordNameMap.find(newWord);
					if (itName == wordNameMap.end())
						wordNameMap.insert(pair<string, string>(newWord, word));

					else if (word.compare(itName->second) < 0)
						itName->second = word;
				}
			}
				
			word.erase();
			if(ch == '\n')
				lineCount++;
		}
	}

	lineCount++;	
}


void WriteFile(ofstream& os, unordered_map<string, int> wordValueMap, unordered_map<string, string> wordNameMap, int chrtCount, int wordCount, int lineCount)
{
	unordered_map<string, int>::iterator itValue = wordValueMap.begin();
	unordered_map<string, string>::iterator itName;
	string topTenName[10];
	int topTenValue[10];
	int i,j;

	os << "Char_Number: " << chrtCount << endl;
	os << "Line_Number: " << lineCount << endl;
	os << "Words_Number " << wordCount << endl;
	os << endl;
	os << "The top ten frequency of words:" << endl;

	for (i = 0; i < 10; i++)
	{
		topTenValue[i] = 0;
		topTenName[i] = "\0";
	}

	while (itValue != wordValueMap.end())
	{
		i = 9;

		while (itValue->second > topTenValue[i] && i >= 0)
			i--;

		if (i < 9)
		{
			for (j = 9; j > i + 1; j--)
			{
				topTenValue[j] = topTenValue[j - 1];
				topTenName[j] = topTenName[j - 1];
			}

			topTenValue[i + 1] = itValue->second;
			topTenName[i + 1] = itValue->first;
		}
		
		itValue++;
	}

	for (i = 0; i < 10; i++)
	{
		itName = wordNameMap.find(topTenName[i]);
		topTenName[i] = itName->second;
	}

	for (i = 0; i < 10; i++)
		os << "<" << topTenName[i] << ">: " << topTenValue[i] << endl;
}
