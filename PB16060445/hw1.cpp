#include<stdio.h>
#include<string.h> 
#include<math.h>
#include<iostream>
#include<cstdlib>
#include<algorithm>
#include <stdlib.h>  
#include <dirent.h>  
#include <sys/stat.h>  
#include <unistd.h>  
#include <sys/types.h> 
#define NUM 100000 

using namespace std;

int characters = 0, words = 0, lines = 0, k, l;
FILE *fin, *fout;

struct Word
{
	char c[1024];
	int n;
}word[NUM];

struct Column
{
	char c1[1024];
	char c2[1024];
};

struct Phrase
{
	Column p;
	int n;
}phrase[NUM];

void listDir(char*);
int JudgeOrder(char a[], char b[]);                             
void DictOrder(void);                          
void Count_Characters_Lines(void);            
void CountWords(void);                        
int  JudgeWordEqual(char a[], char b[]);     
int  JudgeWordEqualPlus(char a[], char b[]);      
void CountWordsFrequency(void);                

void HeapAdjustWord(int *a, int i, int size);   
void BuildHeapWord(int *a, int size);            
void HeapSortWord(int *a, int size);            

void HeapAdjustPhrase(int *a, int i, int size);
void BuildHeapPhrase(int *a, int size);
void HeapSortPhrase(int *a, int size);          

void CountPhrasesFrequency(void);             
int JudgePhraseEqual(Column p, char a[], char b[]);          
void statistics(void);                         



int main(int argc, char *argv[])
{
	char  *p;
	int a[NUM], b[NUM], i, j, t = 1, l = 1;
        listDir(argv[1]);
	fout = fopen("result.txt", "w");
	fprintf(fout, "char_number: %d\nline_number: %d\n", characters, lines);
	fprintf(fout, "word_number: %d\nthe top ten frequency of words: \n", words);

	for (i = 1; i<NUM ; i++)
	{
		if (word[i].n) { a[t] = i; t++; }
	}
	HeapSortWord(a, t - 1);
	
	fprintf(fout, "the top ten frequency of phrases: \n");

	for (i = 1; i<NUM; i++)
	{
		if (phrase[i].n) { b[l] = i; l++; }
	}
	HeapSortPhrase(b, l - 1);


	fclose(fout);
	return 0;
}

//子函数 

void listDir(char *path) 
{
     DIR *pDir ; 
     struct  dirent *ent ; 
     int i=0 ;
     char childpath[512];
     pDir=opendir(path); 
     memset(childpath,0,sizeof(childpath)); 
     while((ent=readdir(pDir))!=NULL)                                   
     {
          if(ent->d_type & DT_DIR)                     
          {
                  if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) continue;
                  sprintf(childpath,"%s/%s",path,ent->d_name);
                  listDir(childpath);
 
           }
           else
           {
                  sprintf(childpath,"%s/%s",path,ent->d_name);       
                  if ((fin = fopen(childpath, "r")) == 0) return ;
                  statistics();
                  fclose(fin);
            }
       }
}

int JudgeOrder(char a[], char b[])
{
	int i;
	for (i = 0; a[i] && b[i]; i++)
	{
		if (a[i] < b[i]) return -1;
		else if ((a[i] == b[i])) return 0;
		else return 1;
	}
	if (!a[i] && b[i]) return -1;
	else if (a[i] && !b[i]) return 1;
	else return 0;
}

void Count_Characters_Lines()
{
	char ch, ch1;
	while ((ch = fgetc(fin)) != EOF)
	{
		if (ch >= 32 && ch <= 126) characters++;
		if (ch == '\n') lines++;
		ch1 = ch;
	}
	if (ch1 != '\n') lines++;
}

