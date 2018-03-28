// WordFrequency.cpp 
//Author: Liu Ze
//Mail: liuze@mail.ustc.edu.cn
//Time: 2018.3.28 11:07
//
#ifdef __linux__
#include <dirent.h>
#endif
#ifdef WIN32
#include<io.h>
#endif

#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>  
#include <map>
#include <unordered_map> 
#include <algorithm>
using namespace std;

#ifdef WIN32
void GetAllFiles(string path, vector<string>& files)
{

	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}

}
#endif

#ifdef __linux__
void GetAllFiles(string path, vector<string>& files)
{
	string name;
	DIR* dir = opendir(path.c_str());//打开指定目录  
    dirent* p = NULL;//定义遍历指针  
    while((p = readdir(dir)) != NULL)//开始逐个遍历  
    {  
        //这里需要注意，linux平台下一个目录中有"."和".."隐藏文件，需要过滤掉  
        if(p->d_name[0] != '.')//d_name是一个char数组，存放当前遍历到的文件名  
        {  
            string name = path + "/" + string(p->d_name);  
            files.push_back(name);
			//cout << name << endl;
			if(p->d_type == 4){
				GetAllFiles(name, files);
			}
        }  
    }  
    closedir(dir);//关闭指定目录  

}
#endif

int HandleString(string &s) {
	//hanlde the string to standard form
	//1. remove the last numbers
	//2. transform to lower
	if (s.empty()) {
		return 0;
	}

	int i = s.size() - 1;
	char c;
	while (i >= 0) {
		c = s[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			break;
		}
		i--;
	}
	int j = 0, k = 0;
	int count = 0;
	while (j <= i) {
		k = j;
		count = 0;
		while (k <= i) {
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
				count++;
				k++;
			}
		}
		if (count >= 4) {
			break;
		}
		j = k + 1;
	}
	if (i < 0 || count < 4) {
		s = "";
	}
	else {
		s.erase(s.begin() + i + 1, s.end());
		s.erase(0, j);
		transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
	}
	return 1;
}

void display_words_map(string path, unordered_map<string, int> &words_map, unordered_map<string, string> &exp_map) {
	//display the TOP10 words
 	vector<string> keylist(10);
	vector<int> numlist(10);
	vector<int>::iterator least;
	unordered_map<string, int>::const_iterator map_it;
	ofstream  outpath(path, ios::app);

	int count = 0, index = 0;
	//find top10
	for (map_it = words_map.begin(); map_it != words_map.end(); map_it++) {
		if (count < 10) {
			keylist[index] = map_it->first;
			numlist[index] = map_it->second;
			index++;
			count++;
		}
		else {
			least = min_element(numlist.begin(), numlist.end());
			index = distance(numlist.begin(), least);
			if (map_it->second > numlist[index]) {
				keylist[index] = map_it->first;
				numlist[index] = map_it->second;
			}
		}
	}
	outpath << "the top ten frequency of word : " << endl;
	//sort by number
	int i, j = 0;
	int temp;
	string Stemp;
	int max;
	int Order[10] = { -1 };
	for (i = 0; i < 10; i++) {
		max = i;
		for (j = i + 1; j < 10; j++) {
			max = numlist[max] > numlist[j] ? max : j;
		}
		temp = numlist[i];
		numlist[i] = numlist[max];
		numlist[max] = temp;
		Stemp = keylist[i];
		keylist[i] = keylist[max];
		keylist[max] = Stemp;
	}
	//Write to file
	for (i = 0; i < 10; i++) {
		outpath << exp_map[keylist[i]] << ": " << numlist[i] << endl;
	}
	outpath << endl << endl;
	outpath.close();
}

