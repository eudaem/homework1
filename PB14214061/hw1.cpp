#include <fstream>  
#include <string>  
#include <vector>  
#include <iostream>  
#include<unordered_map>
#include <valarray>  
#include <time.h>
#include<iterator>
#include<bitset>
#include<string.h>
using namespace std;
string topworld[10];
int wnumtop[10] = { 0 };
int allch = 0; 
int allline = 0; 
string worddtostr(unordered_map<string, bitset<20>> &wordd, string s) {//根据位信息将单词改为大写待更改！！！！！！！！！！！！！！！！！！！！！！
	bitset<20> q = wordd[s];
	int f = s.length();
	for (int j = 0; j < f; j++)
	{
		s[j] = s[j] - 32 * q.test(j);
	}
	return s;
}
#ifdef _WIN32
#include <io.h>
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
#include <dirent.h>
#include <sys/stat.h>
void GetAllFiles(string path, vector<string>& files)
{
	string name;
	DIR* dir = opendir(path.c_str());
	dirent* p = NULL;
	while ((p = readdir(dir)) != NULL)
	{
		if (p->d_name[0] != '.')
		{
			string name = path + "/" + string(p->d_name);
			files.push_back(name);

			if (p->d_type == 4) {
				GetAllFiles(name, files);
			}
		}

	}
	closedir(dir);

}
#endif

int ReadByChar(string file, int &fch, int &line, unordered_map<string, int> &word, unordered_map<string, int> &phrase, unordered_map<string, string> &wordend)
{
	fstream openbychar;
	char c;
	int flag = 0;
	openbychar.open(file, ios::in);
	string sword = ""; 
	string bsword = ""; 
	string sphrase = "";
	string num = "";
	unordered_map<string, string>::iterator it;
	if (openbychar.fail()) {
		return 0; 
	}
	line = 1;
	while (openbychar.get(c))
	{
		if (c >= 32 && c < 127) fch++;
		if (c == '\n') {
			line++;
		}
		if ((c < 91 && c>64) || (c < 123 && c>96)) { 
			flag++;
			bsword.push_back(c);
			if (c < 91 && c>64) 
			{
				c = c + 32;
				sword.push_back(c);
			}
			else sword.push_back(c); 
		}
		else if (c > 47 && c < 58) { 
			if (flag > 3) {
				num.push_back(c);
				bsword.push_back(c);
				sword.push_back(c);
				flag++;
			}
			else {
				flag = 0;
				sword = "";
				bsword = "";
				num = "";
			}
		}
		else {   
			if (flag > 3) {
				if (sword[flag - 1] < 123 && sword[flag - 1]>96) 
				{
					word[sword]++;
					string s = wordend[sword];
					if (wordend[sword] == "") { 
						wordend[sword] = bsword;
					}
					else {
						wordend[sword] = s.compare(bsword) > 0 ? bsword : s;
					}
					sphrase.push_back(' ');
					sphrase.append(sword);
					phrase[sphrase]++;
					sphrase = sword;
				}
				else { 
					sword = sword.substr(0, strlen(sword.c_str()) - strlen(num.c_str()));
					word[sword]++;
					string s = wordend[sword];
					if (wordend[sword] == "") { 
						wordend[sword] = bsword;
					}
					else {
						wordend[sword] = s.compare(bsword) > 0 ? bsword : s;
					}
					sphrase.push_back(' ');
					sphrase.append(sword);
					phrase[sphrase]++;
					sphrase = sword;
				}
				sword = "";
				bsword = "";
				num = "";
				flag = 0;
			}
			else {
				flag = 0;
				sword = "";
				bsword = "";
				num = "";
			}
		}
	}
	if (sword.length() > 3) { 
		if (sword[flag - 1] < 123 && sword[flag - 1]>96) 
		{
			word[sword]++;
			string s = wordend[sword];
			if (wordend[sword] == "") { 
				wordend[sword] = bsword;
			}
			else {
				wordend[sword] = s.compare(bsword) > 0 ? bsword : s;
			}
			sphrase.push_back(' ');
			sphrase.append(sword);
			phrase[sphrase]++;
			sphrase = sword;
		}
		else { 
			sword = sword.substr(0, strlen(sword.c_str()) - strlen(num.c_str())); 
			word[sword]++;
			string s = wordend[sword];
			if (wordend[sword] == "") { 
				wordend[sword] = bsword;
			}
			else {
				wordend[sword] = s.compare(bsword) > 0 ? bsword : s;
			}
	
			sphrase.push_back(' ');
			sphrase.append(sword);
			phrase[sphrase]++;
			sphrase = sword;
		}
	}
	openbychar.close();
	if (fch == 0) line = 0;
	return 0;
}