void CountWords()
{
	char ch;
	int flag = 4, mark = 1;
	while ((ch = fgetc(fin)) != EOF)
	{
		if (flag > 0 && mark == 1 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
		{
			flag--;
			continue;
		}
		if (flag == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9'))
		{
			flag = 4; mark = 1; words++;
			continue;
		}
		if (flag > 0 && flag < 4 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z'))
		{
			flag = 4;
			if (ch >= '0' && ch <= '9') mark = 0;
			continue;
		}
		if (flag == 4 && mark == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9'))
			mark = 1;
	}
	if (flag == 0) words++;
}
	

int JudgeString(char a[], char b[], int n)
{
	int i = 0;
	for (; i<n; i++)
	{
		if (!(a[i] == b[i] || abs(a[i] - b[i]) == 32)) return 0;
	}
	return 1;
}

int JudgeWordEqual(char a[], char b[]) //from JudgeWordEqualPlus(char a[], char b[])
{
	int la = strlen(a), lb = strlen(b);
	char ax[1024], bx[1024];
	while (a[la - 1] >= '0' && a[la - 1] <= '9') la--; strncpy(ax, a, la); ax[la] = '\0';
	while (b[lb - 1] >= '0' && b[lb - 1] <= '9') lb--; strncpy(bx, b, lb); bx[lb] = '\0';
	if (stricmp(ax, bx) == 0)
	{
		if (strcmp(ax, bx) < 0) strcpy(b, a); 
		return 1;
	}
	return 0;
}

int JudgeWordEqualPlus(char a[], char b[])
{
	int i = 0, j, la = strlen(a), lb = strlen(b), flag = 0;
	if (la < lb)
	{
		for (i = lb - 1; i> la - 1; i--) if (!(b[i] >= '0' && b[i] <= '9')) return 0;
		while ((a[i] >= '0' && a[i] <= '9') && (b[i] >= '0' && b[i] <= '9')) i--;
		if (((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')))
		{
			j = i;
			for (; i >= 0; i--) if (!(a[i] == b[i]) && !(((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')) && abs(a[i] - b[i]) == 32)) return 0;
			for (i = 0; i <= j; i++)
			{
				if (a[i] < b[i]) { strcpy(b, a); break; }
				else if ((a[i] > b[i])) break;
			}
			if (!a[i] && b[i]) strcpy(b, a);
			return 1;
		}
		return 0;
	}
	else if (la > lb)
	{
		for (i = la - 1; i> lb - 1; i--) if (!(a[i] >= '0' && a[i] <= '9')) return 0;
		while ((a[i] >= '0' && a[i] <= '9') && (b[i] >= '0' && b[i] <= '9')) i--;
		if (((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')))
		{
			j = i;
			for (; i >= 0; i--) if (!(a[i] == b[i]) && !(((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')) && abs(a[i] - b[i]) == 32)) return 0;
			for (i = 0; i <= j; i++)
			{
				if (a[i] < b[i]) { strcpy(b, a); break; }
				else if ((a[i] > b[i])) break;
			}
			if (!a[i] && b[i]) strcpy(b, a);
			return 1;
		}
		return 0;
	}
	else {
		i = la - 1;
		while ((a[i] >= '0' && a[i] <= '9') && (b[i] >= '0' && b[i] <= '9')) i--;
		if (((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')))
		{
			j = i;
			for (; i >= 0; i--) if (!(a[i] == b[i]) && !(((a[i] >= 'A' && a[i] <= 'Z') || (a[i] >= 'a' && a[i] <= 'z')) && ((b[i] >= 'A' && b[i] <= 'Z') || (b[i] >= 'a' && b[i] <= 'z')) && abs(a[i] - b[i]) == 32)) return 0;
			for (i = 0; a[i] && b[i]; i++)
			{
				if (a[i] < b[i]) { strcpy(b, a); break; }
				else if ((a[i] > b[i])) break;
			}
			if (!a[i] && b[i]) strcpy(b, a);
			return 1;
		}
		return 0;
	}
}



void CountWordsFrequency()
{
	;
	int i = 0, j = 0, flag = 4, mark = 1, m = 1;
	char ch, b[200];
	while ((ch = fgetc(fin)) != EOF)
	{
		if (flag>0 && mark == 1 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
		{
			b[i] = ch; i++;
			flag--;
			continue;
		}

		if (flag == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9'))
		{
			flag = 4; words++; mark = 1;
			b[i] = '\0'; i = 0;  m = 0;

			for (j = 0; j < k; j++)
			{
				if (JudgeWordEqual(b, word[j].c) == 1) { m = 1; break; }

			}
			if (m) word[j].n++;
			else {
				word[k].n = 1; strcpy(word[k].c, b); k++;
			}
			continue;
		}

		if (flag == 0) { b[i] = ch; i++; continue; }

		if (flag>0 && flag<4 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z'))
		{
			flag = 4;  i = 0; memset(b, '\0', sizeof(b));
			if (ch >= '0' && ch <= '9') mark = 0; continue;
		}

		if (flag == 4 && mark == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9')) mark = 1;
	}
	if (flag == 0)
	{
		words++;
		b[i] = '\0'; m = 0;
		for (j = 0; j < k; j++)
		{
			if (JudgeWordEqual(b, word[j].c) == 1) { m = 1; break; }
		}
		if (m) word[j].n++;
		else {
			word[k].n = 1; strcpy(word[k].c, b); k++;
		}
	}

	fprintf(fout, "words: %d\n", words);
}

void CountPhrasesFrequency()
{
	int i = 0, j = 0, flag = 4, mark = 1, m = 1, n = 1;
	char ch, b[200], temp[200];
	while ((ch = fgetc(fin)) != EOF)
	{
		if (flag>0 && mark == 1 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
		{
			b[i] = ch; i++;
			flag--;
			continue;
		}

		if (flag == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9'))
		{
			flag = 4; words++; mark = 1;
			b[i] = '\0'; i = 0;  m = 0; n = 0;

			for (j = 1; j < k; j++)
			{
				if (JudgeWordEqual(b, word[j].c) == 1)
				{
					m = 1;
					break;
				}
			}

			if (m) word[j].n++;
			else {
				word[k].n = 1; strcpy(word[k].c, b); k++;
			}

			if (temp[0])
			{
				for (j = 1; j<l; j++)
				{
					if (JudgePhraseEqual(phrase[j].p, temp, b) == 1)
					{
						n = 1; break;
					}
				}
				if (n) phrase[j].n++;
				else {
					phrase[l].n = 1; strcpy(phrase[l].p.c1, temp); strcpy(phrase[l].p.c2, b); l++;
				}
			}
			strcpy(temp, b);
			continue;
		}

		if (flag == 0) { b[i] = ch; i++; continue; }

		if (flag>0 && flag<4 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z'))
		{
			flag = 4;  i = 0; memset(b, '\0', sizeof(b));
			if (ch >= '0' && ch <= '9')
			{
				mark = 0; continue;
			}
		}

		if (flag == 4 && mark == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9')) 
			mark = 1;
	}
	if (flag == 0)
	{
		words++;
		b[i] = '\0'; m = 0; n = 0;

		for (j = 1; j < k; j++)
		{
			if (JudgeWordEqual(b, word[j].c) == 1)
			{
				m = 1;
				break;
			}
		}

		if (m) word[j].n++;
		else {
			word[k].n = 1; strcpy(word[k].c, b); k++;
		}

		if (temp[0])
		{
			for (j = 1; j<l; j++)
			{
				if (JudgePhraseEqual(phrase[j].p, temp, b) == 1)
				{
					n = 1; break;
				}
			}
			if (n) phrase[j].n++;
			else {
				phrase[l].n = 1; strcpy(phrase[l].p.c1, temp); strcpy(phrase[l].p.c2, b); l++;
			}
		}
	}

	fprintf(fout, "words: %d\n", words);

}

int JudgePhraseEqual(Column p, char a[], char b[])
{
	if (JudgeWordEqual(a, p.c1)&& JudgeWordEqual(b, p.c2)) return 1;
	return 0;
}

void HeapAdjustWord(int *a, int i, int size)  
{
	int lchild = 2 * i;       
	int rchild = 2 * i + 1;    
	int max = i;            
	if (i <= size / 2)         
	{
		if (lchild <= size && word[a[lchild]].n >word[a[max]].n) max = lchild;
		if (rchild <= size && word[a[rchild]].n >word[a[max]].n) max = rchild;
		if (max != i)
		{
			swap(a[i], a[max]);
			HeapAdjustWord(a, max, size);    
		}
	}
}

void BuildHeapWord(int *a, int size)    
{
	int i;
	for (i = size / 2; i >= 1; i--)   
	{
		HeapAdjustWord(a, i, size);
	}
}

void HeapSortWord(int *a, int size)   
{
	int i, j, temp;
	BuildHeapWord(a, size);
	for (i = size; i >= 1; i--)
	{
		temp = a[1]; a[1] = a[i]; a[i] = temp;        
		HeapAdjustWord(a, 1, i - 1);     
		if (i == size - 9 || i == 1)
		{
			for (j = size; j >= (size>10 ? (size - 9) : 1); j--) fprintf(fout, "<%s> :%d\n", word[a[j]].c, word[a[j]].n);
			fprintf(fout, "\n"); return;
		}

	}

}

void HeapAdjustPhrase(int *a, int i, int size)  
{
	int lchild = 2 * i;       
	int rchild = 2 * i + 1;     
	int max = i;            
	if (i <= size / 2)          
	{
		if (lchild <= size && phrase[a[lchild]].n >phrase[a[max]].n) max = lchild;
		if (rchild <= size && phrase[a[rchild]].n >phrase[a[max]].n) max = rchild;
		if (max != i)
		{
			swap(a[i], a[max]);
			HeapAdjustPhrase(a, max, size);    
		}
	}
}

void BuildHeapPhrase(int *a, int size)    
{
	int i;
	for (i = size / 2; i >= 1; i--)   
	{
		HeapAdjustPhrase(a, i, size);
	}
}

void HeapSortPhrase(int *a, int size)    
{
	int i, j, temp;
	BuildHeapPhrase(a, size);
	for (i = size; i >= 1; i--)
	{
		temp = a[1]; a[1] = a[i]; a[i] = temp;         
		HeapAdjustPhrase(a, 1, i - 1);      
		if (i == size - 9 || i == 1)
		{
			for (j = size; j >= (size>10 ? (size - 9) : 2); j--) fprintf(fout, "<%s %s> :%d\n", phrase[a[j]].p.c1, phrase[a[j]].p.c2, phrase[a[j]].n);
			return;
		}
	}
}

int adress(char c[])
{
	char *p = c;
	long h = 0;
	for (; *p; p++)
	{
		if (*p > 64 && *p < 91) h = h + *p;
		else if (*p > 96 && *p < 123) h = h + *p -32;
	}
	return (int)(h % NUM);

}

void statistics()
{
	int i = 0, j = 0, flag = 4, mark = 1, m = 1, n = 1, ad, ad1, d;
	char ch, ch1, b[100], temp[100],*p;
	while ((ch = fgetc(fin)) != EOF)
	{
		if (ch == '\n') lines++; ch1 = ch;
		if (ch >= 32 && ch <= 126) characters++;
		if (flag>0 && mark == 1 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
		{
			b[i] = ch; i++;
			flag--;
			continue;
		}

		if (flag == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9'))
		{
			flag = 4; words++; mark = 1;
			b[i] = '\0'; i = 0;  m = 0; n = 0;

			ad = adress(b); d = 1; 
			while (word[ad].n  && JudgeWordEqual(b, word[ad].c) == 0)
			{
				ad = (ad + 5 ) % NUM; d++;
			}
			if (word[ad].n == 0)	strcpy(word[ad].c, b);
			word[ad].n++;

		  if (temp[0])
			{
				d = 1; 
				ad=adress(temp);
				while (phrase[ad].n && JudgePhraseEqual(phrase[ad].p, temp, b) == 0)
				{
					ad = (ad + 5) % NUM; d++;
				}
				if (phrase[ad].n == 0)
				{
					strcpy(phrase[ad].p.c1, temp); strcpy(phrase[ad].p.c2, b);
				}
				phrase[ad].n++;

			}
			strcpy(temp, b);
			continue;
		}

		if (flag == 0) { b[i] = ch; i++; continue; }

		if (flag>0 && flag<4 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z'))
		{
			flag = 4;  i = 0; memset(b, '\0', sizeof(b));
			if (ch >= '0' && ch <= '9')
			{
				mark = 0; continue;
			}
		}

		if (flag == 4 && mark == 0 && !(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9')) mark = 1;
	}
	if (ch1 != '\n') lines++;
	if (flag == 0)
	{
		words++;
		b[i] = '\0'; m = 0; n = 0;

		ad = adress(b); d = 1;
		while (word[ad].n  && JudgeWordEqual(b, word[ad].c) == 0)
		{
			ad = (ad + 5 ) % NUM; d++;
		}
		if (word[ad].n == 0)	strcpy(word[ad].c, b);
		word[ad].n++;

		if (temp[0])
		{
			d = 1;
			ad = adress(temp);
			while (phrase[ad].n && JudgePhraseEqual(phrase[ad].p, temp, b) == 0)
			{
				ad = (ad + 5 ) % NUM; d++;
			}
			if (phrase[ad].n == 0)
			{
				strcpy(phrase[ad].p.c1, temp); strcpy(phrase[ad].p.c2, b);
			}
			phrase[ad].n++;

		}
	}



}



