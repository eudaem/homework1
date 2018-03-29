#include <stdio.h>
#include <dirent.h>  
#include <fstream>  
#include <string.h>  
#include <vector>  
#include <iostream> 
#include <stdlib.h>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <time.h>
using namespace std;

typedef struct Words {
	string  Word;
	string  Number;
	int count = 0;
	Words *next;
}Words;

typedef struct Phrases {
	int Key1 = 0;
	int Offset1 = 0;
	int Key2 = 0;
	int Offset2 = 0;
	int count = 0;
	Phrases *next;
}Phrases;

void Read(char * Filename);
void Findwords(char * bytes, long Len);
void GetAllFiles(string path, vector<string>& files);

#define N1 1000000
#define N2 5000000
Words *List[N1 + 1];
Phrases *PList[N2 + 1];
clock_t Start,End;

long char_number = 0, line_number = 0, word_num = 0;
void listDir(char *path){

DIR *pDir;
struct dirent *ent;
int i=0;
char childpath[512];

pDir = opendir(path);
memset(childpath,0,sizeof(childpath));
while((ent=readdir(pDir))!=NULL){
	if(ent->d_type&DT_DIR){
		if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) continue;
		sprintf(childpath,"%s/%s",path,ent->d_name);
		listDir(childpath);
	}
	else{
		sprintf(childpath,"%s/%s",path,ent->d_name);
		Read(childpath);
	}
	}
closedir(pDir);	
}
 

void Read(char * Filename) {		
	FILE *f = fopen(Filename, "rb");
	if (f != NULL) {
		fseek(f, 0, SEEK_END);
		long pos = ftell(f);	
		fseek(f, 0, SEEK_SET);		
		char *bytes = (char*)malloc(pos);
		fread(bytes, pos, 1, f);	
		fclose(f);
		Findwords(bytes, pos);
		free(bytes); // free allocated memory
	}
}

int GetHashkey(string word) {
	int j = 0;
	int k = 0;
	int flag = word.length();
	for (j = 0; j < flag; j++) {
		k = (k * 31 + (word[j] > 92 ? word[j] - 32 : word[j]))% N1;
	}
	return  k;
}

int InsertHash(string word, string number,int key) {
	int Offset = 0;
	int k;
	Words *p;
	if (List[key] == NULL) {
		p = new(Words);
		p->Word = word;
		p->Number = number;
		p->count++;
		p->next = NULL;
		List[key] = p;
	}
	else {
		p = List[key];
		while ((k = strcasecmp(p->Word.c_str(), word.c_str())) && p->next) Offset++,p = p->next;
		if (k) {
			p->next = new(Words);
			p = p->next;
			p->Word = word;
			p->Number = number;
			p->count++;
			p->next = NULL;
			Offset++;
		}
		else {
			if (p->Word > word) p->Word = word;
			if (p->Number > number) p->Number = number;
			p->count++;
		}
	}
	return Offset;
}

void InsertPhraseHash(int Key1, int Offset1, int Key2, int Offset2) {
	int key = (hash<int>{}(Key1*5 + Offset1*3 + Key2*11 + Offset2*7) % N2);
	int k = 0;
	Phrases *p;
	if (PList[key] == NULL) {
	{	p = new(Phrases);
		p->Key1 = Key1;
		p->Key2 = Key2;
		p->Offset1 = Offset1;
		p->Offset2 = Offset2;
		p->count++;
		p->next = NULL;
		PList[key] = p; }
	}
	else {
		p = PList[key];
		while (!(k = ((Key1 == p->Key1 && Offset1 == p->Offset1) && (Key2 == p->Key2 && Offset2 == p->Offset2))) && p->next) p = p->next;
		if (!k) {
		{	p->next = new(Phrases);
			p = p->next;
			p->Key1 = Key1;
			p->Key2 = Key2;
			p->Offset1 = Offset1;
			p->Offset2 = Offset2;
			p->count++;
			p->next = NULL; }
		}
		else {
			p->count++;
		}
	}
}