void display_phrase_map(string path, unordered_map<string, int> &phrase_map, unordered_map<string, string> &exp_map) {
	//display the TOP10 phrase
	vector<string> keylist(10);
	vector<int> numlist(10);
	vector<int>::iterator least;
	unordered_map<string, int>::const_iterator map_it;
	ofstream  outpath(path, ios::app);

	string key1, key2, key;
	char c;
	int len, j;
	int count = 0, index = 0;
	
	//find top ten
	for (map_it = phrase_map.begin(); map_it != phrase_map.end(); map_it++) {
		if (count < 10) {
			keylist[index] = map_it->first;
			numlist[index] = map_it->second;
			index++;
			count++;
		}
		else {
			least = min_element(numlist.begin(), numlist.end());
			index = distance(numlist.begin(), least);
			if (map_it->second > numlist[index]) {
				keylist[index] = map_it->first;
				numlist[index] = map_it->second;
			}
		}
	}
	//sort by number
	int i;
	int temp;
	string Stemp;
	int max;
	int Order[10] = { -1 };
	for (i = 0; i < 10; i++) {
		max = i;
		for (j = i + 1; j < 10; j++) {
			max = numlist[max] > numlist[j] ? max : j;
		}
		temp = numlist[i];
		numlist[i] = numlist[max];
		numlist[max] = temp;
		Stemp = keylist[i];
		keylist[i] = keylist[max];
		keylist[max] = Stemp;
	}
	//Write to file
	outpath << "the top ten frequency of phrase :" << endl;
	for (i = 0; i < 10; i++) {
		key1 = key2 = "";
		key = keylist[i];
		len = key.size();
		j = 0;
		c = key[0];
		while (c != ' ') {
			key1 += c;
			j++;
			c = key[j];
		}
		j++;
		while (j < len) {
			c = key[j];
			key2 += c;
			j++;	
		}
		outpath << exp_map[key1] << " " << exp_map[key2]<< ": " << numlist[i] << endl;
	}
	outpath << endl << endl;
	outpath.close();
}

void Display_map(unordered_map<string, int> &wmap) {
	//print the words map and phrase map
	unordered_map<string, int>::const_iterator map_it;
	for (map_it = wmap.begin(); map_it != wmap.end(); map_it++) {
		cout << "(\"" << map_it->first << "\"," << map_it->second << ")" << endl;
	}
}

void Display_map2(unordered_map<string, string> &wmap) {
	//print the expression map 
	unordered_map<string, string>::const_iterator map_it;
	for (map_it = wmap.begin(); map_it != wmap.end(); map_it++) {
		cout << "(\"" << map_it->first << "\"," << map_it->second << ")" << endl;
	}
}

int OneFileCount(string s, int &nchar, int &nline, int &nword, unordered_map<string, int> &words_map, unordered_map<string, int> &phrase_map, unordered_map<string,string> &exp_map) {
	fstream openfile;
	char c;
	
	string st = "", sp = "", se = "";
	string Ss = "";
	openfile.open(s, ios::in);
	if (openfile.fail()) {//whether there is a file error
		return 0;
	}
	nline++;
	while (openfile.get(c)) {
		//count characters
		if (c >= 32 && c <= 126) {
			nchar++;
		}
		else if (c == '\n') {
			nline++;
		}
		//count words and phrases
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
			st += c;
		}
		else {
			se = st;
			HandleString(st);
			if (st.length() >= 4) {
				nword++;
				words_map[st]++;
				//whether there exits st before
				if (exp_map.count(st) == 1) {
					exp_map[st] = exp_map[st] < se ? exp_map[st] : se; //store the little string
				}
				else {
					exp_map[st] = se;
				}
				if (!sp.empty()) {
					Ss = sp + ' ' + st;
					phrase_map[Ss]++;
				}
				sp = st;
			}
			st = "";
		}
	}

	//consider that the last char is number or alphabet
	if (!st.empty()) {
		HandleString(st);
		if (st.size() >= 4) {
			words_map[st]++;
			nword++;
			if (exp_map.count(st) == 1) {
				exp_map[st] = exp_map[st] < se ? exp_map[st] : se; //store the little string
			}
			else {
				exp_map[st] = se;
			}
			if (!sp.empty()) {
				Ss = sp + ' ' + st;
				phrase_map[Ss]++;
			}
			sp = st;
			st = "";
		}
	}
	openfile.close();
	return 1;

}

int main(int argc, char *argv[])
{

	string filepath = "";
	filepath = argv[1];

	string outpath = filepath + "/myresult.txt";
	ofstream  outfile(outpath, ios::app);
	vector<string> files;
	GetAllFiles(filepath, files);
	int size = files.size();
	int nchar = 0;
	int nline = 0;
	int nword = 0;
	unordered_map<string, int> words_map;
	unordered_map<string, int> phrase_map;
	unordered_map<string, string> exp_map;

	for (int i = 0; i < size; i++){
		OneFileCount(files[i], nchar, nline, nword, words_map, phrase_map, exp_map);
	}

	outfile << "char_number: " << nchar << endl;
	outfile <<"line_number: " << nline << endl;
	outfile << "word_number: " << nword << endl << endl << endl;
	display_words_map(outpath, words_map,exp_map);
	display_phrase_map(outpath, phrase_map,exp_map);

	outfile.close();
	cout << "ok" << endl;
	return 0;
}
