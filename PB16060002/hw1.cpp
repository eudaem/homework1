#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<cstring>
#include<iterator>
#include<cctype>
#include<unordered_map>
#include<vector>
#include<utility>
#include <Windows.h> 
#include <string.h> 
#include <io.h>
#include <stdlib.h>
#include <stdexcept>

using namespace std;


class Dictionary {
public:
	string strfile; 
	int lines = 0;  
	int strsize = strfile.size(); 
	unordered_map<string, int> strMap;
	int file_num = 0; 
	vector<string> filename; 
	string path = "E:/newsample"; 
	vector<string> hfwords; 
	unordered_map<string, int> dstrMap; 
	vector<string> hfwordgroup;  
	vector<string> words;  
	int words_num = 0;

	void Initwords(vector<string> &words) {
		int i = 0;
		for (i = 0;i < 100;i++) {
			words.push_back("");
		}
	}

	void Readfile(string &strfile, string filename)
	{
		ifstream ReadFile;
		string tmp;
		string suffixStr = filename.substr(filename.find_last_of('.') + 1);
		//if(!(suffixStr== ".txt"|| suffixStr == ".cpp"|| suffixStr == ".hpp" || suffixStr == ".h" || suffixStr == ".cs"|| suffixStr == ".html"|| suffixStr == ".js"|| suffixStr == ".java"|| suffixStr == ".py"|| suffixStr == ".php")) return;
		ReadFile.open(filename, ios::in);
		while (getline(ReadFile, tmp,' ')) {
			tmp += ' ';
			strfile += tmp;
			tmp.clear();
		}
		ReadFile.close();
	}

	int Countlines(string filename, int &lines)
	{
		ifstream ReadFile;
		int n = 0;
		string temp;
		ReadFile.open(filename, ios::in);
		if (ReadFile.fail())
		{
			return 0;
		}
		else  
		{
			while (getline(ReadFile, temp))
			{
				n++;
			}
			lines += n;
		}
		ReadFile.close();
		return 1;
	}

	void Selectwords(string strfile, vector<string> &words, unordered_map<string, int> &strMap, int &words_num)
	{
		unsigned int i = 0, j = 0, tmp = 0, t = 0; 
		string tmpstr; 
		for (i = 0;i < strfile.size();i++) {
			if ((strfile[i] >= 'a'&&strfile[i] <= 'z') || (strfile[i] >= 'A'&&strfile[i] <= 'Z') || (strfile[i] >= '0'&&strfile[i] <= '9')) {
				if (((strfile[i] >= 'a'&&strfile[i] <= 'z') || (strfile[i] >= 'A'&&strfile[i] <= 'Z')) && ((strfile[i - 1] < '0') || (strfile[i - 1] >'9'&&strfile[i - 1] < 'A') || (strfile[i - 1] > 'Z'&&strfile[i - 1] < 'a') || (strfile[i - 1] > 'z')) || i == 0) {
					for (tmp = 0;tmp < 4;tmp++) {
						if (!((strfile[i + tmp] >= 'a'&&strfile[i + tmp] <= 'z') || (strfile[i + tmp] >= 'A'&&strfile[i + tmp] <= 'Z'))) {
							if (tmpstr != "") {
								j = j % 50;
								words[j] = tmpstr;
								Countwords(tmpstr, strMap);
								Countwordgroup(words[j],words[j+1],strMap);
								j++;
								break;
							}
						}
						else {
							tmpstr[tmp] = strfile[i + tmp];
							t = tmp;
							i++;
						}
					}
				}
				else {
					tmpstr[++t] = strfile[i];
				}
			}
			else {
				if (tmpstr != "") {
					j = j % 50;
					words[j] = tmpstr;
					Countwords(words[j], strMap);
					Countwordgroup(words[j], words[j + 1], strMap);
					j++;
				}
			}
		}
	}

	void Upws(string &word)  
	{
		unsigned int i;
		for (i = 0;i < word.length();i++) {
			if (word[i] >= 'a'&&word[i] <= 'z') {
				word[i] = word[i] - 32;
			}
		}
	}

	void Countwords(string words, unordered_map<string, int> &strMap) {
		int t;
		unordered_map<string, int>::iterator strMap_Iter;
		Upws(words);  
		if (!(strMap.count(words))) {  
			strMap.insert(pair<string, int>(words, 1));
		}
		else {
			t = strMap.at(words);
			t++;
			strMap.at(words) = t;
		}
	}

	void Countwordgroup(string words,string words2, unordered_map<string, int> &dstrMap) {
		int t;
		string tmpdstr;
		unordered_map<string, int>::iterator dstrMap_Iter;
		Upws(words); 
		Upws(words2);
		tmpdstr = words + " " + words2;
		if (!((dstrMap.count(tmpdstr)) && words2!="")) {  
			dstrMap.insert(pair<string, int>(tmpdstr , 1));
		}
		else {
		    t = dstrMap.at(tmpdstr); 
			t++;
			dstrMap.at(tmpdstr) = t;
		}
	}

