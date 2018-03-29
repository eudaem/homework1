#include <iostream>  
#include <vector>  
#include <cstring>        
#include <dirent.h>       //new for linux!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include<ctype.h>

#define WORDLEN 50
#define WORDNUM 200000 
#define GROUPNUM 2000000

using namespace std; 

typedef struct word{
	int frequency;
	char content[WORDLEN];
	char after_deal[WORDLEN];
}word;

typedef struct wordgroup{
	int frequency;
	word *firstword, *secword;
}wordgroup;


FILE *fin, *fout;

word Hashw[WORDNUM];         //word hashlist

wordgroup Hashg[GROUPNUM];   //wordgroup hashlist

word *preword=NULL; 

char wordstore[WORDLEN];                   //to store word for a moment, wordstore[0] is used to store length 

unsigned long int count_character=0;
unsigned long int count_word=0;
unsigned long int count_line=0;

void wordoperation(char* newword);         //counting the number of word
void wordjudge(char ch);             //judge whether it's a word
void toptenw(void);
void topteng(void);
//vector<char*> getFilesList(const char * dir);
void listDir(char *path);             //for linux


 
char* strlwr(char *str)
 {
    if(str == NULL)
        return NULL;
         
    char *p = str;
    while (*p != '\0')
    {
        if(*p >= 'A' && *p <= 'Z')
            *p = (*p) + 0x20;
        p++;
    }
    return str;
}

/*
int main(void){
	FILE *fp;
	int ch;
	fp=fopen("test.txt","w");
	
	while((ch=fgetc(fp))!=EOF){
		if(ch>=32&&ch<=126){         //counting the number of character
		    fcount_character++;  
	    }
	}

	while((ch=getchar())!='\n'){
		if(ch>=32&&ch<=126){         //counting the number of character
		    count_character++;  
	    }
	    wordjudge(ch);
	}
    toptenw();
    topteng();
}*/ 

int main(int argc,char *argv[])                 
{   
	
	//char ch;

	if ((fout=fopen( "result.txt", "w"))==NULL) {
		printf( "cannot open the result file\n");
		exit(0);
	}
	
	//printf("Enter a directory: ");
	//strcpy_s(dir, argv[1]);
	//vector<char*>allPath = getFilesList(dir);
	listDir(argv[1]);
    /*
	for (size_t i = 0; i < allPath.size(); i++)
	{
		char *perPath = allPath.at(i); //perpath stores the roots
		
		//system("pause");
        
	}
	*/

	fprintf(fout,"char_number: %lu\n",count_character);
	fprintf(fout,"line_number: %lu\n", count_line);
	fprintf(fout,"word_number: %lu\n",count_word);
	fprintf(fout,"\nthe top ten frequency of word :\n");
	toptenw();
	fprintf(fout,"\nthe top ten frequency of word :\n");
    topteng();
	//system("pause");
	return 0;
}

void wordjudge(char ch){
	int i;
	char dest[WORDLEN]={0};
	if(ch>='A'&&ch<='Z'||ch>='a'&&ch<='z'||ch>='0'&&ch<='9'){          
		i=wordstore[0];
		if(i<WORDLEN-2){
			i=++wordstore[0]; 
		    wordstore[i]=ch;
		    wordstore[i+1]='\0';
		}
		else{
		//	printf("the word is too long!\n");
		}
	}
	else if(wordstore[0]) {
		for(i=1;i<=4&&wordstore[i]&&(wordstore[i]>='A'&&wordstore[i]<='Z'||wordstore[i]>='a'&&wordstore[i]<='z');i++);
		if(i==5){
			strncpy(dest,wordstore+1,strlen(wordstore)-1);
			dest[strlen(dest)]='\0';
			wordoperation(dest);
			count_word++;
		}
		wordstore[0]=0;
		wordstore[1]='\0';
	}
}

