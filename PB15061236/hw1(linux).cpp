//#define Win32
#define __linux__
#ifdef __linux__
#include <dirent.h>
#endif
#ifdef Win32
#include <io.h>
#endif
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <time.h>
using namespace std;

#define outputnumb 10
#define output //result out put to file
//#define coutput //result out put to screen

#ifdef Win32
void dfsFolder(string folderPath, int search);
#endif
#ifdef __linux__
void listDir(string path);
#endif
void countall(string filepath); //count char word and line
void insertmap(string last_word, string current_word);
void bubblesortword(void);
void bubblesortphrase(void);
//count the number of ASCII and word,use map

struct wordandtimes
{
	string mostwordoccur = "bbbb";
	size_t mostwordtimes = 0;
} mostword[outputnumb + 1];
struct phraseandtimes
{
	string mostphraseoccur = "bbb";
	size_t mostphrasetimes = 0;
} mostphrase[outputnumb + 1];

unordered_map<string, wordandtimes>word_count;
unordered_map<string, phraseandtimes>phrase_count;
long long asccount = 0;
long long wordscount = 0;
long long linescount = 0;

int main(int argc,char* argv[])
{
	clock_t tStart = clock();
#ifdef output
	if (argc < 1) {
		printf("fatal error: no such file or directory\n");
		return -1;
	}
	string path = argv[1];
	//path = "D:\\TestingSamples\\newsample";
#ifdef Win32
	dfsFolder(path, 0);
#endif
#ifdef __linux__
	listDir(path);
#endif
	FILE * outfile = nullptr;
	errno_t err;
	err = fopen_s(&outfile, "result.txt", "w");
#endif
	//dfsFolder("D:\\TestingSamples\\test", 0);
#ifdef output
	fprintf(outfile, "char_number : %ld\n", asccount);
	fprintf(outfile, "line_number : %ld\n", linescount);
	fprintf(outfile, "word_number : %ld\n", wordscount);
#endif
#ifdef coutput
	cout << "char_number :" << asccount << endl;
	cout << "line_number :" << linescount << endl;
	cout << "word_number :" << wordscount << endl;
#endif
	bubblesortword();
#ifdef output
	fprintf(outfile, "\n the top ten frequency of word : \n");
	for (int index = 0; index < outputnumb; index++)
	{
		fprintf(outfile, "%s : %ld\n", mostword[index].mostwordoccur, mostword[index].mostwordtimes);
	}
#endif
#ifdef coutput
	cout << endl << "the top ten frequency of word : " << endl;
	for (int index = 0; index < outputnumb; index++)
	{
		cout << mostword[index].mostwordoccur << " : " << mostword[index].mostwordtimes << endl;
	}
#endif
	bubblesortphrase();
#ifdef output
	fprintf(outfile, "\n the top ten frequency of phrase : \n");
	for (int index = 0; index < outputnumb; index++)
	{//output correctly
		string phrase_now_temp = mostphrase[index].mostphraseoccur;
		string temp1, temp2;
		size_t len = phrase_now_temp.length();
		int k;
		for (k = 0; k < len; k++) {
			if (' ' == phrase_now_temp[k])break;

		}
		temp1 = phrase_now_temp.substr(0, k);
		temp2 = phrase_now_temp.substr(k + 1, len - k - 1);
		fprintf(outfile, "%s %s : %ld\n", temp1,temp2, mostphrase[index].mostphrasetimes);
	}
#endif
#ifdef coutput
	cout << endl << "the top ten frequency of phrase : " << endl;
	for (int index = 0; index < outputnumb; index++)
	{
		cout << mostphrase[index].mostphraseoccur << " : " << mostphrase[index].mostphrasetimes << endl;
	}
#endif
#ifdef output
	fclose(outfile);
	outfile = nullptr;
#endif
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;
}

#ifdef Win32
void dfsFolder(string folderPath, int search)
{
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	intptr_t Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (-1 == Handle)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do {
		//is a file?
		if (FileInfo.attrib == _A_SUBDIR)
		{
			int temp = search;
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath, search + 1);
			}
		}
		else
		{
			string filename = FileInfo.name;
			string suffixStr = filename.substr(filename.find_last_of('.') + 1);//Get the file suffix
			countall(folderPath + '/' + FileInfo.name);
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle);
}
#endif;

#ifdef __linux__
void listDir(string path)  
{
	DIR *dir = opendir(path.c_str());
	dirent *info;
	if (dir) {
		string dir_path = file + "/*.*";
		while ((info = readdir(dir)) != NULL) {
			if ((info->d_name[0] == '.') &&
				((info->d_name[1] == 0) || (info->d_name[1] == '.' && info->d_name[2] == 0)))
				continue;
			listDir(string().assign(file).append("/").append(info->d_name));
		}
		closedir(dir);
	}
	else {
		countall(path);
	}
}
#endif

