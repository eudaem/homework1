/* 
	WordFrequency.cpp 
	Author: Liu Ze
	Sno: PB15061305
	Mail: liuze@mail.ustc.edu.cn
	Time: 2018.3.29 15:20
*/
/*	About
		This code finish the task to count the number of characters, words, phrases and analysis them.
		You can see a more detailed report on my blog: [http://www.cnblogs.com/Franzkfk/]
		Or you can visit my github: [https://github.com/LiuzeGit]
	To Build
		Windows
			In Windows, make sure you have preprocessor macro: WIN32,and use C++11
		Linux
			In Linux, make sure you have preprocessor macro: __linux__,and use C++11 like this:
				g++ -std=c++11 WordFrequency.cpp
			My environment: Ubuntu 16.04LTS, g++ 5.40
	To Run
		Windows
			You can run like this:
			WordFrequency.exe input-path [output-path]
		Linux
			You can run like this:
			WordFrequency input-path [output-path]
*/

#ifdef __linux__ //used for Linux
	#include <dirent.h>
#endif
#ifdef WIN32	//used for Windows
	#include<io.h>
#endif

#include <iostream> 
#include <fstream> 
#include<time.h>

#include <string>  
#include <vector>
#include <map>
#include <unordered_map> 
#include <algorithm>

using namespace std;

#ifdef WIN32
void GetAllFiles(string path, vector<string>& files){	//get all file paths in Windows
	//input: path:the file or directory path
	//output: all files paths
	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	files.push_back(path); //store the current file path
	hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo); //judge whether the file can be open
	if (hFile  != -1){ 
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR)) { //judge whether the path is a subdirectory, if so then recursively process it 
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else { // if it's not a subdirectory,then store it
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
#endif

#ifdef __linux__
void GetAllFiles(string path, vector<string>& files)//get all file paths in Linux
{
	string name;
	DIR* dir = opendir(path.c_str());//Open directory 
    dirent* p = NULL;//pointer for ergodic
    while((p = readdir(dir)) != NULL)//start ergodic 
    {  
        //filter .. and . files
        if(p->d_name[0] != '.')//store filename  
        {  
            string name = path + "/" + string(p->d_name);  
            files.push_back(name);
			//cout << name << endl;
			if(p->d_type == 4){
				GetAllFiles(name, files);
			}
        }  
    }  
    closedir(dir);//close

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
	while (i >= 0) {	//remove the last numbers
		c = s[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			break;
		}
		i--;
	}
	if (i < 0 ) {
		s = "";
	}
	else {
		s.erase(s.begin() + i + 1, s.end());
		int l = s.size();
		for (int k = 0; k < l; k++) {//transform to lower
			s[k] = (s[k] >= 'a' && s[k] <= 'z') ? s[k] : (s[k] + 32);
		}
		//transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);	
	}
	return 1;
}

void display_words_map(string path, unordered_map<string, int> &words_map, unordered_map<string, string> &exp_map) {
	//display the TOP10 words and write to file
 	vector<string> keylist(10);
	vector<int> numlist(10);
	vector<int>::iterator least;
	unordered_map<string, int>::const_iterator map_it; //iterator to iterate the map
	ofstream  outpath(path, ios::app);

	int count = 0, index = 0;
	//find top10: by using a 10 dimensions array to store thr big10 words and its frequency;so we can find the TOP10 by only one traversal 
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

int OneFileCount(string s, int &nchar, int &nline, int &nword, unordered_map<string, int> &words_map, unordered_map<string, int> &phrase_map, unordered_map<string,string> &exp_map) {
	//Count the characters, words, phrases in one file
	char c;
	int i, l;
	int count;
	string st = "", sp = "", se = ""; 
	string Ss = "";
	// st means the current word string(original --HandleString()---> standard) ; 
	//se stores the original words; sp stores the previous standard string 
	// used to represent the phrase:Ss = sp + st;

	ifstream fin(s, ios::in);
	if (!fin) {//whether there is a file error
		return 0;
	}
	nline++;
	vector<char> buf(fin.seekg(0, ios::end).tellg());	//a buffer in the memory to store a file
	fin.seekg(0, ios::beg).read(&buf[0], static_cast<streamsize>(buf.size())); 

	l = buf.size();
	i = 0;
	count = 0; //used to detemine whether a word has more than 4 alphabet prefix
	while (i < l) {	//iterate the file
		//count characters
		c = buf[i];
		if (c >= ' ' && c <= '~') {
			nchar++;
		}
		else if (c == '\n') {
			nline++;
		}
		//count words and phrases
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
			
			if (!(c >= '0' && c <= '9')) {
				st.push_back(c);
				count++;
			}
			else if (count < 4) {
				st = "";
				count = 0;
			}
			else {
				st.push_back(c);
			}
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
					Ss = "";
					Ss.append(sp).append(" ").append(st);
					phrase_map[Ss]++;
				}
				sp = st;
			}
			st = "";
			count = 0;
		}
		i++;
	}

	//consider that the last char is number or alphabet
	if (!st.empty()) {
		se = st;
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
				Ss = "";
				Ss.append(sp).append(" ").append(st);
				phrase_map[Ss]++;
			}
			sp = st;
			st = "";
		}
	}
	fin.close();
	return 1;

}

int main(int argc, char *argv[])
{
	int size;
	int nchar = 0;
	int nline = 0;
	int nword = 0;
	vector<string> files;	//stores all the file path
	clock_t startTime, endTime; // used to estimate the running time
	unordered_map<string, int> words_map;	//hash table for: <key-value> --> <words-frequency>
	unordered_map<string, int> phrase_map;	//hash table for: <key-value> --> <phrase-frequency>
	unordered_map<string, string> exp_map;	//hash table for: <key-value> --> <words-little expression>

	string filepath = "";
	filepath = argv[1]; //get the input path
	string outpath;
	if (argc >= 3) { //if user enter the output path
		outpath = argv[2];
	}
	else {	//if not, then use the default path
		outpath = "Result.txt";
	}
	
	GetAllFiles(filepath, files);	//get all file paths
	size = files.size();
	startTime = clock();
	for (int i = 0; i < size; i++){	// handle onr file
		OneFileCount(files[i], nchar, nline, nword, words_map, phrase_map, exp_map);

	}
	// write to file the result
	ofstream  outfile(outpath, ios::app);
	outfile << "char_number: " << nchar << endl;
	outfile <<"line_number: " << nline << endl;
	outfile << "word_number: " << nword << endl << endl << endl;
	display_words_map(outpath, words_map,exp_map);
	display_phrase_map(outpath, phrase_map,exp_map);

	endTime = clock();
	outfile << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	outfile.close();
	return 0;
}
