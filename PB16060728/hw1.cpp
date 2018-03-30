#include <iostream>
#include <string>
#include <fstream>
//#include <io.h>
#include<ctype.h>
#include <algorithm>
#include <unordered_map>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
using namespace std;
class word_time {
public:
	string word;
	int time;
public:
	word_time() {
		this->word = "";
		this->time = 0;
	}
};
class word_word_time : public word_time {
public:
	string word_s;
	word_word_time() {
		this->time = 0;
		this->word = "";
		this->word_s = "";
	}
};
class bi_word {
public:
	string str1;
	string str2;
	bi_word() {
		this->str1 = "";
		this->str2 = "";
	}
};
int charactercount = 0, wordcount = 0, linecount = 0;
unordered_map<string, word_time> word_list;
unordered_map<string, word_word_time> bi_word_list;
bool is_letter(char m) {
	if (m >= 65 && m <= 90 || m >= 97 && m <= 122)
		return true;
	return false;
}
bool is_depart(char m) {
	if (m >= 65 && m <= 90 || m >= 97 && m <= 122 || m >= 48 && m <= 57)
		return false;
	return true;
}
void add_a_word(string s) {
	string temp = s;
	string::iterator iter;
	iter = s.end();
	iter--;
	while (!is_letter(*iter)) {
		iter--;
	};
	s.erase(iter + 1, s.end());                    
	transform(s.begin(), s.end(), s.begin(), ::toupper);            
	word_list[s].time++;                                                
	if (word_list[s].word == "" || word_list[s].word.compare(temp)>0) {
		word_list[s].word = temp;
	}

}
void add_words(bi_word b_word) {
	bi_word b_word_ = b_word;
	string::iterator it1, it2;
	it1 = b_word.str1.end();
	it2 = b_word.str2.end();
	it1--; it2--;
	while (!is_letter(*it1)) {
		it1--;
	};
	while (!is_letter(*it2)) {
		it2--;
	};
	b_word.str1.erase(it1 + 1, b_word.str1.end());                                        
	b_word.str2.erase(it2 + 1, b_word.str2.end());
	string temp = b_word.str1 + b_word.str2;
	transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
	bi_word_list[temp].time++;
	if (bi_word_list[temp].word == "" || bi_word_list[temp].word.compare(b_word_.str1)>0)
		bi_word_list[temp].word = b_word_.str1;
	if (bi_word_list[temp].word_s == "" || bi_word_list[temp].word_s.compare(b_word_.str2)>0)
		bi_word_list[temp].word_s = b_word_.str2;
}

string str_temp, str_now;
void count_insert(string &line) {
	bi_word temp1;

	int i = 0, k;
	bool flag = false;
	line.append(" ");
	for (; is_depart(line[i]) && (size_t)i < line.length(); i++);
	for (int j = i; line[j] != '\0' && (size_t)j < line.length(); j++) {
		if (is_depart(line[j])) {

			for (k = i; k - i < 4 && (size_t)k< line.length(); k++) {
				if (!is_letter(line[i])) {
					flag = true;
					break;
				}
			}
			if (flag == false) {
				str_temp = line.substr(i, j - i);
				if (str_temp.size() >= 4) {
					add_a_word(str_temp);
					wordcount++;
					if (str_now != "") {
						temp1.str1 = str_now;
						temp1.str2 = str_temp;
						add_words(temp1);

					}
					str_now = str_temp;
				}

			}
			flag = false;
			i = j + 1;

		}

	}
}
vector<word_time> the_most_ten() {
	vector<word_time> most_ten(10);
	unordered_map<string, word_time>::iterator it = word_list.begin();
	while (it != word_list.end()) {
		if (it->second.time > most_ten[9].time) {
			if (it->second.time > most_ten[0].time)
				most_ten.insert(most_ten.begin(), it->second);
			else
				for (int i = 1; i <= 9; i++) {
					if (it->second.time > most_ten[i].time && it->second.time <= most_ten[i - 1].time) {
						most_ten.insert(most_ten.begin() + i, it->second);
						break;
					}
				}
		}
		it++;
	}
	most_ten.erase(most_ten.begin() + 10, most_ten.end());
	return most_ten;
}
vector<word_word_time> the_most_ten_bi() {
	vector<word_word_time> most_ten_bi(10);
	unordered_map<string, word_word_time>::iterator it = bi_word_list.begin();
	while (it != bi_word_list.end()) {
		if (it->second.time > most_ten_bi[9].time) {
			if (it->second.time > most_ten_bi[0].time)
				most_ten_bi.insert(most_ten_bi.begin(), it->second);

			else
				for (int i = 1; i <= 9; i++) {
					if (it->second.time > most_ten_bi[i].time && it->second.time <= most_ten_bi[i - 1].time) {
						most_ten_bi.insert(most_ten_bi.begin() + i, it->second);
						break;
					}
				}
		}
		it++;
	}
	most_ten_bi.erase(most_ten_bi.begin() + 10, most_ten_bi.end());
	return most_ten_bi;
}
void listDir(char *path)
{
	DIR *pDir; 
	struct dirent *ent; 
	int i = 0;
	char childpath[512]; 
	pDir = opendir(path);
	memset(childpath, 0, sizeof(childpath)); 
	while ((ent = readdir(pDir)) != NULL){
		if (ent->d_type & DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;
			sprintf(childpath, "%s/%s", path, ent->d_name);
			listDir(childpath);
		}
		else
		{
			sprintf(childpath, "%s/%s", path, ent->d_name);
			ifstream infile;
			infile.open(childpath, ios::in);
            string d;
			while (getline(infile, d)) {
				for (int i = 0; (size_t)i <= d.size(); i++) {
					if (d[i] >= 32 && d[i] <= 126)
						charactercount++;

				}
				linecount++;
				count_insert(d);
			}
		}
	}
		linecount += 1;
		charactercount--;
}
int main(int argc, char *argv[]) {
	clock_t tStart = clock();
	listDir(argv[1]);
	ofstream out;
	out.open("Result.txt", ios::out);
	out << "char_number:" << charactercount << " " << endl;
	out << "line_number:" << linecount << " " << endl;
	out << "word_number:" << wordcount << " " << endl;
	vector<word_time> a = the_most_ten();
	vector<word_time>::iterator iter;
	out << "the top ten frequency of word:" << endl;
	for (iter = a.begin(); iter != a.end(); iter++)
	{
		out << iter->word << " " << iter->time << endl;

	}
	vector<word_word_time> b = the_most_ten_bi();
	vector<word_word_time>::iterator ite;
	out << "the top ten frequence of phrase:" << endl;
	for (ite = b.begin(); ite != b.end(); ite++)
	{
		out << ite->word << " " << ite->word_s << " " << ite->time << endl;

	}
	out.close();
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	return 0;
}



