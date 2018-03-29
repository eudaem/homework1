
#include<stdio.h>
#include<io.h>
#include<string.h>
#include<iostream>
#include<stdlib.h>

using namespace std;

#define eq ==
#define asciim 126
#define MAXHASH 500000   

int cizu[10000000][4];

int hashadd(string word);
int compareword(string word1, string word2);
int Select();
int Selectcizu();
int dfsFolder(string folderPath);

typedef struct word_part
{
	string word = "";
	int word_num = 0;
	int flag = 0;
}word_part;

word_part hashat[MAXHASH];     

int count_characters = 0;
int count_lines = 0;
int count_words = 0;

int dfsFolder(string folderPath)
{
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	string temp;
	int len;

	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		printf("can not match the folder path\n");
		return 0;
	}
	do {
		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath);
			}
		}
		else
		{
			len = strlen(FileInfo.name);
			if (FileInfo.name[len - 1] != 'g' || FileInfo.name[len - 2] != 'p' || FileInfo.name[len - 3] != 'j')
			{
				temp = folderPath + "\\" + FileInfo.name;
				//printf("%s\n", (char*)temp.data());

				FILE *pFile;
				char *buffer;

				long lsize;
				size_t result;
				errno_t err;

				err = fopen_s(&pFile, (char*)temp.data(), "rb");
				count_lines++;
				if (err != 0)
				{
					printf("File error\n");
					exit(0);
				}

				fseek(pFile, 0, SEEK_END);
				lsize = ftell(pFile);
				rewind(pFile);

				buffer = (char*)malloc(sizeof(char)*lsize);
				if (buffer eq NULL)
				{
					printf("Memory error\n");
					exit(1);
				}

				result = fread(buffer, 1, lsize, pFile);
				if (result != lsize)
				{
					printf("Reading error\n");
					exit(2);
				}
				if (buffer eq NULL) return 0;

				int i = 0;

				char *p = buffer;
				int word_len;
				string word_now;

				while (*p != '\0')
				{
					if (*p eq '\n')
					{
						count_lines++;
						p++;
						if (*p >= 32 && *p <= asciim) count_characters++;
					}
					else if (*p >= 32 && *p <= asciim)                  //get one letter after one loop
					{
						word_len = 0;
						word_now = "";

						while (!((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z'))) //when *p is not letter,then only count it and skip 
						{
							p++;
							if (*p == '\0')break;
							if (*p eq '\n')	break;
							if (*p >= 32 && *p <= asciim) count_characters++;
						}

						if (*(p - 1) >= '0'&&*(p - 1) <= '9')    //123a is not word
						{
							while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
							{
								p++;
								if (*p >= 32 && *p <= asciim) count_characters++;
							}
							continue;
						}

						while (((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z')) && word_len<4)
						{
							word_now.push_back(*p);
							word_len++;
							p++;
							if (*p >= 32 && *p <= asciim) count_characters++;
						}

						if (word_len >= 4)         //find a word successfully
						{
							while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
							{
								word_now.push_back(*p);
								p++;
								if (*p >= 32 && *p <= asciim) count_characters++;
							}
							count_words++;
							hashadd(word_now);
						}
						else                        //the situation that asd123
						{
							while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
							{
								p++;
								if (*p >= 32 && *p <= asciim) count_characters++;
							}
						}
					}
					else
					{
						p++;
						if (*p >= 32 && *p <= asciim) count_characters++;
					}
				}
				if (*(p - 1) eq '\n') count_lines--;
				fclose(pFile);
				free(buffer);
			}
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);
	return 0;
}

FILE *fp;
int openfile(string temp)
{
	FILE *pFile;
	char *buffer;

	long lsize;
	size_t result;
	errno_t err;

	err = fopen_s(&pFile, (char*)temp.data(), "rb");
	count_lines++;
	if (err != 0)
	{
		printf("File error\n");
		exit(0);
	}

	fseek(pFile, 0, SEEK_END);
	lsize = ftell(pFile);
	rewind(pFile);

	buffer = (char*)malloc(sizeof(char)*lsize);
	if (buffer eq NULL)
	{
		printf("Memory error\n");
		exit(1);
	}

	result = fread(buffer, 1, lsize, pFile);
	if (result != lsize)
	{
		printf("Reading error\n");
		exit(2);
	}
	if (buffer eq NULL) return 0;

	int i = 0;

	char *p = buffer;
	int word_len;
	string word_now;

	while (*p != '\0')
	{
		if (*p eq '\n')
		{
			count_lines++;
			p++;
			if (*p >= 32 && *p <= asciim) count_characters++;
		}
		else if (*p >= 32 && *p <= asciim)                  //get one letter after one loop
		{
			word_len = 0;
			word_now = "";

			while (!((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z'))) //when *p is not letter,then only count it and skip 
			{
				p++;
				if (*p == '\0')break;
				if (*p eq '\n')	break;
				if (*p >= 32 && *p <= asciim) count_characters++;
			}

			if (*(p - 1) >= '0'&&*(p - 1) <= '9')    //123a is not word
			{
				while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
				{
					p++;
					if (*p >= 32 && *p <= asciim) count_characters++;
				}
				continue;
			}

			while (((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z')) && word_len<4)
			{
				word_now.push_back(*p);
				word_len++;
				p++;
				if (*p >= 32 && *p <= asciim) count_characters++;
			}

			if (word_len >= 4)         //find a word successfully
			{
				while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
				{
					word_now.push_back(*p);
					p++;
					if (*p >= 32 && *p <= asciim) count_characters++;
				}
				count_words++;
				hashadd(word_now);
			}
			else                        //the situation that asd123
			{
				while ((*p >= 'a'&&*p <= 'z') || (*p >= 'A'&&*p <= 'Z') || (*p >= '0'&&*p <= '9'))
				{
					p++;
					if (*p >= 32 && *p <= asciim) count_characters++;
				}
			}
		}
		else
		{
			p++;
			if (*p >= 32 && *p <= asciim) count_characters++;
		}
	}
	if (*(p - 1) eq '\n') count_lines--;
	fclose(pFile);
	free(buffer);
}

int main(int argc, char* argv[])
{
	string File = argv[1];
	//string File = "C:\\Users\\Geng Zigang\\Desktop\\newsample";

	//fopen_s(&fp, "C:\\Users\\Geng Zigang\\Desktop\\tx2t.txt", "w");
	fopen_s(&fp, "result", "w");
	_finddata_t FileInfo1;
	string strfind1 = File + "\\*";

	long Handle1 = _findfirst(strfind1.c_str(), &FileInfo1);

	if (Handle1 == -1L)
	{
		openfile(File);
	}
	else 
		dfsFolder(File);

	Select();
	fprintf(fp, "-----------\n");
	Selectcizu();

	fprintf(fp, "word is %d\ncharacter is%d \nline is%d\n", count_words, count_characters, count_lines);
	printf("success\n");
	fclose(fp);
	_findclose(Handle1);
	return 0;
}

int Select() {
	int i, j = 0, max, maxi;
	while (j < 10) {
		max = 0;
		maxi = 0;
		for (i = 0; i < MAXHASH; i++)
		{
			if (hashat[i].word_num > max && hashat[i].flag == 0)
			{
				max = hashat[i].word_num;
				maxi = i;
			}
		}
		if (max != 0)
		{
			hashat[maxi].flag = 1;
			fprintf(fp, "%d：%s，its number is %d\n", j + 1, (char*)hashat[maxi].word.data(), hashat[maxi].word_num);
		}
		else fprintf(fp, "%d does not exist\n", j + 1);
		j++;
	}
	return 0;
}

int Selectcizu() {
	int i, j = 0, max, maxi;
	while (j < 10) {
		max = 0;
		maxi = 0;
		for (i = 0; i < MAXHASH * 20; i++)
		{
			if (cizu[i][2] > max && cizu[i][3] == 0)
			{
				max = cizu[i][2];
				maxi = i;
			}
		}
		if (max != 0)
		{
			cizu[maxi][3] = 1;
			fprintf(fp, "%d : %s %s,its number is %d\n", j + 1, (char*)hashat[cizu[maxi][0]].word.data(), (char*)hashat[cizu[maxi][1]].word.data(), max);
		}
		else fprintf(fp, "%d does not exist\n", j + 1);
		j++;
	}
	return 0;
}

int before = -1;
int before1 = 0;
int hashadd(string word)
{
	int hashnum = 0;
	int flag = 0;
	int flag1 = 0;
	int offset;

	int i = 0, con = 0;
	while (word[i] != '\0'&& con <= 8)
	{
		if (word[i] >= 'A' && word[i] <= 'Z')
		{
			hashnum += (word[i] - 'A') * 7000 / (con + 1);
			con++;
		}
		else if (word[i] >= 'a' && word[i] <= 'z')
		{
			hashnum += (word[i] - 'a') * 7000 / (con + 1);
			con++;
		}
		i++;
	}
	string aaa = "that";
	/*flag1 = compareword(word, aaa);
	if (flag1 != 3) fprintf(fp, "%s\n", (char*)word.data());*/
loop:
	if (hashat[hashnum].word_num eq 0)
	{
		hashat[hashnum].word = word;
		hashat[hashnum].word_num = 0;
		hashat[hashnum].word_num = 1;
	}
	else
	{
		flag = compareword(word, hashat[hashnum].word);
		if (flag eq 1 || flag eq - 1)
		{
			if (flag eq - 1)
			{
				hashat[hashnum].word = word;
				hashat[hashnum].word_num++;
			}
			else
			{
				hashat[hashnum].word_num++;
			}
		}
		else if (flag eq 3)
		{
			hashnum++;
			goto loop;
		}
	}

	if (before == -1) {}
	else
	{
		offset = 0;
		while (cizu[before + offset][2] != 0)
		{
			if (cizu[before + offset][1] == hashnum && cizu[before + offset][0] == before1)
			{
				cizu[before + offset][2]++;
				break;
			}
			else offset++;
		}
		if (cizu[before + offset][2] == 0)
		{
			cizu[before + offset][0] = before1;
			cizu[before + offset][1] = hashnum;
			cizu[before + offset][2] = 1;
			cizu[before + offset][3] = 0;
		}
	}

	before1 = hashnum;
	before = hashnum * 20;
	return 0;
}


int compareword(string word1, string word2)
{
	int flag;
	int i, j;
	int big_flag;

	i = word1.length() - 1;
	j = word2.length() - 1;

	while (i >= 0)
	{
		if (word1[i] >= '0'&&word1[i] <= '9') word1[i] = '\0';
		else break;
		i--;
	}
	while (j >= 0)
	{
		if (word2[j] >= '0'&&word2[j] <= '9') word2[j] = '\0';
		else break;
		j--;
	}
	flag = _strcmpi((char*)word1.data(), (char *)word2.data());
	if (flag != 0) return 3;
	else {
		big_flag = strcmp((char *)word1.data(), (char *)word2.data());
		if (big_flag == 0) big_flag++;
		return big_flag;
	}
}
