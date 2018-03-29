

#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<ctype.h>
#include<stdlib.h>
#include<iostream> 
#include<dirent.h>
#include<sys/types.h>
#include<math.h>
#include<strings.h>



#define WORDLEN 5000                        
#define GROUPLEN 10000                                             
#define KEYMAX 26*26*26*26       
#define pathlong 500


int letter_sum = 0;
int toatalnum=0; 
int toatalgroup=0;
using namespace std;
int line_sum = 0; 
char lastword[WORDLEN] = { 0 };
int t = 0;
int filenumber = 0;
char top10_word[10][WORDLEN] = { 0 };
int top10_times[10] = { 0 };
char top10_group[10][GROUPLEN] = { 0 };
int top10_gro_times[10] = { 0 };



struct PAth
{
 	char dir[pathlong];
};
PAth path[2000];


struct hashele      
{
	char word[WORDLEN] ;
	int num;
	hashele* next;
};

struct hashele_gro      
{
	char word[GROUPLEN];
	int num;
	hashele_gro* next;
};

hashele* hashform[KEYMAX];    
hashele_gro* hashform_gro[KEYMAX];

void scanFile(char *szDir);
void readfile();
void readfile_gro();
int check_letter(char c); 
int check_num(char c);
unsigned int worfkey(const char* word);  
int comp(const void*a, const void *b);
void check_repeat(char *word);
void check_gro_repeat(char *word, char *lastword);
int min1(int *top);
void top10();
void topgroup10();


int main(int argc,char *argv[])
{
	int k,i;
	
        int m;
	for(k=0;k<2000;k++)
	 for(m=0;m<pathlong;m++)
		path[k].dir[m]='\0';
       	scanFile(argv[1]);
	readfile();
	readfile_gro();
	top10();
	topgroup10();

        FILE*fp1=NULL;
	fp1=fopen("result.txt","w");
	fprintf(fp1,"word_number: %d\n", toatalnum);
	fprintf(fp1,"line_number: %d\n", line_sum);
	fprintf(fp1,"char_number: %d\n", letter_sum);

	for (i = 0; i < 10; i++)
	fprintf(fp1,"%s  %d times\n", top10_word[i],  top10_times[i]);
	fprintf(fp1,"\n");
	for (i = 0; i < 10; i++)
	fprintf(fp1,"%s  %d times\n", top10_group[i],  top10_gro_times[i]);
	fprintf(fp1,"\n");
	
	fclose(fp1);	
    	fp1=NULL;
}

void scanFile(char *szDir)  
{  
    DIR *pDir = NULL;  
    struct dirent *pFile = NULL;  
    int i=0,j=0,k=0,m=0;
    char dirnew[pathlong]={0};
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
            scanFile(Path);  
        } else {  
           // printf("path:%s     fileName:%s\n", szDir, pFile->d_name);  
            strcpy(dirnew,szDir);
            strcat(dirnew,"/");
            strcat(dirnew,pFile->d_name);
            strcpy(path[filenumber++].dir,dirnew);
          //  printf("my path is %s\n",path[filenumber-1].dir);         
        }  
    }  
      
    closedir(pDir);  
}

void readfile()
{
	FILE *fp=NULL;   
	int  i = 0, j = 0, tt = 0;
	char ch, lc;
	int space_sig = 1;
	int word_sig = 0, word_true = 0, p;
	char word[WORDLEN]={0};
	char cpath[pathlong]={0};
	printf("%d \n",filenumber);
 	for(tt=0;tt<filenumber;tt++)
	{
	j=pathlong-1;
	while(check_letter(path[tt].dir[j]) != 1){j--;}
	cpath[0]=path[tt].dir[j-3];
	cpath[1]=path[tt].dir[j-2];
	cpath[2]=path[tt].dir[j-1];
	cpath[3]=path[tt].dir[j];
	if(strcmp(cpath,".ttf")==0) continue;
	if(strcmp(cpath,".svg")==0) continue;
//	if(strcmp(cpath,"woff")==0) continue;
	printf("%d ",tt);
	printf("path is %s \n", path[tt].dir);
	fp = fopen(path[tt].dir, "r");
//	printf("cccccccc\n");
	if (fp == NULL)    
	{
		printf("fail to open the file\n");
		exit(0);
	}

	int k = 0;

	while ((ch = fgetc(fp)) != EOF)
	{
		
		if(ch>=32 && ch<=127) letter_sum++;
		
		
		if (check_letter(ch) == 1 && space_sig == 1)
		{
			
			word_sig++;
			*(word + k) = ch;
			if (word_sig == 4)
			{
				word_true = 1;
			}
			k++;
		}

		else if (check_num(ch) == 1) 
		{
			if (word_sig >= 4) 
			{
				word_sig++;
				*(word + k) = ch;
				k++;
			}
			else 
			{
				word_sig = 0;
				space_sig = 0;
				k = 0;
			}
		}

		else 
		{
			*(word + k) = '\0';
			space_sig = 1; 
			if (word_true == 1) 
			{
	                 //       printf("%s\n ",word);
				check_repeat(word);
				toatalnum++;
	
			}
			word_sig = 0;
			word_true = 0;
			k = 0;
			for(p=0;p<WORDLEN;p++) word[p]='\0';
			
		}
		if (ch == '\n') line_sum++; 
		lc = ch;
		
	}
	
	fclose(fp);
	if (lc != '\n') line_sum++;

	}
}