void countall(string filepath)
{
	int countasc = 0;
	int countword = 0;
	int countline = 0;
	char tempchar;
	char lastchar = ' ';
	bool flag = false;
	FILE * fp;
	errno_t err;
	string fileline;
	string lastword;
	size_t location;
	err = fopen_s(&fp , filepath.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0L, SEEK_END);
		location = ftell(fp);
		rewind(fp); //read file from beginning
		char* buff;
		buff = (char*)malloc(location * sizeof(char));
		size_t leng = fread(buff, sizeof(char), location, fp);
		if (leng)
		{
			countline++;
		}
		for (int index = 0; index < leng; index++)
		{
			tempchar = buff[index];
			if (tempchar < -1 || tempchar > 255)
			{
				tempchar = ' ';
			}
			if ((!isalnum(lastchar)) && (isalpha(tempchar)))
			{
				flag = true;
				fileline = tempchar;
			}
			else if (flag)
			{
				if (isalnum(tempchar))
				{
					fileline = fileline + tempchar;
				}
				else
				{
					flag = false;
					if ((isalpha(fileline[0])) && (isalpha(fileline[1])) && (isalpha(fileline[2]) && (isalpha(fileline[3]))))
					{
						countword++;
						insertmap(lastword, fileline);
						lastword = fileline;
						fileline.clear();
					}
				}
			}
			if (tempchar == '\n')
			{
				countline++;
			}
			lastchar = tempchar;
		}
		free(buff);

		asccount += location;
		wordscount += countword;
		linescount += countline;
		fclose(fp);
		fp = nullptr;
	}
	else
		cout << "read fail" << endl;
	return;
}
void insertmap(string last_word, string current_word)
{//insert two word to wordmap,and insert a phrase to phrasemap
	string simple_last_word;
	string simple_current_word;
	size_t len = last_word.length();
	string temp_word = last_word;
	transform(temp_word.begin(), temp_word.end(), temp_word.begin(), ::tolower);
	bool is_start = false;
	for (size_t i = len - 1; i >= 0; i--)
	{//simplify the word
		if (isalpha(temp_word[i]))
		{
			is_start = true;
			simple_last_word = temp_word.substr(0, i + 1);
			break;
		}
	}
	len = current_word.length();
	temp_word = current_word;
	transform(temp_word.begin(), temp_word.end(), temp_word.begin(), ::tolower);
	is_start = false;
	for (size_t i = len - 1; i >= 0; i--)
	{//simplify the word
		if (isalpha(temp_word[i]))
		{
			is_start = true;
			simple_current_word = temp_word.substr(0, i + 1);
			break;
		}
	}
	unordered_map<string, wordandtimes> ::iterator getword = word_count.find(simple_current_word);
	if (getword == word_count.end())
	{
		word_count.insert({ simple_current_word,{ current_word,1 } });
	}
	else
	{
		getword->second.mostwordtimes++;
		if (current_word < getword->second.mostwordoccur)
		{
			getword->second.mostwordoccur = current_word;
		}
	}
	string simpleword = getword->second.mostwordoccur;
	unordered_map<string, wordandtimes> ::iterator getwordlast = word_count.find(simple_last_word);
	string simplewordlast;
	if (getwordlast == word_count.end())
	{
		simplewordlast = last_word;
	}
	else
	{
		simplewordlast = getwordlast->second.mostwordoccur;
	}
	string simple_phrase = simple_last_word + ' ' + simple_current_word;
	string raw_phrase = simplewordlast + ' ' + simpleword;
	unordered_map<string, phraseandtimes> ::iterator getphrase = phrase_count.find(simple_phrase);
	if (getphrase == phrase_count.end())
	{
		phrase_count.insert({ simple_phrase,{ raw_phrase,1 } });
	}
	else
	{
		getphrase->second.mostphrasetimes++;
		if (raw_phrase < getphrase->second.mostphraseoccur)
		{
			getphrase->second.mostphraseoccur = raw_phrase;
		}
	}
}
void bubblesortword(void)
{
	wordandtimes tempword;
	for (const auto &w : word_count)
	{
		if (w.second.mostwordtimes >= mostword[outputnumb - 1].mostwordtimes)
		{
			mostword[outputnumb] = w.second;
			for (int index = outputnumb - 1; index >= 0; index--)
			{
				if (mostword[index].mostwordtimes < mostword[index + 1].mostwordtimes)
				{
					tempword = mostword[index];
					mostword[index] = mostword[index + 1];
					mostword[index + 1] = tempword;
				}
				else if (mostword[index].mostwordtimes == mostword[index + 1].mostwordtimes)
				{
					if (mostword[index].mostwordoccur > mostword[index + 1].mostwordoccur)
					{
						tempword = mostword[index];
						mostword[index] = mostword[index + 1];
						mostword[index + 1] = tempword;
					}
				}
				else break;
			}
		}
	}
}
void bubblesortphrase(void)
{
	phraseandtimes tempphrase;
	for (const auto &w : phrase_count)
	{
		if (w.second.mostphrasetimes >= mostphrase[outputnumb - 1].mostphrasetimes)
		{
			mostphrase[outputnumb] = w.second;
			for (int index = outputnumb - 1; index >= 0; index--)
			{
				if (mostphrase[index].mostphrasetimes < mostphrase[index + 1].mostphrasetimes)
				{
					tempphrase = mostphrase[index];
					mostphrase[index] = mostphrase[index + 1];
					mostphrase[index + 1] = tempphrase;
				}
				else if (mostphrase[index].mostphrasetimes == mostphrase[index + 1].mostphrasetimes)
				{
					if (mostphrase[index].mostphraseoccur > mostphrase[index + 1].mostphraseoccur)
					{
						tempphrase = mostphrase[index];
						mostphrase[index] = mostphrase[index + 1];
						mostphrase[index + 1] = tempphrase;
					}
				}
				else break;
			}
		}
	}
}
//_CRT_SECURE_NO_WARNINGS
