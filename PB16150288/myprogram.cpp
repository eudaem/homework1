#include <iostream>
#include <string>
#include <vector>
#include<dirent.h>
#include<iomanip>
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<fstream>
#define MAXINITLENGTH 3010349
#define PHRASELENGTH 16785407
#define LETTERH 64
#define LETTERL 96
#define NUMBER 48
#define DELIMITER 0
#define LETTERH 64
#define LETTERL 96
#define NUMBER 48
#define DELIMITER 0
extern long SumofChar;
extern long SumofWord;
extern long Sumofline;
extern long SumofPhrase;
extern int formarindex;
long SumofChar = 0;
long SumofWord = 0;
long Sumofline = 0;
long SumofPhrase = 0;
int formarindex = -1;//coout for the phrase


using namespace std;

ofstream fout("result.txt");//to output the result





void getallfiles(string homepath, vector<string> &files);
void getcontent(string path, char* &buffer, long &size);
void statistics(char* buffer, long size);
void outputresult();

int WhatKindChar(char c)
{
    if( ((int)c>=65) && ((int)c<=90) )
        return LETTERH;
    if( ((int)c>=97) && ((int)c<=122) )
        return LETTERL;
    if( ((int)c>=48) && ((int)c<=57) )
        return NUMBER;
    else
        return DELIMITER;
}


class HashTable
{
public:
	int* WordFrequency;
	char(*WordTable)[200];
	int* Formar;
	int* Latter;
	int* PhraseFrequency;
	HashTable()
	{
		WordFrequency = new int[MAXINITLENGTH];
		WordTable = new char[MAXINITLENGTH][200];
		Formar = new int[PHRASELENGTH];
		Latter = new int[PHRASELENGTH];
		PhraseFrequency = new int[PHRASELENGTH];
		for (int i = 0; i < MAXINITLENGTH; i++)
		{
			WordFrequency[i] = 0;
		}
		for (int i = 0; i < PHRASELENGTH; i++)
		{
			PhraseFrequency[i] = 0;
		}
	}

