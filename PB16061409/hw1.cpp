// ConsoleApplication2.cpp: ¶¨Òå¿ØÖÆÌ¨Ó¦ÓÃ³ÌÐòµÄÈë¿Úµã¡£
//
/*×ÜË¼Â·£ºhashnum´æ´¢×Ö·û´®µÄ¹þÏ£Öµ£¬ÓÃHashµÄÏÂ±êÀ´±íÊ¾£¬Hash´ËÏÂ±êÖÐ´æ
  ×Å×Ö·ûÖ¸ÕëµÄÏÂ±ê£¨¼´W[n]ÖÐµÄn£©£¬Í¬Ê±Fren[hashnum]´æ´¢¶ÔÓ¦Hash[hashnum]µÄÆµÂÊ
  ËùÒÔ×îÖÕÊä³öÊ±¶ÔFrenÊý×é½øÐÐ²éÕÒ³ö×î´óµÄ£¬È»ºóÕÒ³ö¶ÔÓ¦Hash±íÖÐµÄ×Ö·ûÖ¸ÕëÏÂ±í*/

// #include "stdafx.h"

#include "iostream"
#include <cstring>      // for strcpy(), strcat()
#include<experimental/filesystem>
//#include <io.h>
#define N 50331553
#define LONGMAX 1024

