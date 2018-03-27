// WordFrequency.cpp 
//Author: Liu Ze
//Mail: liuze@mail.ustc.edu.cn
//Time: 2018.3.25 16:51
//

#include<io.h>
#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>  
#include <map>
#include <unordered_map> 
#include <algorithm>
using namespace std;

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
	if (i < 0) {
		s = "";
	}
	else {
		s.erase(s.begin() + i + 1, s.end());
		transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
	}
	return 1;
}

void display_words_map(unordered_map<string, int> &words_map, unordered_map<string, string> &exp_map) {
	//display the TOP10 words
 	vector<string> keylist(10);
	vector<int> numlist(10);
	vector<int>::iterator least;

	int count = 0, index = 0;
	unordered_map<string, int>::const_iterator map_it;
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

	for (int i = 0; i < 10; i++) {
		cout << exp_map[keylist[i]] << ": " << numlist[i] << endl;
	}
}

void display_phrase_map(unordered_map<string, int> &phrase_map, unordered_map<string, string> &exp_map) {
	//display the TOP10 phrase
	vector<string> keylist(10);
	vector<int> numlist(10);
	vector<int>::iterator least;
	unordered_map<string, int>::const_iterator map_it;

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
	for (int i = 0; i < 10; i++) {
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
		cout << exp_map[key1] << " " << exp_map[key2]<< ": " << numlist[i] << endl;
	}
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
	if (openfile.fail()) {
		return 0;
	}
	nline += 1;
	while (openfile.get(c)) {
		//count characters
		if (c >= 32 && c <= 127) {
			nchar++;
		}
		if (c == '\n') {
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
				words_map[st]++;
				//whether there exits st before
				if (exp_map.count(st) == 1) {
					exp_map[st] = exp_map[st] < se ? exp_map[st] : se; //store the little string
				}
				else {
					exp_map[st] = se;
				}
				nword++;
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
			if (exp_map.count(st) == 1) {
				exp_map[st] = exp_map[st] < se ? exp_map[st] : se; //store the little string
			}
			else {
				exp_map[st] = se;
			}
			nword++;
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


int main()
{
	string filepath = "I:/现代软件工程/TestSet";
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
		//cout << i << endl;
	}

	cout << "nchar: " << nchar << endl;
	cout<<"nline: " << nline << endl;
	cout << "nwords: " << nword << endl;
	cout << "---------------words map------------------" << endl;
	display_words_map(words_map,exp_map);
	cout << "---------------phrase map------------------" << endl;
	display_phrase_map(phrase_map,exp_map);

	/*cout << "---------------words map------------------" << endl;
	Display_map(words_map);
	cout << "---------------exp map------------------" << endl;
	Display_map2(exp_map);
	cout << "---------------phrase map------------------" << endl;
	Display_map(phrase_map);*/

	cout << "ok" << endl;
	system("pause");
	return 0;
}