void wordoperation(char* newword){
	int i,t,j,k,h=0,g=0;
	char c,newwordc[WORDLEN];
	strcpy(newwordc,newword);
	t=(int)strlen(newword);
	c=*(newwordc+t-1);
	for(;c>='0'&&c<='9';t--){
		newwordc[t]='\0';
		c=*(newwordc+t-1);
	}
    strlwr(newwordc);
    /*The following centences are to store word*/
    j=strlen(newwordc);
	for(k=1;k<j&&k<10;k++){
		h=(newwordc[k-1]+(h*36))%WORDNUM;
	}
	while(Hashw[h].frequency&&strcmp(Hashw[h].after_deal,newwordc)){
		h=(h+1)%WORDNUM;
	}
	if(Hashw[h].frequency){
		Hashw[h].frequency ++;
		if(strncmp(Hashw[h].content ,newword,51)>0){
			strcpy(Hashw[h].content  ,newword);
		}
	}
	else{
		Hashw[h].frequency =1;
		strcpy(Hashw[h].content,newword);
		strcpy(Hashw[h].after_deal ,newwordc); 
	}
	/*The following centences are to store word group*/
	if(preword){
		for(i=0;i<4;i++){
			g=(g*36+preword->after_deal[i])%GROUPNUM;
		}
		for(i=0;i<4;i++){
			g=(g*36+newwordc[i])%GROUPNUM; 
		}
		while(Hashg[g].frequency && 
		(strncmp(Hashg[g].firstword->after_deal,preword->after_deal,51)||strncmp(Hashg[g].secword->after_deal,newwordc,51))){
			g=(g+1)%GROUPNUM;
		}
		if(Hashg[g].frequency){
			Hashg[g].frequency ++;
		}
		else{
			Hashg[g].frequency =1;
			Hashg[g].firstword =preword;
			Hashg[g].secword =&Hashw[h];
		}
	
	}
	preword=&Hashw[h];
}

void toptenw(void){
	word tenw[10]={0};
	int j,m,n;
	for(j=0;j<WORDNUM;j++){
		if(Hashw[j].frequency ){
			for(m=9;m>=0&&(!tenw[m].frequency||Hashw[j].frequency>tenw[m].frequency);m--);
			if(m+1<=9){
				for(n=9;n>m+1;n--){
					tenw[n]=tenw[n-1];
				}
				tenw[m+1]=Hashw[j];
			}
		}
	}
    for(j=0;j<10;j++){
    	if(tenw[j].frequency)fprintf(fout,"%s: %d\n",tenw[j].content,tenw[j].frequency);
    }
}

void topteng(void){
	wordgroup teng[10]={0};
	long int j,m,n;
	for(j=0;j<GROUPNUM;j++){
		if(Hashg[j].frequency ){
			for(m=9;m>=0&&(!teng[m].frequency||Hashg[j].frequency>teng[m].frequency);m--);
			if(m+1<=9){
				for(n=9;n>m+1;n--){
					teng[n]=teng[n-1];
				}
				teng[m+1]=Hashg[j];
			}
		}
    }
    for(j=0;j<10;j++){
    	if(teng[j].frequency)fprintf(fout,"%s %s: %d\n",teng[j].firstword->content,teng[j].secword->content,teng[j].frequency);
    }
}

void listDir(char *path){
	DIR      *pDir=NULL;
	struct dirent  *ent=NULL;
	int       i=0;
	char      childpath[512],ch;
	
	pDir=opendir(path);
	memset(childpath,0,sizeof(childpath));
	
	while((ent=readdir(pDir))!=NULL){
		if(ent->d_type & DT_DIR){
			if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0)continue;
			sprintf(childpath,"%s%s/",path,ent->d_name);
			listDir(childpath);
		}
		else{
			sprintf(childpath,"%s%s",path,ent->d_name);
		        
			puts(childpath);

			if((fin=fopen(childpath,"r"))==NULL){
        	    fprintf(fout,"cannot open this file\n");
        	    exit(0);
            }
        
	    
	        while((ch=fgetc(fin))!=EOF){
		        if(ch>=32&&ch<=126){         //counting the number of character
		            count_character++;  
	            }
	            if(ch=='\n'){
	            	count_line++;
	            } 
	            wordjudge(ch);
	        } 
		
	        preword=NULL;
	        wordstore[0]=wordstore[1]='\0'; 
	        count_line++; 
	        fclose(fin);
		}
	}
}