FILE *fin, *fout, *fin1, *fin2, *fin3;
char *W[20000000];
char *PH[20000000];
char *PL[20000000];
int Hash[N] = { 0 };
int Fren[N] = { 0 };
int HashP[N] = { 0 };
int FrenP[N] = { 0 };
long int chnum = 0, linenum = 0, wordsnum = 0;
int pp = 0, ii = 0;
using namespace std;
unsigned int BKDRHash(char *str);
unsigned int BKDRHashP(char *str1, char *str2);
bool Ascll(char c);
bool isc(char c);
bool is(char c);
void charnum(void);
void wordnum(void);
int Insert(char *str, int n);
int InsertP(char *str1, char *str2, int n);
bool Compare(char *str1, char *str2);
void listFiles(const char * dir);
int FindWord(char *str); 
int FindWord(char *str) {
	unsigned int hashnum;
	int p;
	hashnum = BKDRHash(str);
	int a;
	p = Hash[hashnum];
	if (W[p] == NULL)
		return 0;
	a = Compare(str, W[p]);

	while (a==0) {
		hashnum++;
		p = Hash[hashnum];
		a = Compare(str, W[p]);
	}
	return p;
}
unsigned int BKDRHashP(char *str1, char *str2) {
	unsigned int hashnum1;
	unsigned int hashnum2;
	hashnum1 = BKDRHash(str1);
	hashnum2 = BKDRHash(str2);
	return((hashnum1 + hashnum2) % N);
}
int InsertP(char *str1, char *str2, int n) {
	if (n == 0)
		return 0;
	int hashnum;
	int p, a, b, length;
	hashnum = BKDRHashP(str1, str2); 
	while (FrenP[hashnum]>0) {
		p = HashP[hashnum];
		a = Compare(PH[p], str1);
		b = Compare(PL[p], str2);
		if (a == 1 && b == 1) {
			FrenP[hashnum]++;
			return 1;
		}
		else
			hashnum = (hashnum + 1) % N;
	}

	HashP[hashnum] = n;
	FrenP[hashnum] = 1;
	return 0;
}
bool Compare(char *str1, char *str2) {
	char new1[LONGMAX], new2[LONGMAX];
	int n1, n2, i, n;
	n1 = strlen(str1);
	if (str2 == NULL) 
		return false;
	n2 = strlen(str2);
	n1--;
	n2--;
	while ((int)str1[n1] > 47 && (int)str1[n1] < 58) {
		n1--;
	}
	for (i = 0; i < n1 + 1; i++)
		new1[i] = str1[i];
	new1[n1 + 1] = '\0';  //È¥µôstr1Êý×Ö½áÎ²
	while ((int)str2[n2] > 47 && (int)str2[n2] < 58) {
		n2--;
	}
	for (i = 0; i < n2 + 1; i++)
		new2[i] = str2[i];
	new2[n2 + 1] = '\0';  //È¥µôstr2Êý×Ö½áÎ²
	n = strcasecmp(new1, new2);
	if (n == 0)
		return true;
	else
		return false;
}
int  Insert(char *str, int n) {
	int hashnum;
	int p, a, length;
	hashnum = BKDRHash(str); //printf("%d",hashnum);
	while (Fren[hashnum]>0) {
		//printf("%s\n", str);
		p = Hash[hashnum];
		a = Compare(W[p], str); //printf("%d", a);
		if (a == 1) {
			if (strcmp(W[p], str)> 0) {
				length = strlen(str);
				length++;
				free(W[p]);
				W[p] = (char *)malloc(length * sizeof(char));
				strcpy(W[p], str);
				Fren[hashnum]++;
				return 1;
			}
			Fren[hashnum]++;
			return 1;
		}
		else
			hashnum = (hashnum + 1) % N;
	}

	Hash[hashnum] = n;
	Fren[hashnum] = 1;
	return 0;
}
unsigned int BKDRHash(char *str)
{
	int n, i;
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..  
	unsigned int hash = 0;
	char copy[LONGMAX];
	n = strlen(str);
	n--;
	while ((int)str[n] > 47 && (int)str[n] < 58) {
		n--;
	}
	for (i = 0; i < n + 1; i++)
		copy[i] = str[i];
	copy[n + 1] = '\0';  //È¥µôÊý×Ö½áÎ²
	n = 0;
	while (copy[n] != '\0')
	{
		if ((int)copy[n] > 96 && (int)copy[n] < 123)
			copy[n] = copy[n] - 32;
		hash = hash * seed + copy[n];
		n++;
	}
	return ((hash & 0x7FFFFFFF) % N);//NÎªÒ»¸ö´óÖÊÊý
}
bool Ascll(char c) {
	if ((int)c >= 32 && (int)c <= 126)
		return true;
	else
		return false;
}
bool isc(char c) {
	if (((int)c > 47 && (int)c < 58) || ((int)c > 64 && (int)c < 91) || ((int)c > 96 && (int)c < 123))
		return true;
	else
		return false;
}
bool is(char c) {
	if (((int)c > 64 && (int)c < 91) || ((int)c > 96 && (int)c < 123))
		return true;
	else
		return false;
}
void charnum(void) {
	char ch;
	while ((ch = fgetc(fin)) != EOF)
	{
		if (Ascll(ch))
			chnum++;
		if (ch == '\n')
			linenum++;

	}
	linenum++;
}
void wordnum(void) {
	//int hashnum;
	char ch, ch1, ch2, ch3;
	int a, k, wordmax = 0, n, sign, signP = 0, length;//p,qÓëi,kÏà¶ÔÓ¦ÊÇ´Ê×éµÄÏÂ±ê
	unsigned int hashmax = 0;
	ch2 = fgetc(fin2);
	ch1 = fgetc(fin1);
	ch1 = fgetc(fin1);
	ch = fgetc(fin);
	ch = fgetc(fin);
	ch = fgetc(fin);
	ch3 = fgetc(fin3); //putchar(ch3);
	ch2 = fgetc(fin2); //putchar(ch2);
	ch1 = fgetc(fin1); //putchar(ch1);
	ch = fgetc(fin); //putchar(ch) ;
	char Get[LONGMAX];
	PH[pp] = (char *)malloc(sizeof(char));
	PH[pp][0] = '\0';
	while (ch != EOF) {
		if (is(ch1) && is(ch2) && is(ch3) && is(ch)) {
			wordsnum++;
			k = 0;
			Get[k] = ch3;
			k++;
			Get[k] = ch2;
			k++;
			Get[k] = ch1;
			k++;
			while (isc(ch)) {
				Get[k] = ch;
				k++;
				ch3 = fgetc(fin3);
				ch2 = fgetc(fin2);
				ch1 = fgetc(fin1);
				ch = fgetc(fin);
			}
			Get[k] = '\0';
			length = strlen(Get) + 1;
			W[ii] = (char *)malloc(length * sizeof(char));
			PL[pp] = (char *)malloc(length * sizeof(char));
			PH[pp + 1] = (char *)malloc(length * sizeof(char));
			strcpy(W[ii], Get);
			strcpy(PL[pp], Get);
			strcpy(PH[pp + 1], Get);
			sign = Insert(W[ii], ii);
			if (sign == 1) {
				free(W[ii]);
			}
			else ii++;
			signP = InsertP(PH[pp], PL[pp], pp);
			if (signP == 1) {

				free(PH[pp]);
				free(PL[pp]);
				PH[pp] = (char *)malloc(length * sizeof(char));
				strcpy(PH[pp], Get);
				free(PH[pp + 1]);
			}
			else pp++;
		}
		ch3 = fgetc(fin3);
		ch2 = fgetc(fin2);
		ch1 = fgetc(fin1);
		ch = fgetc(fin);
	}
	fclose(fin1);
	fclose(fin2);
	fclose(fin3);
}

