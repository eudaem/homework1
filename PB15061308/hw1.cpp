#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<ctype.h>
#include <iostream>
#include <algorithm>
#include <io.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include<dirent.h>
#include<unistd.h> 

using namespace std;                 
 												//此代码为Windows版本，其中的地址需要人工辨识。 
#define Name_Max 30    //文件名最大长度
#define Word_Max 100    //每个单词最大长度
#define pathlong 200

typedef struct word        //连表 单词结构
{
    char w[Word_Max];    //单词
    int count;                //个数
    struct word *next;
}link;
struct top
{
	char w[Word_Max];
	int fre;
}topfre[10]; 
struct wordgroup
{
	char lastword[100];
	char currentword[100];
	int count;
	char group[200];
}wgtop[2000000],wgtopput[10]; 

typedef struct Data {
	unsigned long size;
	char dir[500];
}Data;
Data data[10000]; 
char currentword[100];
char lastword[100];
int FileSearch(char dir[]);
int filenumber = -1;
int count1=0; 
int flag=1;
link *head=NULL;    //单词连表头
FILE *fp=NULL;       //文件指针
FILE *FP=NULL;
int  hang_num=0,letter=0; 

 
int FileSearch(char dir[])//递归遍历当前目录下的所有文件 最后文件名信息存入data[Max]里
{
	long handle;
	_finddata_t findData;
	char dirNew[300];
	strcpy(dirNew, dir);
//	strcat(dirNew, "\\*.*");   //如果是仅有一个文件，没有子目录，就需要把这句删去，在命令行写入完整路径即可。 
	if ((handle = _findfirst(dirNew, &findData)) == -1L)
	{
		printf("Failed to findfirst file");
		return -1;
	}
	if(_findnext(handle, &findData) == 0)
	{while (_findnext(handle, &findData) == 0)
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);

			FileSearch(dirNew);

		}
		else
		{
			if (++filenumber <10000)//将路径入栈
			{
				strcpy(data[filenumber].dir, dir);
				strcat(data[filenumber].dir, "\\");
				strcat(data[filenumber].dir, findData.name);
				data[filenumber].size = findData.size;
				printf("%s\n",data[filenumber].dir) ;
			}
		}
	}}
	else{	if (++filenumber <10000)//将路径入栈
			{
				strcpy(data[filenumber].dir, dir);
			 
				data[filenumber].size = findData.size;
				printf("%s\n",data[filenumber].dir) ;
			}}; 
	_findclose(handle);
}
/*void FileSearch(char *szDir)
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
}*/ 

bool com(top a,top b) 
{
    if (strcmp(a.w,b.w)<0) return true;
    else return false;
}

bool com2(wordgroup a,wordgroup b)
{
    if (strcmp(a.group,b.group)<0) return true;
    else return false;
}


void addwordgroup(char lastword[100],char currentword[100]) //边判断边加入 
{
	int i;
	char a[100],b[100],a1[100],b1[100];
	if(!wgtop[0].count)   
	{strcpy(wgtop[0].lastword,lastword);
	 strcpy(wgtop[0].currentword,currentword);
	 strcpy(wgtop[0].group,lastword);
	 strcat(wgtop[0].group,"  ");
	 strcat(wgtop[0].group,currentword);
	 wgtop[0].count++;
	}
	else 
	{  
			sscanf(lastword,"%[^0-9]",a);
			sscanf(currentword,"%[^0-9]",b);
		for( i=0;wgtop[i].count;i++)
		{
			sscanf(wgtop[i].lastword,"%[^0-9]",a1);
			sscanf(wgtop[i].currentword,"%[^0-9]",b1);
			if(!strcmp(a,a1)&&!strcmp(b,b1))
			{
				wgtop[i].count++;return;
			}
		}
	for( i=0;wgtop[i].count&&i<2000000;i++); 
	strcpy(wgtop[i].lastword,lastword);
	strcpy(wgtop[i].currentword,currentword); 
	strcpy(wgtop[i].group,lastword); 
	strcat(wgtop[i].group,"  ");
	strcat(wgtop[i].group,currentword);//printf("%s",wgtop[i].group);
	wgtop[i].count++;
	}
}
int isnotWord(char a)        //判断是否为字母
{
	if(a>=' '&&a<='~')
	{
		letter++;
	}
    if(a <= 'z' && a >= 'a'||a <= 'Z' && a >= 'A'||a>='1'&&a<='9')
    {
        return 0;
    }
    else if(a=='\n')
    {
        hang_num++;
        return 1;
    }
    else
    {
        return 1;
    }

}

