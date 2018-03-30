#include <stdio.h> 
#include<iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define N 100

int snumchar=0,snumline=0,snumword=0,file_num=0;

//26*26*26*26Hashtable 
typedef struct Hash{
	//char data[5]={0};
	struct Node *next;
}Hash;
Hash HT[456976];
Hash HTWG[456976];
typedef struct Node{
	char dword[300];
	int times;
	struct Node *next;
}Node;
//case insensitive strcmp
int my_strcmp(char wa[],char wb[])
{
	char w1[200]={0},w2[200]={0},w3[200]={0},w4[200]={0};
	strcpy(w1,wa);
	strcpy(w2,wb);
	int i=0,max1=0,max2=0;
	for (i=0;i<200;i++)
	{
		if (w1[i]>=65&&w1[i]<=90)
			w1[i]=w1[i]+32;
		if (w1[i]=='\0')
			break;
	}
	max1=i-1;
	for (i=0;i<200;i++)
	{	
		if (w2[i]>=65&&w2[i]<=90)
			w2[i]=w2[i]+32;
		if (w2[i]=='\0')
			break;
	}
	max2=i-1;
	for (i=max1;i>0;i--)
	{
		if (w1[i]>=97&&w1[i]<=122)
			break;
	}
	max1=i+1;
	for (i=max2;i>0;i--)
	{
		if (w2[i]>=97&&w2[i]<=122)
			break;
	}
	max2=i+1;
	for (i=0;i<max1;i++)
		w3[i]=w1[i];
	for (i=0;i<max2;i++)
		w4[i]=w2[i];
	if (strcmp(w3,w4)==0)
		return 0;
	else
		return strcmp(w3,w4);	
}
//establish Hash table
void Hasht(Hash HT[],char word[])
{
	int i,HTnum=0;
	Node *p=NULL,*q=NULL,*l=NULL;
	
	if (word[0]>=97&&word[0]<=122) 
		HTnum+=(word[0]-97)*26*26*26;
	else if(word[0]>=65&&word[0]<=90)
		HTnum+=(word[0]-65)*26*26*26;
		
	if (word[1]>=97&&word[1]<=122) 
		HTnum+=(word[1]-97)*26*26;
	else if(word[1]>=65&&word[1]<=90)
		HTnum+=(word[1]-65)*26*26;	
		
	if (word[2]>=97&&word[2]<=122) 
		HTnum+=(word[2]-97)*26;
	else if(word[2]>=65&&word[2]<=90)
		HTnum+=(word[2]-65)*26;	
		
	if (word[3]>=97&&word[3]<=122) 
		HTnum+=word[3]-97;
	else if(word[3]>=65&&word[3]<=90)
		HTnum+=word[3]-65;	
	
	if (HT[HTnum].next==NULL)
	{
		p=(Node*)malloc(sizeof(Node));
		strcpy(p->dword,word);
		p->times=1;
		p->next=NULL;
		HT[HTnum].next=p;
		p=NULL;
		
	}
	else
	{
		p=(Node*)malloc(sizeof(Node));
		q=(Node*)malloc(sizeof(Node));
		p=HT[HTnum].next;
		q=p->next;
		if (q==NULL)	
			if (my_strcmp(word,p->dword)>0)
			{
				l=(Node*)malloc(sizeof(Node));
				strcpy(l->dword,word);
				l->times=1;
				l->next=NULL;
				p->next=l;
				p=NULL;
				q=NULL;
				l=NULL;
			
			}	
			else if (my_strcmp(word,p->dword)==0)
			{
				if (strcmp(word,p->dword)>=0)
				{
					p->times=p->times+1;
					p=NULL;
					q=NULL;
				
				}
					
				else
				{
					strcpy(p->dword,word);
					p->times=p->times+1;
					p=NULL;
					q=NULL;
				
				}
				
			}
			else
			{
				l=(Node*)malloc(sizeof(Node));
				strcpy(l->dword,word);
				l->times=1;
				l->next=p;
				HT[HTnum].next=l;
				p=NULL;
				q=NULL;
				l=NULL;
			
			}
		while (q!=NULL)
		{
			if (my_strcmp(word,q->dword)>0)
			{
				p=q;
				q=q->next;
				if (q==NULL)
				{
					l=(Node*)malloc(sizeof(Node));
					strcpy(l->dword,word);
					l->times=1;
					l->next=NULL;
					p->next=l;
					p=NULL;
					q=NULL;
					l=NULL;
					
				}
			}
			else if (my_strcmp(word,q->dword)==0)
			{
				if (strcmp(word,q->dword)>=0)
				{
					q->times=q->times+1;
					p=NULL;
					q=NULL;
				
				}
					
				else
				{
					strcpy(q->dword,word);
					q->times=q->times+1;
					p=NULL;
					q=NULL;
					
				}
			}
			else if (my_strcmp(word,q->dword)<0&&my_strcmp(word,p->dword)>0)
			{
				l=(Node*)malloc(sizeof(Node));
				strcpy(l->dword,word);
				l->times=1;
				l->next=q;
				p->next=l;
				p=NULL;
				q=NULL;
				l=NULL;
				
			}
			else if (my_strcmp(word,p->dword)==0)
			{
				if (strcmp(word,p->dword)>=0)
				{
					p->times=p->times+1;
					p=NULL;
					q=NULL;
				
				}
					
				else
				{
					strcpy(p->dword,word);
					p->times=p->times+1;
					p=NULL;
					q=NULL;
					
				}
			}
			else
			{
				l=(Node*)malloc(sizeof(Node));
				strcpy(l->dword,word);
				l->times=1;
				l->next=HT[HTnum].next;
				HT[HTnum].next=l;
				p=NULL;
				q=NULL;
				l=NULL;
						
			}
		}	
	}
}

