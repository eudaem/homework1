
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
#include <unordered_map>

using namespace std;
#ifdef WIN32
void Get_All_Files(string path, vector<string>& files) {
	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	files.push_back(path);
	hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo);
	if (hFile != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					Get_All_Files(p.assign(path).append("\\").append(fileinfo.name), files);
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
void Get_All_Files(string path, vector<string>& files)
{
	string name;
	DIR* dir = opendir(path.c_str());
	if (!dir) {
		return;
	}
	dirent* p = NULL;
	while ((p = readdir(dir)) != NULL)
	{
		if (p->d_name[0] != '.')
		{
			string name = path + "/" + string(p->d_name);
			files.push_back(name);
			//cout << name << endl;
			if (p->d_type == 4) {
				Get_All_Files(name, files);
			}
		}

	}
	closedir(dir);
	return;
}
#endif


int Read(string filename, int &words_num, int &line, int &char_num, unordered_map<string, int> &words_map, unordered_map<string, int> &phrase_map, unordered_map<string, string> &match_map) {
	fstream read_by_char;
	char c, b;	//c is current char, b is capital letter of c(if small letter)
	int count = 0, i = 0;
	string  word = "", word_min = "", phrase = "";
	read_by_char.open(filename, ios::in);
	if (read_by_char.fail()) {
		return 0; //maybe content
	}
	line++;
	while (read_by_char.get(c)) {
		if (c >= 31 && c < 127) {  //char count
			char_num++;
		}
		if (c == '\n') {	//line count
			line++;
		}


		if (word.size() > 1024) {//words limit
			word = "";
			count = 0;
		}


		if ((c > 47 && c < 58)) {//c is a number
			if (count < 4) {//not a standard word						
				count = 0;
				word = "";
				word_min = "";
			}
			else {//a standard word
				word += c;
				word_min += c;
				count++;
			}
		}
		else if (c > 64 && c < 91) {//c is capital letter
			word += c;
			word_min += c;
			count++;
		}
		else if (c > 96 && c < 123) {//c is small letter
			word += c;
			word_min += c - 32;
			count++;
		}
		else {//c is else 
			if (count >= 4) {
				words_num++;	//words count
				i = count - 1;
				while (word_min[i] < 58) {
					i--;
				}
				word_min.erase(word_min.begin() + i + 1, word_min.end());

				words_map[word_min]++;
				if (match_map.count(word_min)) {
					if (word < match_map[word_min]) {
						match_map[word_min] = word;
					}
				}
				else {
					match_map[word_min] = word;
				}
				if (phrase == "") {//phrase judgement
					phrase = word_min;
				}
				else {
					phrase = phrase + " " + word_min;
					phrase_map[phrase]++;
					phrase = word_min;
				}
			}
			count = 0;
			word = "";
			word_min = "";
		}
	}
	//last word judgement
	if (count >= 4) {
		words_num++;
		i = count - 1;
		while (word_min[i] < 58) {
			i--;
		}
		word_min.erase(word_min.begin() + i + 1, word_min.end());

		words_map[word_min]++;
		if (match_map.count(word_min)) {
			if (word < match_map[word_min]) {
				match_map[word_min] = word;
			}
		}
		else {
			match_map[word_min] = word;
		}
		if (phrase == "") {
			phrase = word_min;
		}
		else {
			phrase = phrase + " " + word_min;
			phrase_map[phrase]++;
			phrase = word_min;
		}
	}
	read_by_char.close();
	return 0;
}

int searchtopwords(string path, unordered_map<string, int> &words_map, unordered_map<string, string> &match_map) {
	int i = 8, j = 0;
	string topwords[10];
	int topwordsnum[10];
	unordered_map<string, int>::iterator it;
	ofstream  outpath(path, ios::app);

	for (i = 0; i < 10; i++) {
		topwords[i] = "";
		topwordsnum[i] = 0;
	}
	for (it = words_map.begin(); it != words_map.end(); it++) {
		if (it->second > topwordsnum[9]) {//should insert
			if (it->second <= topwordsnum[8]) {
				topwordsnum[9] = it->second;
				topwords[9] = match_map[it->first];
			}
			else {
				topwordsnum[9] = topwordsnum[8];
				topwords[9] = topwords[8];
				for (i = 7; i >= 0 && (it->second > topwordsnum[i]); i--) {
					topwordsnum[i + 1] = topwordsnum[i];
					topwords[i + 1] = topwords[i];
				}
				i++;
				topwordsnum[i] = it->second;
				topwords[i] = match_map[it->first];
			}
		}
	}
	outpath << "the top ten frequency of word : " << endl;
	for (i = 0; i < 10; i++) {
		if (topwords[i] == "") {
			break;
		}
		outpath << topwords[i] << "\t" << topwordsnum[i] << "\n" << endl;
	}
	outpath << endl << endl;
	outpath.close();
	return 0;
}

int searchtopphrases(string path, unordered_map<string, int> &phrase_map, unordered_map<string, string> &match_map) {
	int i = 8, j = 0, idx = 0, size = 0;
	string topphrase[10], word1, word2, phrase2;
	int topphrasenum[10];
	unordered_map<string, int>::iterator it;
	ofstream  outpath(path, ios::app);

	for (i = 0; i < 10; i++) {
		topphrase[i] = "";
		topphrasenum[i] = 0;
	}
	for (it = phrase_map.begin(); it != phrase_map.end(); it++) {
		if (it->second > topphrasenum[9]) {
			size = it->first.size();
			if (it->second <= topphrasenum[8]) {
				topphrasenum[9] = it->second;
				idx = it->first.find(" ");
				word1 = it->first.substr(0, idx);
				word2 = it->first.substr(idx + 1, size - idx - 1);
				topphrase[9] = match_map[word1] + " " + match_map[word2];
			}
			else {
				topphrasenum[9] = topphrasenum[8];
				topphrase[9] = topphrase[8];
				for (i = 7; i >= 0 && (it->second > topphrasenum[i]); i--) {
					topphrasenum[i + 1] = topphrasenum[i];
					topphrase[i + 1] = topphrase[i];
				}
				i++;
				topphrasenum[i] = it->second;
				idx = it->first.find(" ");
				word1 = it->first.substr(0, idx);
				word2 = it->first.substr(idx + 1, size - idx - 1);
				topphrase[i] = match_map[word1] + " " + match_map[word2];
			}
		}
	}

	outpath << "the top ten frequency of phrase : " << endl;
	for (i = 0; i < 10; i++) {
		if (topphrase[i] == "") {
			break;
		}
		outpath << topphrase[i] << "\t" << topphrasenum[i] << "\n" << endl;
	}
	outpath << endl << endl;
	outpath.close();

	return 0;
}

int main(int argc, char *argv[])
{

	string filepath = "";
	filepath = argv[1];
	vector<string> files;
	int if_file;
	Get_All_Files(filepath, files);
	int filesize = files.size();
	int all_char = 0; 
	int all_line = 0; 
	int single_char = 0;
	int single_line = 0;
	int all_words = 0;
	int single_words = 0;
	unordered_map<string, int> words_map, phrase_map;
	unordered_map<string, string> match_map;
	for (int i = 0; i < filesize; i++)
	{
		single_char = 0;
		single_line = 0;
		single_words = 0;
		Read(files[i], single_words, single_line, single_char, words_map, phrase_map, match_map);
		all_line += single_line;
		all_char += single_char;
		all_words += single_words;
	}


	string outpath =  "myresult.txt";
	ofstream  outfile(outpath, ios::app);
	outfile << "char_number: " << all_char << "\n"<<endl;
	outfile << "line_number: " << all_line << "\n" << endl;
	outfile << "word_number: " << all_words << "\n" << endl << endl << endl;
	searchtopwords(outpath, words_map, match_map);
	searchtopphrases(outpath, phrase_map, match_map);
	outfile.close();
	cout << "end!" << endl;
	return 0;
}