void addWord(char *w1)        //添加单词
{

    link *p1,*p2;
    char str[Word_Max],str1[Word_Max];
    	sscanf(w1,"%[^0-9]",str1);

    for(p1=head;p1!=NULL;p1=p1->next)    //判断单词在连表中是否存在
    {
    	sscanf(p1->w,"%[^0-9]",str);
        if(!strcmp(	strlwr(str1), strlwr(str)))
        {
            p1->count++;   //存在就个数加1
            count1++;
            if(strcmp(w1,p1->w)<0)
            strcpy(p1->w,w1);
            return;
        }
    }

    p1=(struct word *)malloc(sizeof(word));//若不存在，则添加新单词
    strcpy(p1->w,w1);
    p1->count=1;
    p1->next=NULL;
    count1++;    //总的单词数加加

    if(head==NULL)
    {
        head=p1;//printf("%s\n",p1->w);
    }
    else
    {
        for(p2=head;p2->next!=NULL;p2=p2->next);
        p2->next=p1;
    }


}

void wordCount()    //统计单词
{
    int i=0,j=0;
    char word[Word_Max],wordad[Word_Max],c;
    while(!feof(fp))
    {
        c = fgetc(fp);
       // printf("%c %d\n",c,j);
        if(isnotWord(c))
        {
            word[j]='\0';
            if(j>3)
            {
                addWord(word);
               if(flag==1) {strcpy(lastword,word);  flag=0;goto d1;}
               if(flag==0) {strcpy(currentword,word);/*printf("%s\n",currentword);*/} 
               addwordgroup(lastword,currentword); strcpy(lastword,currentword);//printf("%s\n",lastword);
            }
      d1:      j=0;
          
        }
        else
        {

            word[j]=c;
            j++; if(j>=Word_Max)  j=0;
			
        }
        //count9(word);
        i++;
    }
}

 

void showWord()        //显示单词统计情况
{
	int i;
     fprintf(FP,"总的单词数：%d\n",count1);
     fprintf(FP,"总共有%d行\n",hang_num+1);
     fprintf(FP,"总共有%d字符\n",letter);
     fprintf(FP,"前十词频单词\t个数\n");
      for(i=0;i<10;i++)
     fprintf(FP,"%s\t\t%d\n",topfre[i].w,topfre[i].fre); 
    fprintf(FP,"前十词频词组\t个数\n");
	 for(i=0;i<10;i++)
    fprintf(FP,"%s\t\t%d\n",wgtopput[i].group,wgtopput[i].count); 
     fclose(FP);

}
void word_fre_put()
{
	link*p;  int i,j;
	for(i=0;i<10;i++)
   {
	for(p=head;p!=NULL;p=p->next)
	{    for(j=0;j<i;j++) if(strcmp(p->w,topfre[j].w)==0) {p=p->next;if(p==NULL) goto a1;j=-1;} //printf("%d",p->count);
	     
		if(p->count>topfre[i].fre) {topfre[i].fre=p->count;/*printf("%d",topfre.fre[i]);*/strcpy(topfre[i].w,p->w);/*printf("%s",topfre.w[i]);*/}
	}
   }
 a1:   sort(topfre,topfre+10,com); 
   
 } 
 
void wordgroup_fre_put() 
{
	 int i,j,m,max[10]={0},a[10]={0};
	 for(j=0;j<10;j++)
	 {
	 
	   for(i=0;wgtop[i].count;i++)
	  { for(m=0;m<j;m++) { if(!strcmp(wgtop[i].group,wgtopput[m].group)) {i++;m=-1;if(!wgtop[i].count) goto e1;} } 
	 	if(wgtop[i].count>wgtopput[j].count) {wgtopput[j]=wgtop[i];}
	  }
e1:	  ;
	 } 
	   
c1:	 sort(wgtopput,wgtopput+10,com2);
	 
}

int main()
{
   char dir[100]="f1.txt";
    FileSearch(dir); //printf("%d",filenumber);
       FP=fopen("result.txt","w+");   
	for(int i=0;i<=filenumber;i++) 
{
	if((fp=fopen(data[i].dir,"r+"))==NULL){
        printf("Open the file failure...\n");
        exit(0);}
     wordCount();}
      word_fre_put(); 
    wordgroup_fre_put(); 
	 showWord();
    fclose(fp);
}
