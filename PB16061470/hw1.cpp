#include "io.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unordered_map"

using namespace std;
   
#define small 2

int wordnum = 0;
int charnum = 0;
int linenum = 0;

struct wordsdata                 //words data
{
	char words[1024];
	int number;
	wordsdata *next;
};
struct phrases
{
	char *one;
	char *tw;
	int num;
};

int wordcmp(char *str1, char *str2);
int gettop(struct wordsdata **word);
int getwords(char *path, struct wordsdata **word);
int getfiles(char *path, struct _finddata_t *fileinfo, long handle);

struct phrase_cmp
{
	bool operator()(const phrases &p1, const phrases &p2) const
	{
		return ((wordcmp(p1.one, p2.one) < 2) && (wordcmp(p1.tw, p2.tw) < 2));
	}

};
struct phrase_hash
{
	size_t operator()(const phrases &ph) const
	{
		unsigned long __h = 0;
		int temp;
		size_t i;
		for (i = 0; ph.one[i]; i++)
		{
			temp = ph.one[i];
			if (temp > 64)
			{
				(temp > 96) ? (temp - 96) : (temp - 64);
				__h += (29 * __h + temp);
				__h %= 2147483647;
			}

		}
		for (i = 0; ph.tw[i]; i++)
		{
			temp = ph.tw[i];
			if (temp > 64)
			{
				(temp > 96) ? (temp - 96) : (temp - 64);
				__h += (29 * __h + temp);
				__h %= 2147483647;
			}
		}

		return size_t(__h);
	}

};

typedef unordered_map<phrases, int, phrase_hash, phrase_cmp> Char_Phrase;

Char_Phrase phrasemap;
struct wordsdata *fourletter[26 * 26 * 26 * 26] = {}; //first four letters

int main()
{
	int j = 0;                            
	long handle = 0;                      
	struct _finddata_t fileinfo;               // filestruct 
	char *path = __argv[1];
	
	getfiles(path, &fileinfo, handle);

	gettop(fourletter);

	system("pause");
	return 1;
}

int getfiles(char *path, struct _finddata_t *fileinfo, long handle)
{                                    
	handle = _findfirst(path, fileinfo);            //first open 
	if (handle == -1)
		return -1;


	do
	{
		//printf("> %s\n", path);           

		if (fileinfo->attrib & _A_SUBDIR)         
		{
			if (strcmp(fileinfo->name, ".") != 0 && strcmp(fileinfo->name, "..") != 0)
			{
				char temppath[1024] = "";              //to mark path
				long temphandle = 0;
				struct _finddata_t tempfileinfo;
				strcpy(temppath, path);
				strcat(temppath, "/*");

				temphandle = _findfirst(temppath, &tempfileinfo); 
				if (temphandle == -1)
					return -1;

				do                              
				{
					if (strcmp(tempfileinfo.name, ".") != 0 && strcmp(tempfileinfo.name, "..") != 0)
					{
						strcpy(temppath, path);
						strcat(temppath, "/");
						strcat(temppath, tempfileinfo.name);
						getfiles(temppath, &tempfileinfo, temphandle);
					}
				} while (_findnext(temphandle, &tempfileinfo) != -1);

				_findclose(temphandle);
			}

		} 
		else
			getwords(path, fourletter);


	} while (_findnext(handle, fileinfo) != -1);

	_findclose(handle);      

	return 1;

}

