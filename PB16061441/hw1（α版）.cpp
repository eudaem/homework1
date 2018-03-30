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

#pragma warning(disable : 4996)
using namespace std; 


typedef struct word{
	int frequency;
	char content[WORDLEN];
	char after_deal[WORDLEN];
	word* next;
}word;

typedef struct wordgroup{
	int frequency;
	word *firstword, *secword;
	wordgroup *next;
}wordgroup;

word* Hashw[26][26][26][26]={NULL};      //word hashlist

wordgroup *Hashg[26][26][26][26]={NULL};  //wordgroup hashlist

word *preword=NULL; 




unsigned long int count_character=0;
unsigned long int count_word=0;
unsigned long int count_line=0;

void wordoperation(char* newword);         //counting the number of word
void wordjudge(char ch,char *wordstore);             //judge whether it's a word
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

int main()                 
{   
    FILE *fin,*fout;
	char dir[200],ch;
	char wordstore[WORDLEN]={0};                   //to store word for a moment, wordstore[0] is used to store length 
	
	printf("Enter a directory: ");
	cin.getline(dir, 200);
	vector<char*>allPath = getFilesList(dir);

	for (size_t i = 0; i < allPath.size(); i++)
	{
		char *perPath = allPath.at(i); //perpath stores the roots
		
		
        if((fin=fopen(perPath,"r"))==NULL){
        	printf("cannot open this file\n");
        	exit(0);
        }
        
	    
	    while((ch=fgetc(fin))!=EOF){
		    if(ch>=32&&ch<=126){         //counting the number of character
		        count_character++;  
	        }
	        if(ch=='\n'){
	        	count_line++;
	        } 
	        wordjudge(ch,wordstore);
	    } 
	    preword=NULL;
	    wordstore[0]=wordstore[1]='\0'; 
	    count_line++; 
	    fclose(fin);
	}
	printf("char_number: %lu\n",count_character);
	printf("word_number: %lu\n",count_word);
	printf("line_number: %lu\n",count_line);
	toptenw();
   // topteng();
	return 0;
}

void wordjudge(char ch,char *wordstore){
	char dest[WORDLEN]={0};
	int i;
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
	int k;
	word *p,*s;
	wordgroup *i,*l;
	char c,newwordc[WORDLEN];
	strcpy(newwordc,newword);
	k=strlen(newwordc);
	c=*(newwordc+k-1);
	for(;c>='0'&&c<='9';k--){
		newwordc[k]='\0';
		c=*(newwordc+k-1);
	}
    strlwr(newwordc);
    /*The following centences are to store word*/
	for(p=Hashw[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a'];p&&strcmp(p->after_deal,newwordc);p=p->next);
	if(!p){
		s=(word*)malloc(sizeof(word));
		s->frequency =1;
		strcpy(s->content,newword);
		strcpy(s->after_deal,newwordc);
		if(!Hashw[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']){
			Hashw[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']=s;
			s->next=NULL;
		}
		else{
			s->next=Hashw[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a'];
		    Hashw[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']=s;	
		}
			
	}
	else{
		p->frequency++;
		if(strcmp(p->content,newword)>0){
			strcpy(p->content ,newword);
		} 
		//puts(p->content );
	} 
	/*The following centences are to store word group*/
	if(preword){
		
		for(i=Hashg[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a'];
		i&&(strcmp(i->firstword->after_deal,preword->after_deal)||strcmp(i->secword->after_deal,newwordc));
		i=i->next);
		
	    if(!i){
	    	l=(wordgroup*)malloc(sizeof(wordgroup));
	    	l->frequency =1;
	    	l->firstword =preword;
	    	l->secword =p?p:s;
	    	if(!Hashg[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']){
			    Hashg[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']=l;
			    l->next=NULL;
		    }
		    else{
			    l->next=Hashg[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a'];
		        Hashg[*newwordc-'a'][*(newwordc+1)-'a'][*(newwordc+2)-'a'][*(newwordc+3)-'a']=l;	
		    } 
	    }
		else{
		    i->frequency++; 
		} 
	}
	preword=p?p:s;
	*/
}

void toptenw(void){
	word *tenw[10]={NULL},*s;
	int i,j,k,l,m,n;
	for(j=0;j<26;j++){
		for(k=0;k<26;k++){
			for(l=0;l<26;l++){
				for(i=0;i<27;i++){
					if(!Hashw[j][k][l][i])continue;
					else{
						for(s=Hashw[j][k][l][i];s;s=s->next){
							for(m=9;m>=0&&(!tenw[m]||s->frequency>tenw[m]->frequency);m--);
							if(m+1<=9){
								for(n=9;n>m+1;n--){
									tenw[n]=tenw[n-1];
								}
								tenw[m+1]=s;
								
						    }
						}
					}
				}
			}
		}
	}
    for(i=0;i<10;i++){
    	if(tenw[i])printf("%s: %d\n",tenw[i]->content,tenw[i]->frequency);
    }
}

void topteng(void){
	wordgroup *teng[10]={NULL},*s;
	int i,j,k,l,m,n;
	for(j=0;j<26;j++){
		for(k=0;k<26;k++){
			for(l=0;l<26;l++){
				for(i=0;i<26;i++){
					if(!Hashg[j][k][l][i])continue;
					else{
						for(s=Hashg[j][k][l][i];s;s=s->next){
							for(m=9;m>=0&&(!teng[m]||s->frequency>teng[m]->frequency);m--);
							if(m+1<=9){
								for(n=9;n>m+1;n--){
									teng[n]=teng[n-1];
								}
								teng[m+1]=s;
								
						    }
						}
					}
				}	
			}
		}
	}
    for(i=0;i<10;i++){
    	if(teng[i])printf("%s %s: %d\n",teng[i]->firstword->content,teng[i]->secword->content,teng[i]->frequency);
    }
}

vector<char*> getFilesList(const char * dir)
{
	vector<char*> allPath;
	char dirNew[200];
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");     

	intptr_t handle;

	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1) {
		strcpy(dirNew ,dir);
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
		if (findData.attrib & _A_SUBDIR) 
		{ 
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;


			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);
			vector<char*> tempPath = getFilesList(dirNew);
			allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());
		}
		else 
		{
			char *filePath = new char[200];
			strcpy(filePath, dir);
			strcat(filePath, "\\");
			strcat(filePath, findData.name);
			allPath.push_back(filePath);

		}
	} while (_findnext(handle, &findData) == 0);
	_findclose(handle);      
	return allPath;
}