void Hashtwg(Hash HTWG[], char wg[])
{
	int HTnum = 0;
	Node *p = NULL, *q = NULL, *l = NULL;

	if (wg[0] >= 97 && wg[0] <= 122)
		HTnum += (wg[0] - 97) * 26 * 26 * 26;
	else if (wg[0] >= 65 && wg[0] <= 90)
		HTnum += (wg[0] - 65) * 26 * 26 * 26;

	if (wg[1] >= 97 && wg[1] <= 122)
		HTnum += (wg[1] - 97) * 26 * 26;
	else if (wg[1] >= 65 && wg[1] <= 90)
		HTnum += (wg[1] - 65) * 26 * 26;

	if (wg[2] >= 97 && wg[2] <= 122)
		HTnum += (wg[2] - 97) * 26;
	else if (wg[2] >= 65 && wg[2] <= 90)
		HTnum += (wg[2] - 65) * 26;

	if (wg[3] >= 97 && wg[3] <= 122)
		HTnum += wg[3] - 97;
	else if (wg[3] >= 65 && wg[3] <= 90)
		HTnum += wg[3] - 65;

	if (HTWG[HTnum].next == NULL)
	{
		p = (Node*)malloc(sizeof(Node));
		strcpy(p->dword, wg);
		p->times = 1;
		p->next = NULL;
		HTWG[HTnum].next = p;
		p = NULL;
	}
	else
	{
		p = (Node*)malloc(sizeof(Node));
		q = (Node*)malloc(sizeof(Node));
		p = HTWG[HTnum].next;
		q = p->next;
		if (q == NULL)
			if (strcmp(wg, p->dword)>0)
			{
				l = (Node*)malloc(sizeof(Node));
				strcpy(l->dword, wg);
				l->times = 1;
				l->next = NULL;
				p->next = l;
				p = NULL;
				q = NULL;
				l = NULL;
			}
			else if (strcmp(wg, p->dword) == 0)
			{
				p->times = p->times + 1;
				p = NULL;
				q = NULL;
			}
					
		
			else
			{
				l = (Node*)malloc(sizeof(Node));
				strcpy(l->dword, wg);
				l->times = 1;
				l->next = p;
				HTWG[HTnum].next = l;
				p = NULL;
				q = NULL;
				l = NULL;
			}
		while (q)
		{
			if (strcmp(wg, q->dword)>0)
			{
				p = q;
				q = q->next;
				if (q == NULL)
				{
					l = (Node*)malloc(sizeof(Node));
					strcpy(l->dword, wg);
					l->times = 1;
					l->next = NULL;
					p->next = l;
					p = NULL;
					q = NULL;
					l = NULL;
				}
			}
			else if (strcmp(wg, q->dword) == 0)
			{
					q->times = q->times + 1;
					p = NULL;
					q = NULL;
			}
			else if (strcmp(wg, q->dword)<0 && strcmp(wg, p->dword)>0)
			{
				l = (Node*)malloc(sizeof(Node));
				strcpy(l->dword, wg);
				l->times = 1;
				l->next = q;
				p->next = l;
				p = NULL;
				q = NULL;
				l = NULL;
			}
			else if (strcmp(wg, p->dword) == 0)
			{
					p->times = p->times + 1;
					p = NULL;
					q = NULL;
			}
			else
			{
				l = (Node*)malloc(sizeof(Node));
				strcpy(l->dword, wg);
				l->times = 1;
				l->next = HTWG[HTnum].next;
				HTWG[HTnum].next = l;
				p = NULL;
				q = NULL;
				l = NULL;
			}
		}
	}
}

