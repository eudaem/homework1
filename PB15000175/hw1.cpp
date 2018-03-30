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
int GetTopTenWords(unordered_map<string, int> wordValueMap, unordered_map<string, string> wordNameMap, string topTenWordName[],int topTenWordNum[]);
int GetTopTenPhrases(unordered_map<string, string> wordNameMap, string topTenPhraseName[], int topTenPhraseNum[],int wordCount);

string allWords[20000000];

int main(int arc,char* args[])
{
	char dir[200];
	string fileName[2000];
	int i, fileNum = 0,chrtCount = 0,wordCount=0, lineCount = 0;
	FILE *fp;

	unordered_map<string,int> wordValueMap;
	unordered_map<string,string> wordNameMap;

	strcpy_s(dir, args[1]);
	SearchFiles(dir,fileName,fileNum);
	cout << "File Number: " << fileNum << endl;

	for (i = 0; i < fileNum; i++)
	{
		if (fopen_s(&fp,fileName[i].c_str(), "r")!=0)
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

	strcpy_s(searchdir, dir);
	strcat_s(searchdir, "\\*");
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
				strcpy_s(newdir, dir);
				strcat_s(newdir, "\\");
				strcat_s(newdir, findData.name);
				SearchFiles(newdir,fileName,fileNum);
			}	
			else
			{
				strcpy_s(newfile, dir);
				strcat_s(newfile, "\\");
				strcat_s(newfile, findData.name);
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
					for ( j = word.length() - 1; word[j] >= 48 && word[j] <= 57; j--);
					string newWord(word, 0, j + 1);
					for (j = 0; j < newWord.length(); j++)
						if (newWord[j] >= 65 && newWord[j] <= 90)
							newWord[j] = newWord[j] + 32;

					allWords[wordCount] = newWord;
					wordCount++;

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
	string topTenWordName[10];
	int topTenWordNum[10];
	string topTenPhraseName[10];
	int topTenPhraseNum[10];
	int i,wordNum,phraseNum;

	os << "Char_Number: " << chrtCount << endl;
	os << "Line_Number: " << lineCount << endl;
	os << "Words_Number " << wordCount << endl;
	os << endl;
	os << "The top ten frequency of words:" << endl;

	wordNum = GetTopTenWords(wordValueMap,wordNameMap,topTenWordName,topTenWordNum);
	for (i = 0; i < wordNum; i++)
		os << "<" << topTenWordName[i] << ">: " << topTenWordNum[i] << endl;

	os << endl;
	os << "The top ten frequenzy of phrases:" << endl;

	phraseNum = GetTopTenPhrases(wordNameMap, topTenPhraseName, topTenPhraseNum, wordCount);
	for (i = 0; i < phraseNum; i++)
		os << "<" << topTenPhraseName[i] << ">: " << topTenPhraseNum[i] << endl;
}

int GetTopTenWords(unordered_map<string, int> wordValueMap, unordered_map<string, string> wordNameMap, string topTenWordName[], int topTenWordNum[])
{
	unordered_map<string, int>::iterator itValue = wordValueMap.begin();
	unordered_map<string, string>::iterator itName;
	int i, j,wordNum;
	for (i = 0; i < 10; i++)
	{
		topTenWordNum[i] = 0;
		topTenWordName[i] = "\0";
	}

	while (itValue != wordValueMap.end())
	{
		i = 9;

		while (itValue->second > topTenWordNum[i] && i >= 0)
			i--;

		if (i < 9)
		{
			for (j = 9; j > i + 1; j--)
			{
				topTenWordNum[j] = topTenWordNum[j - 1];
				topTenWordName[j] = topTenWordName[j - 1];
			}

			topTenWordNum[i + 1] = itValue->second;
			topTenWordName[i + 1] = itValue->first;
		}

		itValue++;
	}

	wordNum = wordValueMap.size();
	if (wordNum >= 10)
		wordNum = 10;
		
	for (i = 0; i < wordNum; i++)
	{
		itName = wordNameMap.find(topTenWordName[i]);
		topTenWordName[i] = itName->second;
	}

	return wordNum;
}

int GetTopTenPhrases(unordered_map<string, string> wordNameMap, string topTenPhraseName[], int topTenPhraseNum[],int wordCount)
{
	unordered_map<string, int> phraseMap;
	unordered_map<string, int>::iterator itPhrase;
	unordered_map<string, string>::iterator itName;
	string phrase, word1, word2;
	int i, j,phraseNum;

	for (i = 0; i < 10; i++)
	{
		topTenPhraseNum[i] = 0;
		topTenPhraseName[i] = "\0";
	}

	for (i = 0; i < wordCount - 1; i++)
	{
		phrase = allWords[i] + "$" + allWords[i + 1];
		itPhrase = phraseMap.find(phrase);
		if (itPhrase == phraseMap.end())
			phraseMap.insert(pair<string, int>(phrase, 1));
		else
			itPhrase->second++;
	}

	itPhrase = phraseMap.begin();
	while (itPhrase != phraseMap.end())
	{
		i = 9;

		while (itPhrase->second >topTenPhraseNum[i] && i >= 0)
			i--;

		if (i < 9)
		{
			for (j = 9; j > i + 1; j--)
			{
				topTenPhraseNum[j] = topTenPhraseNum[j - 1];
				topTenPhraseName[j] = topTenPhraseName[j - 1];
			}

			topTenPhraseNum[i + 1] = itPhrase->second;
			topTenPhraseName[i + 1] = itPhrase->first;
		}

		itPhrase++;
	}

	phraseNum = phraseMap.size();
	if (phraseNum >= 10)
		phraseNum = 10;

	for (i = 0; i < phraseNum; i++)
	{
		for (j = 0; j < topTenPhraseName[i].length(); j++)
			if (topTenPhraseName[i][j] == '$')
				break;
		word1.assign(topTenPhraseName[i], 0, j);
		word2.assign(topTenPhraseName[i], j + 1, topTenPhraseName[i].length());
		itName = wordNameMap.find(word1);
		topTenPhraseName[i].assign(itName->second);
		topTenPhraseName[i].append(" ");
		itName = wordNameMap.find(word2);
		topTenPhraseName[i].append(itName->second);
	}

	return phraseNum;
}