void readfile_gro()
{
	FILE *fp=NULL;   
	int  i = 0, j = 0, tt = 0;
	char ch, lc;
	int space_sig = 1;
	int word_sig = 0, word_true = 0, p;
	char word[WORDLEN]={0};
	char cpath[pathlong]={0};
	printf("%d \n",filenumber);
 	for(tt=0;tt<filenumber;tt++)
	{
	j=pathlong-1;
	while(check_letter(path[tt].dir[j]) != 1){j--;}
	cpath[0]=path[tt].dir[j-3];
	cpath[1]=path[tt].dir[j-2];
	cpath[2]=path[tt].dir[j-1];
	cpath[3]=path[tt].dir[j];
	if(strcmp(cpath,".ttf")==0) continue;
	if(strcmp(cpath,".svg")==0) continue;
//	if(strcmp(cpath,"woff")==0) continue;
	printf("%d ",tt);
	

	printf("path is %s \n", path[tt].dir);
	fp = fopen(path[tt].dir, "r");

	if (fp == NULL)    
	{
		printf("fail to open the file\n");
		exit(0);
	}                            
	int k = 0;

	while ((ch = fgetc(fp)) != EOF)
	{
		


		if (check_letter(ch) == 1 && space_sig == 1) 
		{
			
			word_sig++;
			*(word + k) = ch;
			if (word_sig == 4) 
			{
				word_true = 1;
			}
			k++;
		}

		else if (check_num(ch) == 1) 
		{
			if (word_sig >= 4) 
			{
				word_sig++;
				*(word + k) = ch;
				k++;
			}
			else 
			{
				word_sig = 0;
				space_sig = 0;
				k = 0;
				for(p=0;p<WORDLEN;p++) lastword[p]='\0';
			}
		}

		else 
		{
			*(word + k) = '\0';
			space_sig = 1; 
			if (word_true == 1) 
			{
				check_gro_repeat(word,lastword);
				for(p=0;p<WORDLEN;p++) lastword[p]='\0';
				strcpy(lastword, word);
				toatalgroup++;
			}
	//		else for(p=0;p<WORDLEN;p++) lastword[p]='\0';
			word_sig = 0;
			word_true = 0;
			k = 0;
			for(p=0;p<WORDLEN;p++) word[p]='\0';
			
		}
	
	}
	
	fclose(fp);

	}
}
int check_letter(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return(1);
	return(0);
}

int check_num(char c)
{
	if (c >= '0' && c <= '9') return(1);
	return(0);
}

unsigned int worfkey(const char* word)  
{

	const signed char *p = (const signed char*)word;    
	unsigned int key = *p;
	if (*p <= 'Z')  
		key = *p - 'A';
	else
		key = *p - 'a';
	if (*(p + 1) <= 'Z')
		key = key * 26 + *(p + 1) - 'A';
	else
		key = key * 26 + *(p + 1) - 'a';
	if (*(p + 2) <= 'Z')
		key = key * 26 + *(p + 2) - 'A';
	else
		key = key * 26 + *(p + 2) - 'a';
	if (*(p + 3) <= 'Z')
		key = key * 26 + *(p + 3) - 'A';
	else
		key = key * 26 + *(p + 3) - 'a';
	return(key);
}

void check_repeat(char *word)
{
	unsigned int place = worfkey(word);
	int j = 0, k = 0, same = 0, t,u=0,i=0,m=0;
	hashele* p = hashform[place];

	while (p)
	{       
		for(j=50;j>0;j--)
		{
		  if(check_letter(p->word[j])==1) break;
		}

		for(u=50;u>0;u--)
		{
		  if(check_letter(word[u])==1) break;
		}


		if(u != j) same=0;
		else
		{
		   t=strncasecmp(p->word, word, j+1);
		   if(t==0) same=1;
		}

		if(same == 1)
		{
		

			if (strcmp(word, p->word) < 0) strcpy(p->word, word);


		p->num++;
		return;
		}
		p = p->next;
	}
	hashele* newele = (hashele*)malloc(sizeof(hashele)); 
	newele->num = 1;                         
	for(m=0;m<WORDLEN;m++) newele->word[m]='\0';
	strcpy(newele->word, word);              
	newele->next = hashform[place];          
	hashform[place] = newele;
	




}	

