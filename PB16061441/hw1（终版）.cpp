#include <iostream>  
#include <vector>  
#include <cstring>        
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>

#define WORDLEN 50
#define WORDNUM 2000000 
#define GROUPNUM 20000000

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
vector<char*> getFilesList(const char * dir);

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
	
	char dir[200],ch;

	if (fopen_s(&fout, "result.txt", "w") != NULL) {
		fprintf(fout, "cannot open the result file\n");
		exit(0);
	}
	
	//printf("Enter a directory: ");
	strcpy_s(dir, argv[1]);
	vector<char*>allPath = getFilesList(dir);

	for (size_t i = 0; i < allPath.size(); i++)
	{
		char *perPath = allPath.at(i); //perpath stores the roots
		
		//system("pause");
        if(fopen_s(&fin,perPath,"r")!=NULL){
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
			strncpy_s(dest,wordstore+1,strlen(wordstore)-1);
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
	strcpy_s(newwordc,newword);
	t=(int)strlen(newword);
	c=*(newwordc+t-1);
	for(;c>='0'&&c<='9';t--){
		newwordc[t]='\0';
		c=*(newwordc+t-1);
	}
    _strlwr_s(newwordc);
    /*The following centences are to store word*/
    j=(int)strlen(newwordc);
	for(k=1;k<j&&k<10;k++){
		h=(newwordc[k-1]+(h*36))%WORDNUM;
	}
	while(Hashw[h].frequency&&strcmp(Hashw[h].after_deal,newwordc)){
		h=(h+1)%WORDNUM;
	}
	if(Hashw[h].frequency){
		Hashw[h].frequency ++;
		if(strcmp(Hashw[h].content ,newword)>0){
			strcpy_s(Hashw[h].content  ,newword);
		}
	}
	else{
		Hashw[h].frequency =1;
		strcpy_s(Hashw[h].content,newword);
		strcpy_s(Hashw[h].after_deal ,newwordc); 
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
		(strcmp(Hashg[g].firstword->after_deal,preword->after_deal)||strcmp(Hashg[g].secword->after_deal,newwordc))){
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
	word tenw[10]={NULL};
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


vector<char*> getFilesList(const char * dir)
{
	vector<char*> allPath;
	char dirNew[200];
	strcpy_s(dirNew, dir);
	strcat_s(dirNew, "\\*.*");      

	intptr_t handle;

	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1) {  
		strcpy_s(dirNew ,dir);
		handle = _findfirst(dirNew, &findData);
		if (-1 == handle)
		{
			cout << "can not found the file ... " << endl;
			return allPath;
		}
		else
		{
			allPath.push_back(const_cast <char*>(dir));
			return  allPath;
		}
	}

	do
	{
		if (findData.attrib & _A_SUBDIR)// 
		{
			//  
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;


			//cout << findData.name << "\t<dir>\n";  
			  
			strcpy_s(dirNew, dir);
			strcat_s(dirNew, "\\");
			strcat_s(dirNew, findData.name);
			vector<char*> tempPath = getFilesList(dirNew);
			allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());
		}
		else   
		{
			char *filePath = new char[200];
			strcpy_s(filePath, 200,dir);
			strcat_s(filePath, 200,"\\");
			strcat_s(filePath, 200, findData.name);
			allPath.push_back(filePath);
			  

		}
	} while (_findnext(handle, &findData) == 0);
	_findclose(handle);   
	return allPath;
}