int stchar(char filename[],Hash HT[],int flag)
{
	FILE *fp = NULL;
	int numchar = 0,numline=0,numword=0,wordlong=0;
	char word[200]={0};
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("open file fall..\n");
		return 0;
	}	
	int ch=0,chr=0;
	while (!feof(fp)) {
		ch=fgetc(fp);
		if (word[0]==0&&(chr<48||(chr>57&&chr<65)||(chr>90&&chr<97)||chr>122)&&((ch>=65&&ch<=90)||(ch>=97&&ch<=122)))
		{
			word[0]=ch;
			wordlong=1;
		}		
		else if (wordlong!=0&&((ch>=65&&ch<=90)||(ch>=97&&ch<=122)))
		{
			word[wordlong]=ch;
			wordlong++;
		}			
		else if (ch>=48&&ch<=57&&wordlong>=4)
		{
			word[wordlong]=ch;
			wordlong++;
		}
		else if (word!="\0"&&wordlong>=4)
		{
			if (flag==2)
			    Hasht(HT,word);
			else
			    numword++;
			word[0]=0;
			while (wordlong)
			{
				word[wordlong]=0;
				wordlong--;
			}		
			wordlong=0;
		}
		else
		{
			word[0]=0;
			while (wordlong)
			{
				word[wordlong]=0;
				wordlong--;
			}
			
			wordlong=0;
		}

		if (ch>=32&&ch<=126&&flag==1)			
			numchar++;		
		if (ch==10&&flag==1)		
			numline++;
		chr=ch;
	}
	if (flag==1)
	{
		//printf("%s\n",filename);
		snumchar+=numchar;
		snumline+=numline;
		snumword+=numword;
		file_num+=1;
	}

	//printf("\n %s have char_number:%d\tline-number:%d\t\tWord_number:%d\n\n", filename, numchar,numline,numword);
	fclose(fp);	
	return 0;
}