int getwords(char *path, struct wordsdata **word)
{
	FILE *fp;
	int j = 0;
	int cmp = 0;
	int num = 0;              
	char temp = 0;             
	int length = 0;

	char present[1024] = "";  

	char address[4] = "";
	struct wordsdata *q = NULL;
	struct wordsdata *pre = NULL;
	struct wordsdata *neword = NULL;
	struct wordsdata *now = NULL;
	struct wordsdata *previous = NULL;
	struct phrases *newphrase = NULL;

	if ((fp = fopen(path, "r")) == NULL)
	{
		//printf("error!!! \n", path);
		return 0;
	}
	linenum++;
	while (temp != -1)
	{
		
		temp = fgetc(fp);
		if (temp > 31 && temp < 127)
			charnum++;
		if (temp == '\n' || temp == '\r')
			linenum++;

		while ((temp >= '0' && temp <= '9') || (temp >= 'a' && temp <= 'z') || (temp >= 'A' && temp <= 'Z'))
		{
			if (length != -1 && length < 4)
			{
				if (temp >= 'A')  
				{
					present[length] = temp;
					address[length] = (temp >= 'a' ? (temp - 'a') : (temp - 'A'));
					length++;
				}
				else           
					length = -1;
			}
			else if (length >= 4)
			{
				present[length] = temp;
				length++;
			}
			temp = fgetc(fp);
			if (temp > 31 && temp < 127)
				charnum++;
			if (temp == '\n' || temp == '\r')
				linenum++;
		} // end while

		  
		if (length >= 4)
		{
			wordnum++;
			num = address[0] * 17576 + address[1] * 676 + address[2] * 26 + address[3];

			if (word[num] == NULL)
			{
				word[num] = new wordsdata;
				neword = new wordsdata;
				neword->number = 1;
				neword->next = NULL;
				strcpy(neword->words, present);
				word[num]->next = neword;
				now = neword;
			}
			else
			{
				pre = word[num];
				q = pre->next;
				cmp = wordcmp(q->words, present);

				while (cmp == small)
				{
					pre = q;
					q = q->next;
					if (q != NULL)
						cmp = wordcmp(q->words, present);
					else
						break;
				}
				if (q != NULL && cmp <= 1)
				{
					now = q;
					q->number++;
					if (cmp == 1)
						strcpy(q->words, present);				
				}

				else
				{
					neword = new wordsdata;
					neword->number = 1;
					strcpy(neword->words, present);
					pre->next = neword;
					neword->next = q;
					now = neword;
				}
			}

			if (previous != NULL)
			{
				newphrase = new phrases;

				newphrase->tw = now->words;
				newphrase->one = previous->words;
				
				unordered_map<phrases, int>::const_iterator got = phrasemap.find( *newphrase);
				if (got != phrasemap.end())
				{
					phrasemap[*newphrase]++;
				}
				else
				{
					phrasemap.insert(pair<phrases, int>(*newphrase, 1));
				}
			}
			previous = now;

			
			for (int j = 0; present[j] && j < 1024; j++)
				present[j] = 0;
		}
		length = 0;
	}

	fclose(fp);
	return 1;
}

int wordcmp(char *str1, char *str2)
{
	char *p1 = str1;
	char *p2 = str2;
	char q1 = *p1;
	char q2 = *p2;

	if (q1 >= 'a' && q1 <= 'z')
		q1 -= 32;

	if (q2 >= 'a' && q2 <= 'z')
		q2 -= 32;

	while (q1 && q2 && q1 == q2)
	{
		p1++;
		p2++;

		q1 = *p1;
		q2 = *p2;

		if (q1 >= 'a' && q1 <= 'z')
			q1 -= 32;

		if (q2 >= 'a' && q2 <= 'z')
			q2 -= 32;
	}

	while (*p1 >= '0' && *p1 <= '9')
		p1++;
	while (*p2 >= '0' && *p2 <= '9')
		p2++;

	if (*p1 == 0 && *p2 == 0)   
		return strcmp(str1, str2);

	if (q1 < q2)
		return 2;

	if (q1 > q2)
		return 3;

	return 4;
}

int gettop(struct wordsdata **word)
{
	int i = 0, j = 0;
	struct wordsdata *topw[12] = {};
	struct phrases *toph[12] = {};
	struct wordsdata *w = NULL;
	FILE *fp;
	fp = fopen("result.txt", "w");
	fprintf(fp,"characters:%d \nwords:%d \nlines:%d\n",  charnum,wordnum, linenum);

	for (j = 0; j < 12; j++)
	{		
		toph[j] = new struct phrases;
		toph[j]->num = 0;
		topw[j] = new struct wordsdata;
		topw[j]->number = 0;
	}
	for (i = 0; i < 456976; i++)
	{
		if (word[i] != NULL)
		{
			w = word[i]->next;
			while (w != NULL)
			{
				topw[11]->number = w->number;
				topw[11]->next = w;
				j = 11;
				while (j > 1 && topw[j]->number > topw[j - 1]->number)
				{
					topw[0] = topw[j];
					topw[j] = topw[j - 1];
					topw[j - 1] = topw[0];
					j--;
				}
				w = w->next;
			}
		}
	}
	for (j = 1; j < 11; j++)
	{
		if (topw[j]->number)
			fprintf(fp,"\n%s :%d", topw[j]->next->words, topw[j]->number);
	}
	for (Char_Phrase::iterator it = phrasemap.begin(); it != phrasemap.end(); it++)
	{
		toph[11]->one = it->first.one;
		toph[11]->tw = it->first.tw;
		toph[11]->num = it->second;
		j = 11;
		while (j > 1 && toph[j]->num > toph[j - 1]->num)
		{
			toph[0] = toph[j];
			toph[j] = toph[j - 1];
			toph[j - 1] = toph[0];
			j--;
		}
	}
	fprintf(fp, "\n");
	for (j = 1; j < 11; j++)
	{
		if (toph[j]->num)
			fprintf(fp,"\n%s %s :%d", toph[j]->one, toph[j]->tw, toph[j]->num);
	}
	fclose(fp);
	return 1;
}
