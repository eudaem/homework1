#include<stdio.h>
#include <stdlib.h>
#include<dirent.h>
#include<unistd.h> 
#include<string.h>
#include<malloc.h>
#include<ctype.h>
#include <cstring>        
#include <io.h>

#define Max 10000
#define pathlong 200
#define WORDLEN 270                     
#define WORDLENz 400                        
#define OUTWORD 0                         
#define INWORD  1                        
#define KEYMAX 4570000       

int a;
char dir[300] = {0};
int num1 = 0, num2 = 0, num2_ = 0, num3 = 0, num3_ = 0, num4=0, num4_=0;
int filenumber = 0;

void FileSearch(char *szDir);
void WriteAllFile(void);
char *mystrncpy(char *string, int n);
void display(void);
void a_cha(FILE *fp);
void b_cha(FILE *fp);
void insertaword(char* word);
void insertawordz(char* word);
unsigned int worfkey(char* word); 
char* trzumu(char* word);
int compare(char* word1, char* word2);
void storeword(char* word1, char* word2);
void qsot(void);
void qsotz(void);
int comp(const void*a, const void *b);
int compz(const void*a, const void *b);
void findword(char *word);
int isalph(char ch);

typedef struct Data {
	unsigned long size;
	char dir[500];
}Data;
Data data[Max];

char szTest[300];

struct hashele      
{
	char word[WORDLEN];
	int num;
	hashele* next;
};
hashele* hashform[KEYMAX];
hashele *(*sort);        

struct hashelez      
{
	char word[WORDLENz];
	int num;
	hashelez* next;
};
hashelez* hashformz[KEYMAX];
hashelez *(*sortz);



int main(int argc, char *argv[])
{	

	
	strcpy(dir, argv[1]);
	FileSearch(dir);//c:\\Users\\zyc11\\Desktop\\softpro\\newsample
	WriteAllFile();
	FILE *fp1,*fp2,*fp3;

	if ((fp1 = fopen("filenamee.txt", "r")) == NULL)
	{
		printf("no files\n");
		return 0;
	}

	while (!feof(fp1))// zi zimu hang
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest)-1,fp1);   
		a = strlen(szTest);
		if ((fp2 = fopen(mystrncpy(szTest, a), "r")) == NULL)
		{
			if (a ==0)
				break;
			else
			printf("The file %s can not be opened.\n",szTest);
			break;
		}
		else
		a_cha(fp2);
	}
	sort = (hashele **)calloc(num2, sizeof(hashele **));
	qsot();
	fclose(fp1);


	if ((fp3 = fopen("filenamee.txt", "r")) == NULL)
	{
		printf("ne files\n");
		return 0;
	}
	while (!feof(fp3))//cizu
	{
		memset(szTest, 0, sizeof(szTest));

		fgets(szTest, sizeof(szTest) - 1, fp3);   
		a = strlen(szTest);

		if ((fp2 = fopen(mystrncpy(szTest, a), "r")) == NULL)
		{
			if (a == 0)
				break;
			else
				printf("The file %s can not be opened.\n", szTest);
			break;
		}
		else
			b_cha(fp2);
	}
	sortz = (hashelez **)calloc(num4, sizeof(hashelez **));
	qsotz();
	for (int i = 0;i <= 9;i++)
	{
		if (num2_ <= i)
			break;
		printf("%s %d\n", sort[i]->word, sort[i]->num);

	}
	for (int i = 0;i <= 9;i++)
	{
		if (num4_ <= i)
			break;
		printf("%s %d\n", sortz[i]->word, sortz[i]->num);

	}
	
	fclose(fp3);

	printf("cha:%d\n", num1);
	printf("line:%d\n", num3);
	printf("word:%d\n", num2);
	printf("word group:%d\n", num4);
	display();

	
} 

void display(void)
{
	FILE*fp;
	fp = fopen("result.txt", "w");
	fprintf(fp, "characters: %d\n", num1);
	fprintf(fp, "words:%d\n", num2);
	fprintf(fp, "lines:%d\n", num3);
	fprintf(fp, "word groups:%d\n\n", num4);

	fprintf(fp, "the top ten word\n");
	for (int i = 0;i <= 9;i++)
	{
		if (num2_ <= i)
			break;
		fprintf(fp,"%s  %d\n", sort[i]->word, sort[i]->num);

	}
	fprintf(fp, "\n\n");
	fprintf(fp, "the top ten word group\n");
	for (int i = 0;i <= 9;i++)
	{
		if (num4_ <= i)
			break;
		fprintf(fp,"%s   %d\n", sortz[i]->word, sortz[i]->num);

	}

	fclose(fp);

}