void Hashtup(Hash HT[],char word[])
{
	int i=0,HTnum=0;
	Node *p=NULL;
	p=(Node*)malloc(sizeof(Node));

	if (word[0]>=97&&word[0]<=122) 
		HTnum+=(word[0]-97)*26*26*26;
	else if(word[0]>=65&&word[0]<=90)
		HTnum+=(word[0]-65)*26*26*26;
		
	if (word[1]>=97&&word[1]<=122) 
		HTnum+=(word[1]-97)*26*26;
	else if(word[1]>=65&&word[1]<=90)
		HTnum+=(word[1]-65)*26*26;	
		
	if (word[2]>=97&&word[2]<=122) 
		HTnum+=(word[2]-97)*26;
	else if(word[2]>=65&&word[2]<=90)
		HTnum+=(word[2]-65)*26;	
		
	if (word[3]>=97&&word[3]<=122) 
		HTnum+=word[3]-97;
	else if(word[3]>=65&&word[3]<=90)
		HTnum+=word[3]-65;

	p=HT[HTnum].next;
	while (p)
	{
		if (my_strcmp(word,p->dword)==0)
		{
			strcpy(word,p->dword);
			break;
		}
		p=p->next;
	}
}

int stcharwg(char filename[],Hash HT[])
{

	FILE *fp = NULL;
	int wordlong=0;
	char word[300]={0},word1[300]={0},word2[300]={0},wg[300]={0};
		//printf("%s\n",filename);
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("file open fall.\n");
		return 0;
	}	

	while (!feof(fp)) {
		int ch=0;
		ch=fgetc(fp);		
		if((ch>=65&&ch<=90)||(ch>=97&&ch<=122))
		{
			word[wordlong]=ch;
			//printf("%d%c",ch,word[numword]);
			wordlong++;
		}			
		else if(ch>=48&&ch<=57&&wordlong>=4)
		{
			word[wordlong]=ch;
			wordlong++;
		}
		else if(word!="\0"&&wordlong>=4)
		{
			Hashtup(HT,word);
			strcpy(word1,word2);
			strcpy(word2,word);
			
			if (word1[0]!=0)
			{
				strcpy(wg,word1);
				strcat(wg," ");
				strcat(wg,word2);
				Hashtwg(HTWG,wg);
			}
			word[0]=0;
			while(wordlong)
			{
				word[wordlong]=0;
				wordlong--;
			}
			
			wordlong=0;
		}
		else
		{
			word[0]=0;
			while(wordlong)
			{
				word[wordlong]=0;
				wordlong--;
			}
			
			wordlong=0;
		}
		
	}
	
	fclose(fp);
	return 0;
}


int readFileList(char *basePath,int flag)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
    char path[1000];
    strcpy(path,basePath);
	
    //printf("%s\n",basePath);

    if ((dir=opendir(basePath)) == NULL)
    {
	if (flag==1)
	    stchar(basePath,HT,1);
	else if (flag==2)
	    stchar(basePath,HT,2);
	else if (flag==3)
	    stcharwg(basePath,HT);
	return 1;
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
	{
	    strcpy(path,basePath);
	    strcat(path,"/");
	    strcat(path,ptr->d_name);
	    //printf("\n\n%s\n\n",path);
	    if (flag==1)
	        stchar(path,HT,1);
	    else if (flag==2)
	        stchar(path,HT,2);
	    else if (flag==3)
	        stcharwg(path,HT);
	    
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
	}
        else if(ptr->d_type == 10)    ///link file
        {
	    strcat(path,basePath);
	    strcat(path,"/");
	    strcat(path,ptr->d_name);
	    if (flag==1)
	        stchar(path,HT,1);
	    else if (flag==2)
	        stchar(path,HT,2);
	    else if (flag==3)
	        stcharwg(path,HT);
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
	}
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base,flag);
        }
    }
    closedir(dir);
    return 1;
}