	int getwordnumber()
	{
		int result = 0;
		for (int i = 0; (i < MAXINITLENGTH); i++)
		{
			if (WordFrequency[i] != 0)
				result++;
		}
		return result;
	}
	long getphrasenumber()
	{
		long result = 0;
		for (int i = 0; i < PHRASELENGTH; i++)
		{
			if (PhraseFrequency[i] != 0)
				result++;
		}
		return result;
	}
	void get10words()
	{
		int index[10] = { 0 };
		for (int m = 0; m < 10; m++)
		{
			int flag = 0;
			for (int i = 0; i < MAXINITLENGTH; i++)
			{
				for (int c = 0; c < m; c++)
				{
					if (i == index[c])
						i++;
				}
				if (WordFrequency[i] > WordFrequency[flag])
				{
					flag = i;
				}
			}
			index[m] = flag;
			fout << WordTable[flag] << ": " << WordFrequency[flag] << endl;
		}
	}
	void get10phrases()
	{
		int index[10] = { 0 };
		for (int m = 0; m < 10; m++)
		{
			int flag = 0;
			for (int i = 0; i < PHRASELENGTH; i++)
			{
				for (int c = 0; c < m; c++)
				{
					if (i == index[c])
						i++;
				}
				if (PhraseFrequency[i] > PhraseFrequency[flag])
				{
					flag = i;
				}
			}
			index[m] = flag;
			fout << WordTable[Formar[flag]] << " " << WordTable[Latter[flag]] << ": " << PhraseFrequency[flag] << endl;
		}
	}
	int append(char* sample, int formarindex) {
		char tochangestring[200];//wordcompare doesn't changed, use string like before
		unsigned int index = hash(sample);//TODO
		int c;//speciallly for the char*
		for (int i = 1; WordFrequency[index] != 0 && (wordcompare(WordTable[index], sample, tochangestring)) == false; i++)
		{
			index = (index + i * i) % MAXINITLENGTH;
		}
		if (WordFrequency[index] == 0)
		{
			WordFrequency[index]++;
			//wordcompare(sample, sample, tochangestring);
			for (c = 0; sample[c] != '\0'; c++)
			{
				WordTable[index][c] = sample[c];
			}
			WordTable[index][c] = '\0';
		}
		else if (strcmp(tochangestring, WordTable[index]) == 0)
		{
			WordFrequency[index]++;
		}
		else if (strcmp(tochangestring, WordTable[index]) != 0)
		{
			WordFrequency[index]++;
			//tochangestring = wordcompare(sample, sample);
			for (c = 0; tochangestring[c] != '\0'; c++)
			{
				WordTable[index][c] = tochangestring[c];
			}
			WordTable[index][c] = '\0';
		}
		if (formarindex == -1)
			return index;
		//the append the phrase
		unsigned int phraseindex = hash2(formarindex, index);//hash again
		for (int i = 0; ((PhraseFrequency[phraseindex] != 0) && ((formarindex != Formar[phraseindex]) || (index != Latter[phraseindex]))); i++)
		{
			phraseindex = (phraseindex + i * i) % PHRASELENGTH;
		}
		if (PhraseFrequency[phraseindex] == 0)
		{
			PhraseFrequency[phraseindex]++;
			Formar[phraseindex] = formarindex;
			Latter[phraseindex] = index;
		}
		else
		{
			PhraseFrequency[phraseindex]++;
		}
		return (int)index;
	}
		int getwordfrequency(char* sample)
	{
		unsigned int index = hash(sample);
		char tochangestring[200];
		for (int i = 1; (WordFrequency[index] != 0) && (wordcompare(sample, WordTable[index], tochangestring) == false); i++)
		{
			index = (index + i * i) % MAXINITLENGTH;
		}
		return WordFrequency[index];
	}
	int getphrasefrequency(char* formar, char* latter)
	{
		int formarindex = hash(formar);
		char tochangestring[200];
		for (int i = 1; (WordFrequency[formarindex] != 0) && (wordcompare(formar, WordTable[formarindex], tochangestring) == false); i++)
		{
			formarindex = (formarindex + i * i) % MAXINITLENGTH;
		}
		int latterindex = hash(latter);
		for (int i = 1; (WordFrequency[latterindex] != 0) && (wordcompare(latter, WordTable[latterindex], tochangestring) == false); i++)
		{
			latterindex = (latterindex + i * i) % MAXINITLENGTH;
		}
		int phraseindex = hash2(formarindex, latterindex);//hash again
		for (int i = 0; ((PhraseFrequency[phraseindex] != 0) && ((formarindex != Formar[phraseindex]) || (latterindex != Latter[phraseindex]))); i++)
		{
			phraseindex = (phraseindex + i * i) % PHRASELENGTH;
		}
		return PhraseFrequency[phraseindex];
	}
private:
	unsigned int hash(char* sample)
	{//TODO: to pay attention on the loop bound
		long long result = 0;
		unsigned int i, j, m;
		int samplelength;
		for (samplelength = 0; sample[samplelength] != '\0'; samplelength++);//get the length of sample
		for (j = samplelength - 1; (j > 0) && (WhatKindChar(sample[j]) == NUMBER); j--);
		for (m = 0; m <= j; m++)
		{
			result += (((int)sample[m] - WhatKindChar(sample[m]))) * (unsigned int)pow(13, m);
		}
		result = result % MAXINITLENGTH;
		return (int)result;

	}
	unsigned int hash2(int formarindex, unsigned int latterindex)
	{
		unsigned phraseindex = (unsigned int)formarindex * 7 + latterindex * 23;
		phraseindex = phraseindex & PHRASELENGTH;
		return (int)phraseindex;
	}
	bool wordcompare(char* a, char* b, char *p)
	{/*
		this function is to compare a and b to see if they are the same wordorphrase of the
		if they are not the same , the function will return a empty string: std:string()/""
		if they are the same , the function will return a string to represent the one between them with higher rank in priority
	 */
		int c;//counte for char*
		int alen, blen;
		for (alen = 0; a[alen] != '\0'; alen++);
		for (blen = 0; b[blen] != '\0'; blen++);

		int wi, wj, wm;
		for (wi = alen - 1; (wi > 0) && (WhatKindChar(a[wi]) == NUMBER); wi--);
		for (wj = blen - 1; (wj > 0) && (WhatKindChar(b[wj]) == NUMBER); wj--);
		if (wi != wj)
			return false;//return a empty string ""
		else
		{
			for (wm = 0; (wm <= wi) && ((a[wm] - WhatKindChar(a[wm]))== (b[wm] - WhatKindChar(b[wm]))); wm++);//find the place starting to be different
			if (wm <= wi)
			{//is different in fommer part, return a empty string ""
				return false;
			}
			else
			{//is the same word
				if (strcmp(a, b) > 0)
				{
					for (c = 0; c < blen; c++)
						p[c] = b[c];
					p[c] = '\0';
				}
				else
				{
					for (c = 0; c < alen; c++)
						p[c] = a[c];
					p[c] = '\0';
				}
				return true;
			}
		}
	}
};
HashTable WordTable;