	bool TraverseFiles(string path, int &file_num)
	{
		_finddata_t file_info;
		string current_path = path + "/*.*"; 
		int handle = _findfirst(current_path.c_str(), &file_info);
		if (-1 == handle)
			return false;
		do
		{
			string attribute;
			if (file_info.attrib == _A_SUBDIR) 
				attribute = "dir";
			else
				attribute = "file";
			file_num++;
		} while (!_findnext(handle, &file_info));  
		_findclose(handle);
		return true;
	}

	void DfsFolder(string path, int layer, vector<string> &filename) 
	{
		_finddata_t file_info;
		string current_path = path + "/*.*"; 
		string copy_path;
		int handle = _findfirst(current_path.c_str(), &file_info);
		if (-1 == handle)
		{
			cout << "cannot match the path" << endl;
			return;
		}

		do
		{
			if (file_info.attrib == _A_SUBDIR)
			{
				int layer_tmp = layer;
				if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  
					DfsFolder(path + '/' + file_info.name, layer_tmp + 1, filename); 
			}
			else
			{
				copy_path = path + "/" + file_info.name;
				filename.push_back(copy_path);
			}
		} while (!_findnext(handle, &file_info));  										  
		_findclose(handle);                       
	}

	void Hfwords(vector<string> &hfwords, unordered_map<string, int> strMap)  
	{
		int high = 100000000, i;
		string tmp_max, tmp_secondmax, tmp;
		unordered_map<string, int>::iterator iter;
		iter = strMap.begin();
		tmp_secondmax = iter->first;
		for (i = 0; i < 10; i++)
		{
			for (iter = strMap.begin();iter != strMap.end();iter++)
			{
				tmp_max = iter->first;
				if (strMap[tmp_max] < high && strMap[tmp_max] > strMap[tmp_secondmax]) {
					tmp_max = tmp_max;
				}
				else {
					if (strMap[tmp_max] > strMap[tmp_secondmax]) {
						tmp = tmp_max;
						tmp_max = tmp_secondmax;
						tmp_secondmax = tmp;
					}
				}
			}
			high = strMap[tmp_max];
			hfwords.push_back(tmp_max);
		}
	}

	void Hfwordgroup(vector<string> &hfwordgroup, unordered_map<string, int> dstrMap) 
	{
		int high = 100000000, i;
		string tmp_max, tmp_secondmax, tmp;
		unordered_map<string, int>::iterator iter;
		iter = dstrMap.begin();
		tmp_secondmax = iter->first;
		for (i = 0; i < 10; i++)
		{
			for (iter = dstrMap.begin();iter != dstrMap.end();iter++)
			{
				tmp_max = iter->first;
				if (dstrMap[tmp_max] < high && dstrMap[tmp_max] > dstrMap[tmp_secondmax]) {
					tmp_max = tmp_max;
				}
				else {
					if (dstrMap[tmp_max] > dstrMap[tmp_secondmax]) {
						tmp = tmp_max;
						tmp_max = tmp_secondmax;
						tmp_secondmax = tmp;
					}
				}
			}
			high = dstrMap[tmp_max];
			hfwordgroup.push_back(tmp_max);
		}
	}

	void Output()
	{
		int m;
		unsigned int i; 
		unordered_map<string, int>::iterator iter = strMap.begin();
		for (iter;iter != strMap.end();iter++)
			words_num++;
		for (i = m = 0;i < strfile.size();i++)
			if (strfile[i] >= 32 && strfile[i] <= 126)
				m++;
		cout << "characters:" << ' ' << m << endl;
		cout << "lines:" << ' ' << lines << endl;
		cout << "words:" << ' ' << words_num << endl;
		for (i = 0;i < hfwords.size();i++)
			cout << '<' << hfwords[i] << ">:   " << strMap[hfwords[i]] << endl;
		for (i = 0;i < hfwordgroup.size();i++)
			cout << '<' << hfwordgroup[i] << ">:   " << dstrMap[hfwordgroup[i]] << endl;
		ofstream Save("E:/ConsoleApplication2/Result.txt");
		Save << "characters:" << ' ' << m << endl;
		Save << "lines:" << ' ' << lines << endl;
		Save << "words:" << ' ' << words_num << endl;
		for (i = 0;i < hfwords.size();i++)
			Save << '<' << hfwords[i] << ">:   " << strMap[hfwords[i]] << endl;
		for (i = 0;i < hfwordgroup.size();i++)
			Save << '<' << hfwordgroup[i] << ">:   " << dstrMap[hfwordgroup[i]] << endl;
	}

};



int main()
{
	Dictionary test;
	unsigned int i = 1;
	system("color 0F");
	test.Initwords(test.words);
	test.DfsFolder(test.path, 0, test.filename);
	for (i = 0;i < test.filename.size();i++) {
		test.Countlines(test.filename[i], test.lines);
		test.Readfile(test.strfile, test.filename[i]);
	}
	test.Selectwords(test.strfile, test.words,test.strMap,test.words_num);
	test.Hfwords(test.hfwords, test.strMap);
	test.Hfwordgroup(test.hfwordgroup, test.dstrMap);
	test.Output();
	return 0;
}