void printwten()
{
	int wfreq[10000]={0},freq[10]={0},num=0,max=0,flag=0,count=0; 
	Node *p=NULL;
	p=(Node*)malloc(sizeof(Node));
	int i=0,j=0;
	for (i=0;i<456976;i++)
	{
		p=HT[i].next;
		while (p!=NULL)
		{
			for (j=0;j<num;j++)
				if (wfreq[j]==p->times)
				{
					p=p->next;
					flag=1;
					break;
				}
			if (flag==0)
				{
					wfreq[num]=p->times;
					num=num+1;
					p=p->next;
				}
			flag=0;
		}
	}

	for (i=0;i<10;i++)
	{
		for (j=0;j<num;j++)
		if (wfreq[j]>max)
			max=wfreq[j];
		for (j=0;j<num;j++)
		if (wfreq[j]==max)
			wfreq[j]=0;	
		freq[i]=max;
		max=0;
	}
	printf("the top ten frequency of word:\n"); 
	FILE *fp;
	if((fp=fopen("Result.txt","a"))==NULL)
		printf("cannot open this file:Result.txt");
	fprintf(fp,"the top ten frequency of word:\n");
	fclose(fp);
	flag=0;
	for (i=0;i<10;i++)
		for (j=0;j<456976;j++)
		{
			p=HT[j].next;
			while (p!=NULL)
			{
				if (p->times==freq[i])
				{
					printf("%-30s%d\n",p->dword,freq[i]);
					FILE *fp;
					if((fp=fopen("Result.txt","a"))==NULL)
						printf("cannot open this file:Result.txt");
					fprintf(fp,"%-30s%d\n",p->dword,freq[i]);
					fclose(fp);
					count=count+1;
				}
				if (count==10)
				{
					flag=1;
					break;
				}
				p=p->next;
			}
			if (flag==1)
			{
				i=10;
				break;
			}
		}
}


void printwgten()
{
	int wfreq[10000]={0},freqwg[10]={0},num=0,max=0,flag=0,count=0; 
	Node *p=NULL;
	p=(Node*)malloc(sizeof(Node));
	int i=0,j=0;
	for (i=0;i<456976;i++)
	{
		p=HTWG[i].next;
		while (p!=NULL)
		{
			for (j=0;j<num;j++)
				if (wfreq[j]==p->times)
				{
					p=p->next;
					flag=1;
					break;
				}
			if (flag==0)
				{
					wfreq[num]=p->times;
					num=num+1;
					p=p->next;
				}
				flag=0;
		}
	}
	
	for (i=0;i<10;i++)
	{
		for (j=0;j<num;j++)
		if (wfreq[j]>max)
			max=wfreq[j];
		for (j=0;j<num;j++)
		if (wfreq[j]==max)
			wfreq[j]=0;	
		freqwg[i]=max;
		max=0;
	}
	
	printf("the top ten frequency of phrase:\n"); 
	FILE *fp;
	if((fp=fopen("Result.txt","a"))==NULL)
		printf("cannot open this file:Result.txt");
	fprintf(fp,"the top ten frequency of phrase:\n");
	fclose(fp);
	flag=0;
	for (i=0;i<10;i++)
		for (j=0;j<456976;j++)
		{
			p=HTWG[j].next;
			while (p!=NULL)
			{
				if (p->times==freqwg[i])
				{
					printf("%-60s%d\n",p->dword,freqwg[i]);
					FILE *fp;
					if((fp=fopen("Result.txt","a"))==NULL)
						printf("cannot open this file:Result.txt");
					fprintf(fp,"%-60s%d\n",p->dword,freqwg[i]);
					fclose(fp);
					count=count+1;
				}
				if (count==10)
				{
					flag=1;
					break;
				}
				p=p->next;
			}
			if (flag==1)
			{
				i=10;
				break;
			}
		}
}

int main(int argc,char* argv[])

{
	readFileList(argv[1],1);

	printf("char_number:%d\nline_number:%d\nword_number:%d\nfile_number:%d\n",snumchar,snumline,snumword,file_num);
	FILE *fp;
	if((fp=fopen("Result.txt","a"))==NULL)
		printf("cannot open this file:Result.txt");
	fprintf(fp,"char_number:%d\nline_number:%d\nword_number:%d\nfile_number:%d\n",snumchar,snumline,snumword,file_num);
	fclose(fp);	

	readFileList(argv[1],2);
	readFileList(argv[1],3);
	
	printwten();
	printwgten();		
}