void check_gro_repeat(char *word, char *lastword)
{
	unsigned int place = worfkey(word);
	if(lastword[0] == '\0') return;
	if (lastword[0] != '\0')
	{
	unsigned int place1 = worfkey(lastword);


	int j = 0, k = 0, same = 0, t,u=0,i=0,m=0;
	char group[GROUPLEN]={0};
	hashele* p1 = hashform[place1];
	hashele* p2 = hashform[place];
	hashele_gro* q = hashform_gro[place1];
	



	
	while (p1)
	{       
		for(j=50;j>0;j--)
		{
		  if(check_letter(p1->word[j])==1) break;
		}

		for(u=50;u>0;u--)
		{
		  if(check_letter(lastword[u])==1) break;
		}

		if(u != j) same=0;
		else
		{
		   t=strncasecmp(p1->word, lastword, j+1);
		   if(t==0) same=1;
		}

		if(same == 1)
		{

			if (strcmp(lastword, p1->word) > 0) strcpy(group, p1->word);
			else strcpy(group, lastword);

		break;
		}
		p1 = p1->next;
	}
	if(same == 0) strcpy(group, lastword);

	strcat(group," ");

	same=0;
	while (p2)
	{       
		for(j=50;j>0;j--)
		{
		  if(check_letter(p2->word[j])==1) break;
		}

		for(u=50;u>0;u--)
		{
		  if(check_letter(word[u])==1) break;
		}

		if(u != j) same=0;
		else
		{
		   t=strncasecmp(p2->word, word, j+1);
		   if(t==0) same=1;
		}

		if(same == 1)
		{

		  {
			if (strcmp(word, p2->word) > 0) strcat(group, p2->word);
			else strcat(group, word);
		  }

		break;
		}
		p2 = p2->next;
	}
	if(same == 0) strcat(group, word);


	while (q)
	{       
		if(strcmp(q->word,group)==0) {q->num++;return;}
		q=q->next;
	} 



	hashele_gro* newele_gro = (hashele_gro*)malloc(sizeof(hashele_gro));  
	newele_gro->num = 1;                        
        for(m=0;m<GROUPLEN;m++) newele_gro->word[m]='\0';
	strcpy(newele_gro->word, group);
	newele_gro->next = hashform_gro[place1];          
	hashform_gro[place1] = newele_gro;

	}



}	















int min1(int *top) 
{
	int i, min, place=0;
	min = top[0];
	for (i = 0; i < 10; i++)
		if (top[i] < min)
		{
			place = i;
			min = top[i];
		}
	return(place);
}

void top10()
{
	
	int i, j, k, min_num, place;

	for (i = 0; i < KEYMAX; i++)  
	{
		if (hashform[i])
		{
			hashele* p = hashform[i];
			while (p)
			{
				place = min1(top10_times);
				min_num = top10_times[place];
				if (p->num > min_num)
				{
					strcpy(top10_word[place], p->word);
					top10_times[place] = p->num;
				}
				p = p->next;
			}
			
		}


	}

	int temp;
	char samp[WORDLEN];  
    for(int i=0;i<10;i++)  
    {    
        for(int j=i+1;j<10;j++)     
        {  
            if(top10_times[i]>top10_times[j])  
            {  
                temp=top10_times[i];  
                top10_times[i]=top10_times[j];  
                top10_times[j]=temp;
		strcpy(samp,top10_word[i]);
		strcpy(top10_word[i],top10_word[j]);
		strcpy(top10_word[j],samp);  
            }  
        }  
    }  
	for (i = 0; i < 10; i++)
		printf("%s  %d times\n", top10_word[i], top10_times[i]);
	printf("\n");

 return;
   


}

void topgroup10()
{

	int i, j, k, min_num, place;

	for (i = 0; i < KEYMAX; i++)  
	{
		if (hashform_gro[i])
		{
			hashele_gro* p = hashform_gro[i];
			while (p)
			{
				place = min1(top10_gro_times);
				min_num = top10_gro_times[place];
			
				if (p->num > min_num)
				{

					strcpy(top10_group[place], p->word);
					top10_gro_times[place] = p->num;
				}
				
				
				p = p->next;
			}

		}
	


	}

	int temp;
	char samp[WORDLEN];  
    for(int i=0;i<10;i++)  
    {    
        for(int j=i+1;j<10;j++)     
        {  
            if(top10_gro_times[i]>top10_gro_times[j])  
            {  
                temp=top10_gro_times[i];  
                top10_gro_times[i]=top10_gro_times[j];  
                top10_gro_times[j]=temp;
		strcpy(samp,top10_group[i]);
		strcpy(top10_group[i],top10_group[j]);
		strcpy(top10_group[j],samp);  
            }  
        }  
    }
for (i = 0; i < 10; i++)
		printf("%s  %d times\n", top10_group[i], top10_gro_times[i]);
	printf("\n");
  
return;
}

