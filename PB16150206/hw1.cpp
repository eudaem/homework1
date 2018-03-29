//please swithch the Unicode character set to MBCS(Multi-Byte Character System before compiling!!!!
//It has the function of phrase statistics, but it will crash if the file is too big,so I annotated it out.
//The problem lies in 410-431 lines and the bug is the kind I never seen and can't get solution on the Internet.

//linux header files
#ifdef __linux__
#include <dirent.h>
#endif
//windows header files
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include<io.h>
#include<map>
#include <vector>  
#include <algorithm>  
#include <utility> 
#include<Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
using  namespace std;

//result structure
typedef struct {
	long charaNum;
	long lineNum;
	long wordNum;
	map<string, int> dict;  //transformed
	map<string, int> nonedict;  //primary
	map<string, int> phrase;   //primary
//	map<string, int> transphrase;   //transformed
}fileProp;

//declarations
void fileRead(string p, fileProp *result);

void folderTraverse(string p, fileProp *result);
void DFS(string p, fileProp *result);

bool isSourceFile(string p);
bool isValidDir(string p);

long charaStat(string p);
long lineStat(string p);
bool isWord(char *ch);
long wordStat(string p, fileProp *result);
void phraseStat(string p,fileProp *result);
map<string, int> mapProc(map<string, int> dict, map<string, int> nonedict);

void resultPrint(fileProp *result);

#ifdef WIN32
void DFS(string p, fileProp *result) {
	_finddata_t file_info;
	string current_path = p + "/*.*";
	long handle = _findfirst(current_path.c_str(), &file_info);
	if (-1L == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}
	do {
		if (file_info.attrib & _A_SUBDIR) {
			if ((strcmp(file_info.name, "..") != 0) && (strcmp(file_info.name, ".") != 0))
				DFS(p + '/' + file_info.name, result);
		}
		else {
			cout << p << "\\" << file_info.name << endl;
			fileRead(p + "\\" + file_info.name, result);
		}
	} while (_findnext(handle, &file_info) == 0);
	_findclose(handle);
}
#endif

#ifdef __linux__
void DFS(string path, fileProp *result)
{
	string name;
	DIR* dir = opendir(path.c_str());
	dirent* p = NULL;
	while ((p = readdir(dir)) != NULL)
	{
		if (p->d_name[0] != '.')
		{
			string name = path + "/" + string(p->d_name);
			fileRead(name, result);
			if (p->d_type == 4) {
				DFS(name, result);
			}
		}

	}
	closedir(dir);

}
#endif

//main
int main(int argc,char *argv[])
{
	fileProp *result = new fileProp;
	result->charaNum = 0;
	result->lineNum = 0;
	result->wordNum = 0;
	string fileName;
	fstream fileEx;
	fileName = argv[1];
	fileEx.open(fileName, ios::in);
	if (isSourceFile(fileName)) {
		fileRead(fileName, result);
		resultPrint(result);
	}
	else if (isValidDir(fileName)) {
		folderTraverse(fileName, result);
		resultPrint(result);
	}
	else
		cout << "It's not a valid file!" << endl;
	return 0;
}

//statistic for different types of file
void fileRead(string p, fileProp *result) {
	result->charaNum += charaStat(p);
	result->lineNum += lineStat(p);
	result->wordNum += wordStat(p, result);
	phraseStat(p, result);
}


void folderTraverse(string p, fileProp *result) {
	DFS(p, result);
}


//judge the type of the file
#ifdef WIN32
bool isSourceFile(string p) {
	_finddata_t file_info;
	string current_path = p;
	LPCSTR ext;
	char name[256];
	int handle = _findfirst(current_path.c_str(), &file_info);
	if (file_info.attrib == _A_SUBDIR)
		return false;

	strcpy_s(name, p.c_str());
	ext = PathFindExtension(name);
	if (ext != ".jpg")
		return true;
	else
		return false;
}
#endif

#ifdef __linux__
bool isSourceFile(string p) {
	string name;
	DIR* dir = opendir(path.c_str());
	dirent* p = NULL;
	if (p->d_name[0] != '.')
		return true;
	else
		return false;
}
#endif

bool isValidDir(string p) {
	if (true == (bool)PathIsDirectory(p.c_str()))
		return true;
	else
		return false;
}


//statistic functions
long charaStat(string p) {
	long n = 0;
	char ch;
	ifstream infile(p);
	while (infile.get(ch)) {
		if ((ch >= 32) && (ch <= 126))
			n++;
	}return n;
}

long lineStat(string p) {
	char c;
	long n = 0;
	ifstream infile(p);
	while (infile.get(c)) {
		if (c == '\n')
			n++;
	}
	if (c != '\n')
		return n + 1;
	else
		return n;
}

long wordStat(string p, fileProp *result) {
	int i = 0;
	char c;
	char str[256] = { 0 };
	string word1, word2;
	ifstream infile(p);
	while (infile.get(c)) {
		if ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z'))
		{
			str[i] = c;
			i++;
		}
		else if (isWord(str)) {
			result->wordNum++;
			word1 = str;
			result->nonedict[word1] += 1;
			transform(word1.begin(),word1.end(),word1.begin(), ::tolower);
			result->dict[word1] += 1;  //word frequency
			for (i = 0; i < 256; i++)
				str[i] = 0;
			i = 0;
		}
		else {
			for (i = 0; i < 256; i++)
				str[i] = 0;
			i = 0;
		}
	}
	return 0;
}

