#include<iostream>
#include<io.h>
#include<fstream>
#include<string>
#include<Windows.h>
#include<stdio.h>
#include<map>
#include < unordered_map >
using namespace std::tr1;
using namespace std;

int g_LineNumber = 0;			//The number of rows used for recording 
int g_CharacterNumber = 0;		//The number of characters used to record the number of characters
int g_Wordnumber = 0;			//The number of words used to record the number of words
int g_circle = 4;				//A loop counter, used to judge whether itit is the beginning of a word 

typedef struct MyWord			//The storage of the original word in the structure and the frequency of its appearance, as well as its replacement letters 
{
	string originword;
	int    frequency;
	string Maxword;
}MyWord;

typedef struct Pharze			//Storage of phrases and frequency of appearance in a structure
{
	string firstword;
	string secondword;
	int frequency;
	string originword;
}Pharze;

Pharze g_pharsesample;							//Used for statistical phrases 

void CounterCharacter(char buffer)				//Statistical character number 
{

	if ((buffer >= 32) && (buffer <= 126))		//Determine whether the character is in the Ascii code 
	{
		g_CharacterNumber++;
	}
	if (buffer == '\n')
	{
		g_LineNumber++;
	}
}
bool JudgeLetter(char letter)	//Judge whether a character is an English letter
{
	if (((letter >= 'A') && (letter <= 'Z')) || ((letter >= 'a') && (letter <= 'z')))
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool JudgeNumber(char number)
{
	if ((number >= '0') && (number <= '9'))
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool JudgeCase(string dest, string source)		//To distinguish Case
{
	if ((size(dest) != size(source))||(dest==source))
	{
		return false;
	}
	int i = size(dest);
	for (int j = 0; j < i; j++)
	{
		if ((((source.at(j) - dest.at(j)) % 32) == 0)&&((source.at(j)-dest.at(j))>=0))
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}
void CounterPhrase(string partword, unordered_map<string, Pharze> &mapPharze)			//Statistical phrase number 
{
	unordered_map<string, Pharze>::iterator pharzeiter;
	if (g_pharsesample.firstword.empty())		//After judging the word, if yes,it has become the first word
	{
		g_pharsesample.firstword = partword; 
	}
	else
	{
		g_pharsesample.secondword = partword;
		pharzeiter = mapPharze.find(g_pharsesample.firstword + g_pharsesample.secondword);		//Judge whether the phrase can be found, the number of it is increased, and it is inserted into the map
		if (pharzeiter != mapPharze.end())
		{
			pharzeiter->second.frequency++;
		}
		else
		{
			mapPharze.insert(pair<string, Pharze>(g_pharsesample.firstword + g_pharsesample.secondword, g_pharsesample));
		}
		g_pharsesample.firstword.clear();		 //Re initialize the phrase global variable to prepare to continue to accept the next phrase 
		g_pharsesample.secondword.clear();
		g_pharsesample.frequency = 1;
		g_pharsesample.originword.clear();
	}
}
void CounterWord(string singleword, unordered_map<string, MyWord> &mapWord, unordered_map<string, Pharze> &mapPharze)		//Count the total number of words and the number of words and phrases
	{
	if (size(singleword) > 1024) { return; }
	unordered_map<string, MyWord>::iterator worditer;
	unordered_map<string, MyWord>::iterator worditer_s;
	int wordend = 0;    //Used to record the end of the word 
	int numberinit = 0;	//Used to record the starting position of a number in a word
	string word_prefix; //Used to record prefixes of words
	MyWord word_detail; //Used to record full words and frequencies
	word_detail.frequency = 1;
	g_Wordnumber++;
	wordend = size(singleword);
	for (numberinit = wordend-1; JudgeNumber(singleword.at(numberinit)); numberinit--){}
	numberinit++;										//Find the starting position of the number
	word_prefix = singleword.substr(0, numberinit);		//Record prefix
	word_detail.originword = singleword;
	worditer = mapWord.find(word_prefix);				//Whether there is the same prefix in map
	if (worditer != mapWord.end())
	{
		worditer->second.frequency++;					//The word frequency plus one of the word
		if (strcmp(word_detail.originword.c_str(), worditer->second.originword.c_str())>0)		//Find the lexicographic sorting earlier in the map
			{
					worditer->second.originword = word_detail.originword;
			}
	}
	else
	{
		mapWord.insert(pair<string, MyWord>	(word_prefix, word_detail));						//If you can't find it, insert it in map
	}
	CounterPhrase(word_prefix, mapPharze);
}

void SearchFile(string folderpath, unordered_map<string,MyWord> &mapWord, unordered_map<string,Pharze> &mapPharze)	//Traverse all files
{
	char fileword;			//The character used to store and read in a file
	bool isWord=false;
	string singleword;		//It is used to record words when conditions are satisfied
	ifstream readfile;
	_finddata_t fileinfo;
	string deep_path = folderpath + "\\*.*";	//Find all the files in the current folder
	long Handle = _findfirst(deep_path.c_str(), &fileinfo);		//Looking for the first file handle
	if (Handle == -1)		//When the folder is empty, it returns directly
	{
		cout<<"The file is at the end"<<endl;
		return ;
	}
	do
	{
		if (fileinfo.attrib&_A_SUBDIR)
		{
			if ((strcmp(fileinfo.name, ".") != 0)&&(strcmp(fileinfo.name,"..")!=0))	//Used to determine whether or not a folder is a folder
			{
				string newpath = folderpath + "\\" + fileinfo.name;		//If it is, then continue to call the function recursively
				SearchFile(newpath,mapWord,mapPharze);
			}
		}
		else
		{
			readfile.open(folderpath + "\\" + fileinfo.name);	//Open the current folder
			if (!readfile.is_open())
			{
				cout << folderpath+"\\"+fileinfo.name<<"fail to open" << endl;
				exit(1);
				system("pause");
			}
			cout << folderpath+"\\"+ fileinfo.name << endl;
			while (!readfile.eof())
			{
				fileword = readfile.get();//Take a word out of the document
				CounterCharacter(fileword);
				if (isWord)					//Judge whether the word has been made up.
				{
					if (JudgeLetter(fileword) || JudgeNumber(fileword))
					{
							singleword = singleword + fileword;
					}
					else
					{
						isWord = false;		//No words can be made up again
						CounterWord(singleword, mapWord, mapPharze);					//Transfer words
						singleword.clear();
					}
				}
				else {
					if (JudgeLetter(fileword))
					{
						g_circle--;
						if (singleword.empty())
						{
							singleword = fileword;
						}
						else
						{
							singleword = singleword + fileword;
						}
					}
					else
					{
						g_circle = 4;
						singleword.clear();
					}
					if (g_circle == 0)		//The description has met the 4 consecutive characters as the English alphabet
					{
						isWord = true;
						g_circle = 4;
					}
				}
			}
			if (fileword != '\n')	g_LineNumber++;			//A file used to consider only one line without a newline character
			cout << folderpath + "\\" + fileinfo.name << "已经打开完成" << endl;
			readfile.close();
		}
	} while (_findnext(Handle, &fileinfo) != -1);
	_findclose(Handle);
}
void WordMerge(unordered_map<string, MyWord> &mapWord)
{
	unordered_map<string, MyWord>::iterator worditer;
	unordered_map<string, MyWord>::iterator worditer_s;
	for (worditer = mapWord.begin(); worditer != mapWord.end(); worditer++)
	{
		for (worditer_s = mapWord.begin(); worditer_s != mapWord.end(); worditer_s++)		//Conversion case
		{

				if (JudgeCase(worditer->first, worditer_s->first))
				{
					worditer->second.frequency = worditer_s->second.frequency+worditer->second.frequency;
					worditer_s->second.frequency = 0;
					worditer_s->second.Maxword = worditer->first;
				}
		}
	}
}

void WPSort(unordered_map<string, MyWord> &mapWord, unordered_map<string, Pharze> &mapPharze)		//Words and phrases with the highest statistical frequency
{
	ofstream write;
	string word[10];					//Record 10 most frequent words
	int wordfrequency[10] = { 0 };		//Record the number of the 10 most frequent words
	string pharze[10];					//Record 10 most frequent phrases
	int pharsefrequency[10] = { 0 };	//Record the number of 10 most frequent phrases
	unordered_map<string, MyWord>::iterator worditer;
	unordered_map<string, Pharze>::iterator pharzeiter;
	unordered_map<string, Pharze>::iterator pmax;			//Used to record the largest pharze in each round sort
	unordered_map<string, MyWord>::iterator wmax;			//Used to record the largest word in each round sort
	unordered_map<string, MyWord>::iterator findorigin1;	//To retrieve the original word, 1 represents the first word, 2 represents second words.
	unordered_map<string, MyWord>::iterator findorigin2;
	bool isPlacew=false;									//TO judge whether word has been placed 
	bool isPlacep = false;
	write.open("C:\\Users\\欧阳炳濠\\Desktop\\result.txt");
	for (int i = 0; i < 10; i++)
	{
		for (pharzeiter = mapPharze.begin(); pharzeiter != mapPharze.end(); pharzeiter++)
		{
			if (pharzeiter->second.frequency > pharsefrequency[i])
			{
				findorigin1 = mapWord.find(pharzeiter->second.firstword);
				if (findorigin1->second.frequency == 0) { pharzeiter->second.originword = findorigin1->second.Maxword; }
				else { pharzeiter->second.originword = findorigin1->first; }
				findorigin2 = mapWord.find(pharzeiter->second.secondword);
				if (findorigin2->second.frequency == 0) { pharzeiter->second.originword =pharzeiter->second.originword+" "+ findorigin2->second.Maxword; }
				else { pharzeiter->second.originword = pharzeiter->second.originword+" "+ findorigin2->first; }
				pharsefrequency[i] = pharzeiter->second.frequency;
				pharze[i] = pharzeiter->second.originword;
				pmax = pharzeiter;
				isPlacew = true;
			}
		}
		if (isPlacew)
		{
			mapPharze.erase(pmax);
			isPlacew = false;
		}
	}
	for (int i = 0; i < 10;i++)
	{
		for (worditer = mapWord.begin(); worditer != mapWord.end(); worditer++)
		{
			if (worditer->second.frequency > wordfrequency[i])
			{
				wordfrequency[i] = worditer->second.frequency;
				word[i] = worditer->first;
				wmax = worditer;
				isPlacep = true;
			}

		}
		if (isPlacep)
		{
			mapWord.erase(wmax);
			isPlacep = false;
		}
	}
	write << "Line Number" << g_LineNumber << endl;
	write << "Character Number" << g_CharacterNumber << endl;
	write << "Word Number" << g_Wordnumber << endl;
	write << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;		//The following are the things stored in the txt file
	write << "The number of word" << endl;
	for (int i =0; i < 10; i++)
	{
		if (!word[i].empty()) 
		{
			cout << word[i] << "		" << wordfrequency[i] << endl;
			write << word[i] << "		" << wordfrequency[i] << endl;
		}
		else
		{
			cout << "The number of word is less than ten and has benn enumerated completely" << endl;
			write << "The number of word is less than ten and has benn enumerated completely" << endl;
			break;
		}
	}
	write << endl;
	write << endl;
	write << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	write << "The number of pharze" << endl;
	for (int i = 0; i < 10; i++)
	{
		if (!pharze[i].empty())
		{
			cout << pharze[i] << "		" << pharsefrequency[i] << endl;
			write << pharze[i] << "		" << pharsefrequency[i] << endl;
		}
		else
		{
			cout << "The number of pharze is less than ten, and has benn enumerated completely" << endl;
			write << "The number of pharze is less than ten, and has benn enumerated completely" << endl;
			break;
		}
	}

}

int main(int argc,char **argv)				 //Get the folder path with the command line parameters
{
	string path = argv[1];			//Path command line
	unordered_map<string, Pharze> mapPharze;
	unordered_map<string, MyWord> mapWord;
	g_pharsesample.frequency = 1;			//Initial variables for initializing phrases
	SearchFile(path,mapWord,mapPharze);
	WordMerge(mapWord);						//Merge and write, spend a lot of time
	WPSort(mapWord, mapPharze);		
	cout << "行数总数"<<g_LineNumber << endl;
	cout << "字符总数" << g_CharacterNumber << endl;
	cout << "单词总数" << g_Wordnumber << endl;
	system("pause");
    return 0;
}