void b_cha(FILE *fp)
{
	char ch;

	char word1[WORDLENz];
	char word2[WORDLENz];

	int state = OUTWORD;
	int statez = 1;
	int k = 0;
	ch = fgetc(fp);
	do
	{
		//if (ch < 0 || ch>127)
			//continue;
		if (statez == 1)
		{
			if (isalph(ch) || (ch >= 48 && ch <= 57))     
			{
				if (isalph(ch))
				{
					state = INWORD;

					*(word1 + k) = ch;                            
					k++;
				}
				else
				{
					if (k <= 3)
					{
						k = 0;
						state = OUTWORD;
						continue;
					}
					else
					{
						state = INWORD;
						*(word1 + k) = ch;                             
						k++;
					}

				}
			}
			else if (state == INWORD)
			{
				if (k <= 3)
				{
					k = 0;
					state = OUTWORD;
					continue;
				}
				else
				{
					state = OUTWORD;                 
					*(word1 + k) = '\0';
					findword(word1);
					k = 0;
					statez = 2;
					
				}
			}
			
		}
		else if (statez == 2)
		{
			if (isalph(ch) || (ch >= 48 && ch <= 57))     
			{
				if (isalph(ch))
				{
					state = INWORD;

					*(word2 + k) = ch;                             
					k++;
				}
				else
				{
					if (k <= 3)
					{
						k = 0;
						state = OUTWORD;
						continue;
					}
					else
					{
						state = INWORD;
						*(word2 + k) = ch;                             
						k++;
					}

				}
			}
			else if (state == INWORD)
			{
				if (k <= 3)
				{
					k = 0;
					state = OUTWORD;
					continue;
				}
				else
				{
					state = OUTWORD;                
					
					*(word2 + k ) = '\0';

					findword(word2);
					strcat(word1, " ");
					strcat(word1,word2);
					insertawordz(word1);
					strcpy(word1,word2);
			
					*(word2) = '\0';
					k = 0;
					num4++;
				}
			}


			
		}
	} while ((ch = fgetc(fp)) != EOF);

	fclose(fp);


}



void a_cha(FILE *fp)
{
	char ch;

	char word[WORDLEN];
	int state = OUTWORD;
	int k = 0;
	ch = fgetc(fp);
	do
	{
		//if (ch < 0 || ch>127)
			//continue;
		if (ch >= 32&&ch <=127)
		{
			num1++;}
			
		if (ch =='\n') {
				num3++; 
				num3_ = ch; 
			}
		

		if (isalph(ch)|| (ch >= 48 && ch <= 57))    
		{
			if (isalph(ch))
			{
				state = INWORD;
			
				*(word + k) = ch;                            
				k++;
			}
			else
			{
				if (k <= 3)
				{
					k = 0;
					state = OUTWORD;
					continue;
				}
				else
				{
					state = INWORD;
					*(word + k) = ch;                             
					k++;
				}

			}
		}
		else if ( state == INWORD)
			{
				if (k <= 3)
				{
					k=0;
					state = OUTWORD;
					continue;
				}
				else
				{
					state = OUTWORD;               
					*(word + k) = '\0';
					
					insertaword(word);    
					num2++;
					k = 0;
				}
			}
			
	} while ((ch = fgetc(fp)) != EOF);


	if (num3_ != '\n') {
		num3++;
	}
	fclose(fp);
	

}



void insertaword( char* word)
{
	unsigned int place = worfkey(word);   
	hashele* p = hashform[place];           
	while (p)            
	{
		if (compare(p->word, word) == 0)
		{
			p->num++;
			 storeword(p->word, word);
			return;
		}
		p = p->next;                     
	}

	hashele* newele = (hashele*)malloc(sizeof(hashele)); 
	newele->num = 1;                         
	strcpy(newele->word, word);              
	newele->next = hashform[place];         
	hashform[place] = newele;
}

void insertawordz(char* word)
{
	unsigned int place = worfkey(word);    
	hashelez* p = hashformz[place];            
	while (p)            
	{
		if (strcmp(p->word, word) == 0)
		{
			p->num++;
			return;
		}
		p = p->next;                     
	}

	hashelez* newele = (hashelez*)malloc(sizeof(hashelez));  
	newele->num = 1;                         
	strcpy(newele->word, word);              
	newele->next = hashformz[place];          
	hashformz[place] = newele;

}

unsigned int worfkey( char* word)  
{
	char p[5];
	int key;
	p[4] = 0;
	for (int i = 0;i <= 3;i++)
	{
		if (*(word + i) >= 'A'&&*(word + i) <= 'Z')                     
			p[i] = *(word + i) + ('a' - 'A');
		else
			p[i] = *(word + i);
		p[4] = p[4] + p[i];
		p[4] = p[4] % 10;

	}
	key = 175760 * (*p - 97) + 6760 * (*(p + 1) - 97) + 260 * (*(p + 2) - 97) + 10 * (*(p + 3) - 97) + p[4];
	return key;                                    

}

