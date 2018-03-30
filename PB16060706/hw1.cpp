#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<io.h>
#include"targetver.h"
#include<fstream>
#include<vector>
#include<iostream>
#include<tchar.h>
#include<numeric>
#include<functional>
#include<unordered_map>
using namespace std;
int  char_num = 0, line_num = 1, word_num = 0;
#define lnode struct node
struct node {
	int num;
	char word[200];
	struct node* next;
};

void GetAllFiles(string path, vector<string>& files,int &char_num,int &line_num,int &word_num)
{
	FILE *f1 =NULL;
	int count = 0, i = 0;
	long  handle = 0;  // File Handle
	struct _finddata_t fileinfo;  // File Information
	string p;
	char * location;  // location converts char * into string
	char code;
	if ((handle = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))  // to check whether it is a folder or a file
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files, char_num, line_num, word_num);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				location = (char *)p.data();
				f1 = fopen(location, "r");
				if (f1) {
					while ((code = fgetc(f1)) != EOF){
						if (code >= 32 && code <= 126) char_num++;
						if (code == '\n')
							line_num++;
						if ((code >= 65 && code <= 90) || (code >= 97 && code <= 122)){
							count++;
						}

						else {
							if (count >= 4){
								word_num++;
								count = 0;
							}
							else count = 0;
						}

					}
				}

			}
		} while (_findnext(handle, &fileinfo) == 0);
		_findclose(handle);
	}
}
int main(){
	
	FILE  *f2;
	char * DATA_DIR = __argv[1], *s1 = "characters:",*s2="lines:",*s3="words:";
	vector < string > files;
	GetAllFiles(DATA_DIR, files, char_num, line_num, word_num);
	//printf("%d %d %d\n", char_num, line_num, word_num);
	f2 = fopen("result.txt", "w");
	if (f2) {
		fputs(s1, f2);
		fprintf(f2,"%d",char_num);
		fputc('\n', f2);
		fputs(s2, f2);
		fprintf(f2, "%d", line_num);
		fputc('\n', f2);
		fputs(s3, f2);
		fprintf(f2, "%d", word_num);
	}
	system("pause");
	return 0;
}