void listFiles(const char * dir)
{
	namespace fs=std::experimental::filesystem;

	string name;

	for(auto &p: fs::recursive_directory_iterator(dir))
	{
		if(!fs::is_directory(p.path()))
		{
			name = p.path();
			fin=fopen(name.c_str(),"r");
			charnum();
			rewind(fin);
			fin1 = fopen(name.c_str(),"r");
			fin2 = fopen(name.c_str(),"r");
			fin3 = fopen(name.c_str(),"r");
			wordnum();
			fclose(fin);
			printf("%s close\n", name.c_str());
		}

	}

}


int main(int argc,char *argv[]) {
	int i;
	int err;   //C:\Users\LHR\source\repos\ConsoleApplication2\Release\ConsoleApplication2.exe
	char dir[200];
	char A[10][2*LONGMAX];
	int n1, n2;
	cout << "Enter a directory: ";
	//cin.getline(dir, 200);
	strcpy(dir,argv[1]);

	listFiles(dir);
	fout = fopen("result.txt","w");
	fprintf( fout,"char_number :%ld \nline_number  :%ld\n", chnum, linenum);
	fprintf( fout,"word_number :%ld\n\n", wordsnum);
	int x, y, r1, r2;
	for (x = 0; x<10; x++)
		for (y = x + 1; y < N; y++)
			if (FrenP[x] < FrenP[y]) {
				r1 = FrenP[x];
				FrenP[x] = FrenP[y];
				FrenP[y] = r1;
				r2 = HashP[x];
				HashP[x] = HashP[y];
				HashP[y] = r2;
			}
	if (W[FindWord(PH[HashP[9]])]) {
		for (i = 0; i < 10; i++) {

			n1 = strlen(W[FindWord(PH[HashP[i]])]);
			strcpy(A[i], W[FindWord(PH[HashP[i]])]);
			A[i][n1] = ' ';
			A[i][n1 + 1] = '\0';
			strcat(A[i], W[FindWord(PL[HashP[i]])]);
		}
	}
	for (x = 0; x<10; x++)
		for (y = x + 1; y < N; y++) 
			if (Fren[x] < Fren[y]) {
				r1 = Fren[x];
				Fren[x] = Fren[y];
				Fren[y] = r1;
				r2 = Hash[x];
				Hash[x] = Hash[y];
				Hash[y] = r2;
			}
	fprintf(fout, "the top ten frequency of word :\n");
	for (i = 0; i < 10; i++)
		fprintf(fout,"%s:%d\n", W[Hash[i]], Fren[i]);
	fprintf(fout, "\n\nthe top ten frequency of phrase:\n");
	for(i = 0 ;i < 10;i++)
		fprintf(fout, "%s:%d\n", A[i], FrenP[i]);
	fclose(fout);
	return 0;
}