void getallfiles(string homepath, vector<string>& files)
{
    DIR* dir = opendir(homepath.c_str());
    struct dirent* ptr;
    string absolutePath;
    string subDirect;
    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".") != 0 && strcmp(ptr->d_name,"..") != 0)
        {
            subDirect.assign(homepath);
            if(ptr->d_type == 4)
            {
                subDirect += "/";
                subDirect += ptr->d_name;
                getallfiles(subDirect, files);
                //files.push_back(subDirect);
            }
            else if(ptr->d_type == 8)
            {
                string absolutePath = subDirect + "/";
                absolutePath += ptr->d_name;
                //cout << absolutePath << endl;
                files.push_back(absolutePath);
            }
        }
    }
}
void getcontent(string path, char *& buffer, long &size)
{
	FILE* fp;
	size_t result;
	buffer = NULL;
	//to open the file in the path
	fp = fopen(path.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	//to get the same size space in the RAM
	buffer = new char[size];
	//to copy the file
	result = fread(buffer, 1, size, fp);
	fclose(fp);

}
void statistics(char * buffer, long size)
{ 
	formarindex = -1;
	char sample[200] = { '\0' };//a sample word in the proceeding of finding words
	int samplelength = 0;
	int charkind;//a mark to show what kind of char it is
	//int c;//specially counting for char*
	for (int i = 0; i < size; i++)
	{
		//whole iterate to traverse the file
		charkind = WhatKindChar(*(buffer + i));//TODO:add accumulate SumChar and Sumofline in this function
        if((int)*(buffer+i)>=32 && (int)*(buffer+i)<=126)
		    SumofChar++;
		if (*(buffer + i) == '\n')
			Sumofline++;
		if ( samplelength < 4 && (charkind == LETTERH || charkind == LETTERL))
		{
			sample[samplelength] = *(buffer + i);
			samplelength++;
			sample[samplelength] = '\0';
			continue;
		}
		if ( samplelength >= 4 && (charkind != DELIMITER))
		{
			sample[samplelength] = *(buffer + i);
			samplelength++;
			sample[samplelength] = '\0';
			continue;
		}
		else
		{
			if (samplelength< 4)
			{
				sample[0] = '\0';
				samplelength = 0;
				continue;
			}
			else
			{//have found a word 
				//Low(sample);//TODO
				formarindex = WordTable.append(sample, formarindex);
				SumofWord++;
				SumofPhrase++; 
				sample[0] = '\0';
				samplelength = 0;
			}
		}
	}
	delete(buffer);
}


void outputresult()
{
    fout << "char_number" <<SumofChar << endl;
    fout << "line_number" << Sumofline << endl;
	fout << "word_number"<< SumofWord << endl;
	fout << "10 most common words are :" << endl;
	WordTable.get10words();
	fout << "10 most common phrases are:" << endl;
	WordTable.get10phrases();
}

int main(int argc, char* argv[])
{
	double cost;
	if (argc != 2) {
		cout << "wrong paremeter, please input the folder's full path";
		return 1;
	}
	string homepath = argv[1];
	vector<string> files;
	char* buffer = NULL;
	long size = 0;
	//get all the files names, and put them into vector<string> files
	getallfiles(homepath, files);
	for (int i = 0; i < files.size(); i++)
	{
		//transfer the content to the RAM at one time, and get the head pointer buffer
		getcontent(files[i], buffer, size);//buffer apply memory in this function
		/*
		all above have been tested
		*/
		statistics(buffer, size);//TODO:free the memory of buffer in this function
		//TODO
	}
	outputresult();
  	return 0;
}