map<string, int> mapProc(map<string, int> dict, map<string, int> nonedict) {
	string tmp;
	int cmp;
	int wordFreq;
	map<string, int>::iterator it;
	map<string, int>::iterator tmpit;
	for (it = nonedict.begin(); it != nonedict.end(); it++) {
		tmp = it->first;
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		wordFreq = dict.find(tmp)->second;
		it->second = wordFreq;
	}
	for (it = nonedict.begin(); it != nonedict.end(); it++) {
		tmpit = it;
		cmp = it->second;
		it++;
		for (it; it != nonedict.end(); it++) {
			if ((it->second) == cmp)
				nonedict.erase(it);
		}
		it = tmpit;
	}
	return nonedict;
}

void phraseStat(string p, fileProp *result) {
	int i = 0, j = 0;
	char c;
	ifstream infile(p);
	bool isPhrase;
	string phr1,phr2,phr;
	char wordA[1024] = { '\0' };
	char wordB[1024] = { '\0' };
	infile.get(c);
	while (!infile.eof()) {
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z')||(c>='0')&&(c<='9'))
		{
			if ((!infile.get(c)) ||
				((c < '0') || (c > '9'&&c < 'A')||(c>'Z'&&c<'a')||(c>'z')))
				break;
			wordA[i] = c;
			i++;
		}
		i = 0;
		infile.get(c);
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordB[j] = c;
			j++;
		}
		j = 0;
		infile.get(c);
		isPhrase = isWord(wordA) && isWord(wordB);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+ phr2;
			result->phrase[phr] += 1;
		//	transform(phr.begin(), phr.end(), phr.begin(), ::tolower);
		//	result->transphrase[phr] += 1;
		}
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordA[i] = c;
			i++;
		}
		i = 0;
		infile.get(c);
		isPhrase = isWord(wordA) && isWord(wordB);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+ phr2;
			result->phrase[phr] += 1;
		//	transform(phr.begin(), phr.end(), phr.begin(), ::tolower);
		//	result->transphrase[phr] += 1;
		}
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordB[j] = c;
			j++;
		}
		j = 0;
		infile.get(c);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+phr2;
			result->phrase[phr] += 1;
		//	transform(phr.begin(), phr.end(), phr.begin(), ::tolower);
		//	result->transphrase[phr] += 1;
		}
	}
	
}

bool isWord(char *ch) {
	if (
		((ch[0] >= 'A'&&ch[0] <= 'Z') || (ch[0] >= 'a'&&ch[0] <= 'z'))
		&& ((ch[1] >= 'A'&&ch[1] <= 'Z') || (ch[1] >= 'a'&&ch[1] <= 'z'))
		&& ((ch[2] >= 'A'&&ch[2] <= 'Z') || (ch[2] >= 'a'&&ch[2] <= 'z'))
		&& ((ch[3] >= 'A'&&ch[3] <= 'Z') || (ch[3] >= 'a'&&ch[3] <= 'z'))
		)
		return true;
	else
		return false;
}


//output function
bool cmp(const pair<string, int> &p1, const pair<string, int> &p2)
{
	if (p1.second > p2.second)
		return true;
	else
		return false;
}

void resultPrint(fileProp *result) {
	result->nonedict = mapProc(result->dict, result->nonedict);
//	result->phrase = mapProc(result->transphrase, result->phrase);
	ofstream outfile("result.txt");

	//print characters
	cout << "characters:" << result->charaNum << endl;
	outfile << "characters:" << " " << result->charaNum << endl;

	//print words
	cout << "word:" << result->wordNum << endl;
	outfile << "word:" << " " << result->wordNum << endl;

	//print lines
	cout << "line:" << result->lineNum << endl;
	outfile << "line:" << " " << result->lineNum << endl;

	cout << endl;
	outfile << endl;


	//print word frequency
	map<string, int>::iterator it1;
	vector<pair<string, int>> arr1;
	vector<pair<string, int>>::iterator ait1;
	for (it1 = result->nonedict.begin(); it1 != result->nonedict.end(); it1++)
		arr1.push_back(make_pair(it1->first, it1->second));
	sort(arr1.begin(), arr1.end(), cmp);
	if ((arr1.end() - arr1.begin()) >= 10) {
		for (ait1 = arr1.begin(); ait1 < (arr1.begin() + 10); ait1++) {
			cout << ait1->first << ": " << ait1->second << endl;
			outfile << ait1->first << ": " << ait1->second << endl;
		}
	}
	else {
		for (ait1 = arr1.begin(); ait1 < arr1.end(); ait1++) {
			cout << ait1->first << ": " << ait1->second << endl;
			outfile << ait1->first << ": " << ait1->second << endl;
		}
	}
	arr1.~vector();

	cout << endl;
	outfile << endl;

	/*
	//pring phrase frequency
	map<string, int>::iterator it2;
	vector<pair<string, int>> arr2;
	vector<pair<string, int>>::iterator ait2;
	for (it2 = result->phrase.begin(); it2 != result->phrase.end(); it2++)
		arr2.push_back(make_pair(it2->first, it2->second));
	sort(arr2.begin(), arr2.end(), cmp);
	if ((arr2.end() - arr2.begin()) >= 10) {
		for (ait2 = arr2.begin(); ait2 < (arr2.begin() + 10); ait2++) {
			cout << ait2->first << ": " << ait2->second << endl;
			outfile << ait2->first << ": " << ait2->second << endl;
		}
	}
	else {
		for (ait2 = arr2.begin(); ait2 < arr2.end(); ait2++) {
			cout << ait2->first << ": " << ait2->second << endl;
			outfile << ait2->first << ": " << ait2->second << endl;
		}
	}
	arr2.~vector();
	*/
}




