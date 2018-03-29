#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
#include<cctype>
#include<regex>
#include<io.h>

using namespace std;

#define WORD_POOL_SIZE 18000000;
#define MAX_FIGURES 20;
int begFlag = 1;

typedef struct {
	unsigned int charNum;
	unsigned int lineNum;
	unsigned int wordNum;
}amount;

class word{

public:
	string wordStr;
	unsigned int freq;

	word(string str, unsigned int fre) {
		wordStr = str;
		freq = fre;
	}


	void resetWordStr(string str) {
		if (str < wordStr) {
			wordStr = str;
		}
	}

	bool operator == (const word &obj) const {
		string word1 = this->wordStr, word2 = obj.wordStr;
		int i = word1.length() - 1;
		int j = word2.length() - 1;
		while (i >= 0)
		{
			if (word1[i] >= '0'&&word1[i] <= '9')
				word1[i] = '\0';
			else break;
			i--;
		}
		while (j >= 0)
		{
			if (word2[j] >= '0'&&word2[j] <= '9')
				word2[j] = '\0';
			else break;
			j--;
		}
		if (i == j) {

			for (int t = 0; t <= i; t++) {
				if (word1[t] != word2[t] && abs(word1[t] - word2[t]) != 32)
					return false;
			}

		}
		else return false;
		return true;
	}

	void printWord(ofstream &output) {
		output << wordStr << "\t" << freq << endl;
	}
	
};

bool wordCompare(word former, word latter) {
	return former.freq > latter.freq;
}

unsigned int Hash(string str) {
	
	const char *p = str.c_str();
	unsigned int seed = 7, key;
	unsigned long long hash = 0;
	int figures=0;
	while (*p!='\0'&& figures<= 20) {
		hash = hash*seed + (*p);
		p++;
		figures++;
	}
	key = hash%WORD_POOL_SIZE;
	return(key);
}

void examineNewWord(vector<word> &wvec, word &newWord) {

	string str = newWord.wordStr;
	int i = str.length() - 1;

	while (i >= 0) {
		if (str[i] >= '0'&&str[i] <= '9') {
			str[i] = '\0';
		}
		else if (str[i]>=97&&str[i]<=122) {
			str[i] = str[i] - 32;
		}
		i--;
	}

	unsigned int key = Hash(str);
	int outOfSlot = 1;
	int open = 0;
	vector<word>::iterator beg = wvec.begin();
	vector<word>::iterator itr = beg + key;
	
	while (outOfSlot) {
		itr = beg + (itr - beg + open * 13)%WORD_POOL_SIZE;
		if (itr->wordStr == "\0") {
			itr->wordStr = newWord.wordStr;
			itr->freq++;
			outOfSlot = 0;
		}
		else if (*itr == newWord) {
			itr->resetWordStr(newWord.wordStr);
			itr->freq++;
			outOfSlot = 0;
		}
		open++;
	}

}

void getNewExpr(string &str, vector<word> &wvec, unsigned int &wordNum) {

	word newWord("\0",1);
	string wordPattern("[[:alpha:]]{4}[[:alnum:]]{0,1020}");
	regex reg(wordPattern);

	for (sregex_iterator it(str.begin(), str.end(), reg), end_it;
		it != end_it; it++) {
		wordNum++;
		newWord.wordStr = it->str();
		examineNewWord(wvec, newWord);
	}
}

/* calculate the amount of characters with ASCII code within [32,126]*/
unsigned long getCharNum(string &str) {

	unsigned long charNum = 0;
	string::iterator end = str.end(), citr;
	for (citr = str.begin(); citr != end; citr++) {
		if (*citr >= 32 && *citr <= 126)
			charNum++;
	}
	return charNum;
}


/* calculate the number of lines in one file */
unsigned long getLineNum(string filename) {

	ifstream input(filename);
	unsigned long lines = 0;
	string str;
	while (!input.eof()) {
		/*
		if (getline(input, str)) {

			lines++;
		}*/
		getline(input, str);
		lines++;
	}

	return lines;
}

void fileProcess(const char* filename, amount &result, vector<word> &wvec) {

	ifstream input;
	stringstream buffer;
	string srcStr;

	try {
		input.open(filename);
		if (!input.is_open()) {
			throw runtime_error("cannot open the file");
		}
	}
	catch (runtime_error err) {
		cout << err.what();
		return;
	}


	if (input.eof())
		return;

	buffer << input.rdbuf();
	srcStr = buffer.str();

	// update the amount of characters
	result.charNum += getCharNum(srcStr);

	//update the amount of lines
	result.lineNum += getLineNum(filename);

	//update the wvec
	getNewExpr(srcStr, wvec, result.wordNum);


	input.close();
}

/* print the results in the required format*/
void getResult(const char* resfile, amount &result, vector<word> &wvec) {

	auto wvecSize = wvec.size();
	//auto pvecSize = pvec.size();
	ofstream output(resfile);

	output << "char_number :" << result.charNum << endl;
	output << "line_number :" << result.lineNum << endl;
	output << "word_number :" << result.wordNum << endl;

	//sort wvec in descending frequency order
	vector<word>::iterator wbeg = wvec.begin(), wend = wvec.end(), witr;
	sort(wbeg, wend, wordCompare);

	output << " " << endl;
	output << "the top ten frequency of words" << endl;
	if (wvecSize) {

		if (wvecSize < 10) {
			for (witr = wbeg; witr != wend; witr++) {
				witr->printWord(output);
			}
		}
		else {
			vector<word>::iterator wlast = wbeg + 10;
			for (witr = wbeg; witr != wlast; witr++) {
				witr->printWord(output);
			}
		}
	}
	
}

/* determine whether the given path is a directory or a file,
if it is a directory, push names of all the files in the
directory into fvec*/
int getAllFiles(string path, vector<string> &files)
{
	long   hFile = 0;
	int flag = -1;

	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		flag = 0;
		while (_findnext(hFile, &fileinfo) == 0)
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //if it is a folder
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("/").append(fileinfo.name));//save filename
					getAllFiles(p.assign(path).append("/").append(fileinfo.name), files);
				}
			}
			else    //it is a file
			{
				files.push_back(p.assign(path).append("/").append(fileinfo.name));//文件名
			}
		}
		_findclose(hFile);
	}

	return flag;
}

int main(int argc, char* argv[]) {

	amount  result;
	result.charNum = 0;
	result.lineNum = 0;
	result.wordNum = 0;
	vector<word> wvec(18000000,word("\0",0));
	int dirFlag;
	vector<string> fvec;

	string path = "05.txt";

	const char* resFile = "Result.txt";

	dirFlag = getAllFiles(argv[1], fvec);

	if (dirFlag == 0) {

		vector<string>::iterator end = fvec.end(), it;
		for (it = fvec.begin(); it != end; it++) {
			fileProcess(it->c_str(), result, wvec);
		}
	}
	else {
		fileProcess(argv[1], result, wvec);
	}

	getResult(resFile, result, wvec);
	//system("pause");
}