int compare(char* word1, char* word2)
{
	int len1, len2,i;
	int m, n;
	if (strcasecmp(word1, word2) == 0)
		return 0;
	else
	{
		len1 = strlen(word1);
		len2 = strlen(word2);
		
		for (i = len1 - 1;i >= 0;i--)		
			if (isalph(*(word1 + i)))
			{
				m = i;
				break;
			}

		for (i = len2 - 1;i >= 0;i--)		
			if (isalph(*(word2 + i)))
			{
				n = i;
				break;
			}
			if (m != n)
				return 1;
			else
				if (strncasecmp(word1, word2, m+1) == 0)
					return 0;
				else
					return 1;

	}		

}


void storeword(char* word1, char* word2)
{
	int t;
	t=strcmp(word1, word2);
	
	if (t>0)
		strcpy(word1, word2);
		return ;
}

void qsot(void) 
{
	int i;
	
	num2_ = 0;
	hashele* p;

	for (i = 0; i < KEYMAX; ++i)  
	{
		if (hashform[i])
		{
			p = hashform[i];
		
			while (p)
			{	
				*(sort + num2_) = p;
				num2_++;
				
				p = p->next;
			}
		}
	}
	qsort(sort, num2_, sizeof(sort[0]), comp);        


}

void qsotz(void)
{
	int i;
	num4_ = 0;
	hashelez* p;

	for (i = 0; i < KEYMAX; ++i)  
	{
		if (hashformz[i])
		{
			p = hashformz[i];

			while (p)
			{
				*(sortz + num4_) = p;
				num4_++;

				p = p->next;
			}
		}
	}
	qsort(sortz, num4_, sizeof(sortz[0]), compz);       


}
int comp(const void*a, const void *b)            
{
	struct hashele **aa = (hashele **)a;     
	struct hashele **bb = (hashele **)b;
	if (((*aa)->num) == ((*bb)->num))                    
		return(strcmp((*aa)->word, (*bb)->word)>0 ? 1 : -1);
	else
		return(((*aa)->num)>((*bb)->num) ? -1 : 1);      
}
int compz(const void*a, const void *b)             
{
	struct hashelez **aa = (hashelez **)a;     
	struct hashelez **bb = (hashelez **)b;
	if (((*aa)->num) == ((*bb)->num))                   
		return(strcmp((*aa)->word, (*bb)->word)>0 ? 1 : -1);
	else
		return(((*aa)->num)>((*bb)->num) ? -1 : 1);       
}


void findword(char *word)
{
	unsigned int place = worfkey(word);    
	hashele* p = hashform[place];             
	while (p)             
	{
		if (compare(p->word, word) == 0)
		{
			strcpy(word, p->word);
			return;
		}
		p = p->next;                     
	}
	return ;
}

void WriteAllFile(void)
{
	FILE*fp;
	fp = fopen("filenamee.txt", "w");
							  //fprintf(fp, "Total files is %d\n\n", i + 1);
	while (filenumber > 0)
	{
		filenumber--;
		fprintf(fp, "%s\n", data[filenumber].dir);
		
	}
	fclose(fp);
}




char *mystrncpy(char *st, int n)
{
	char *p = st;
	int i = 0;

	p[n - 1] = '\0';

	printf("%s\n", p);
	return p;

}

void FileSearch(char *szDir)
{
	DIR *pDir = NULL;
	struct dirent *pFile = NULL;
	int i = 0;
	char dirnew[pathlong] = { 0 };
	pDir = opendir(szDir);
	if (pDir == NULL) return;

	while ((pFile = readdir(pDir)) != NULL) {
		if (pFile->d_type & DT_DIR) {
			if (strcmp(pFile->d_name, ".") == 0
				|| strcmp(pFile->d_name, "..") == 0) continue;

			char Path[pathlong];
			int len = strlen(szDir);
			strncpy(Path, szDir, len + 1);
			if (szDir[len - 1] != '/') strncat(Path, "/", 2);
			strncat(Path, pFile->d_name, strlen(pFile->d_name) + 1);
			FileSearch(Path);
		}
		else {
		 
			strcpy(dirnew, szDir);
			strcat(dirnew, "/");
			strcat(dirnew, pFile->d_name);
			strcpy(data[filenumber++].dir, dirnew);
			

		}
	}

	closedir(pDir);
}
int isalph(char ch)
{
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return 1;
	else
		return 0;
}