void Findwords(char * bytes, long Len) {

	string word1, number1, word2, number2;
	int Key1 = 0, Key2 = 0, Offset1 = 0, Offset2 = 0;
	int flag = 1;
	for (int i = 0; i < Len; i++)
	{
		if (bytes[i] > 31 && bytes[i] < 127) char_number++;
		else if (bytes[i] == 10)  line_number++;

		if (((bytes[i] > 64 && bytes[i] < 91) || (bytes[i] > 96 && bytes[i] < 123))) {

			if (flag) {
				if (!(number1.empty()))	word1 += number1, number1.clear();

				word1 += bytes[i];
			}

		}
		else{
			if (bytes[i] > 47 && bytes[i] < 58) {


				if (flag) {
					if (word1.length() < 4) {
						word1.clear();
						number1.clear();
						flag = 0;
						continue;
					}
					number1 += bytes[i];
				}
			}
			else {
				if (word1.length() > 3) {

					word_num++;

					Key1 = GetHashkey(word1);
					Offset1 = InsertHash(word1, number1, Key1);

					if (Key2 != 0) InsertPhraseHash(Key2, Offset2, Key1, Offset1);
					Key2 = Key1;
					Offset2 = Offset1;
				}
				word1.clear();
				number1.clear();


				flag = 1;
			}

		}

	}
	if (word1.length() > 3) {

		word_num++;

		Key1 = GetHashkey(word1);
		Offset1 = InsertHash(word1, number1, Key1);
		if (Key2 != 0) InsertPhraseHash(Key2, Offset2, Key1, Offset1);
		word1.clear();
		number1.clear();
	}
	line_number++;
}

void WordSort(Words *p,Words *Max) {
	int i, j;
	for (i = 0; p->count > Max[i].count && i < 10; i++);
	for (j = 0; j < i - 1; j++) {
		Max[j] = Max[j + 1];
	}
	if (i == 10){
		Max[9] = *p;
	}
	else{
		if (p->count == Max[i].count) {
			if (p->Word < Max[i].Word) {
				if (i != 0) {	
					Max[i - 1] = Max[i];
				}
				Max[i] = *p;
			}
			else {
				if (i != 0) Max[i - 1] = *p;
		
			}
		}
		else {
			if (i != 0) Max[i - 1] = *p;	
	
		}
	}
}

int JudgePheasesEqual(Phrases *P1,Phrases *P2) {
	string word1, word2, word3, word4;

	Words *p1 = List[P1->Key1], *p2 = List[P1->Key2];
	Words *p3 = List[P2->Key1], *p4 = List[P2->Key2];

	for (int i = 0; i < P1->Offset1; p1 = p1->next, i++);
	word1 = p1->Word;

	for (int i = 0; i < P1->Offset2; p2 = p2->next, i++);
	word2 = p2->Word;

	for (int i = 0; i < P2->Offset1; p3 = p3->next, i++);
	word3 = p3->Word;

	for (int i = 0; i < P2->Offset2; p4 = p4->next, i++);
	word4 = p4->Word;

	if (word1 < word3 || ((word1 == word3) && word2 < word4)) return 1;

	else return 0;
}

void PhraseSort(Phrases *p,Phrases *Max) {
	int i, j;
	for (i = 0; p->count > Max[i].count && i < 10; i++);
	for (j = 0; j < i - 1; j++) {
		Max[j] = Max[j + 1];
	}
	if (i == 10) {
		Max[9] = *p;
	}
	else {
		if (p->count == Max[i].count) {
			if (JudgePheasesEqual(p,&Max[i])) {
				if (i != 0) {
					Max[i - 1] = Max[i];
				}
				Max[i] = *p;
			}
			else {
				if (i != 0) Max[i - 1] = *p;

			}
		}
		else {
			if (i != 0) Max[i - 1] = *p;

		}
	}
}

void Findmax() {
	ofstream ofn("result.txt");
	Words Max[10];
	Phrases PMax[10];
	Words *p = NULL;
	Phrases *P = NULL;
	ofn << "char_number:" << char_number << endl << "line_number:" << line_number << endl << "word_num:" << word_num << endl << endl;


	for (int i = 0; i < N1; i++) {
		p = List[i];
		while (p) {
			WordSort(p, Max);
			p = p->next;
		}
	}

	for (int i = 0; i < N2; i++) {
		P = PList[i];
		while (P) {

			PhraseSort(P, PMax);
			P = P->next;
		}
	}
	
	//Output

	ofn << "the top ten frequency of word :" << endl << endl << endl;
	for (int i = 9; i >= 0 && Max[i].count != 0; i--) {
		ofn << Max[i].Word <<Max[i].Number<< "   " << Max[i].count << endl;
	}
	ofn << endl << endl << "the top ten frequency of phrase : " << endl << endl << endl;

	for (int i = 9; i >= 0 && PMax[i].count != 0; i--) {
		p = List[PMax[i].Key1];
		for (int j = 0; j < PMax[i].Offset1; j++, p = p->next);
		ofn << p->Word << p->Number << "   ";

		p = List[PMax[i].Key2];
		for (int j = 0; j < PMax[i].Offset2; j++, p = p->next);
		ofn << p->Word << p->Number << "   ";

		ofn << PMax[i].count << endl;
	}
}

int main(int argc, char *argv[])
{	
	string DATA_DIR = argv[1];
	vector<string> files;
	listDir(argv[1]);	
	Findmax();
}