void SplitString(string& s, vector<std::string>& v)
{
	const std::string& c = " ";
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

int  getwmin() { 
	int minnum = 2147483647;
	int min = 0;
	for (int i = 0; i < 10; i++) {
		if (wnumtop[i] < minnum) {
			minnum = wnumtop[i];
			min = i;
		}
	}
	return min;
}

int getmax() {
	int maxnum = 0;
	int max = 0;
	for (int i = 0; i < 10; i++) {
		if (wnumtop[i] > maxnum) {
			maxnum = wnumtop[i];
			max = i;
		}
	}
	return max;
}

void top(unordered_map<string, int> & word) {

	unordered_map<string, int>::iterator it = word.begin();
	unordered_map<string, int>::iterator end = word.end();
	int min = getwmin();
	for (; it != end; it++) {
		if (it->second > wnumtop[min]) {


			topworld[min] = it->first;
			wnumtop[min] = it->second;
			min = getwmin();
		}
	}

}

void writewordtxt(string filepath, unordered_map<string, string> wordend) { 
	fstream writebychar;
	int max = 0;
	writebychar.open(filepath, ios::app);
	for (int i = 0; i < 10; i++)
	{
		max = getmax();
		if (wnumtop[max] != 0) {
			writebychar << (wordend[topworld[max]]);
			writebychar << " ";
			writebychar << wnumtop[max] << "\n";
		}
		wnumtop[max] = 0;
	}
	writebychar << "\n\n";
	writebychar.close();
}

void writeworddtxt(string filepath, unordered_map<string, string> &wordend) { 
	fstream writebychar;
	int max = 0;
	writebychar.open(filepath, ios::app);
	for (int i = 0; i < 10; i++)
	{
		max = getmax();
		vector<string> s;
		SplitString(topworld[max], s);
		if (s[0] != ""&&wnumtop[max] != 0) {
			writebychar << wordend[s[0]] << " " << wordend[s[1]];
			writebychar << " ";
			writebychar << wnumtop[max] << "\n";
		}
		wnumtop[max] = 0;
	}
	writebychar << "\n\n";
	writebychar.close();
}

void writecharnum(string path, unordered_map<string, int> word) {
	fstream write;
	int max = 0;
	unordered_map<string, int>::iterator it = word.begin();
	unordered_map<string, int>::iterator end = word.end();
	int wordnum = 0;
	for (; it != end; it++) {
		wordnum = wordnum + it->second;
	}
	write.open(path, ios::out);
	write << "char_number : ";
	write << allch << "\n";
	write << "line_number : ";
	write << allline << "\n";
	write << "word_number:";
	write << wordnum << "\n \n";;
	write.close();
}

int main(int argc, char *argv[])
{
	unordered_map<string, int> word; 
	unordered_map<string, int> phrase; 
	unordered_map<string, string> wordend; 
	vector<string> files;
	string writepath = "";
	string filepath = "";

	string s = argv[1];
	writepath = "result.txt";
	if (s.find(".") == string::npos) {
		filepath = s;
		GetAllFiles(filepath, files);
		int size = files.size();
		for (int i = 0; i < size; i++)
		{
			int fch = 0;
			int line = 0;
			ReadByChar(files[i], fch, line, word, phrase, wordend);
			allch += fch;
			allline += line;
		}
	}
	else {
		ReadByChar(s, allch, allline, word, phrase, wordend);
	}
	writecharnum(writepath, word);
	if (allch != 0) {
		top(word);
		writewordtxt(writepath, wordend);
		top(phrase); 
		writeworddtxt(writepath, wordend);
	}
	return 0;
}
