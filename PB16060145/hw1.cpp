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

class word {

private:
	string wordStr;
	unsigned int freq;
public:
	word() = default;
	word(string str) {
		wordStr = str;
		freq = 1;
	}

	string getWordStr() {
		return wordStr;
	}

	unsigned int getFreq() {
		return freq;
	}

	void addFreq() {
		freq++;
	}

	void resetWordStr(string str) {
		if (str < wordStr) {
			wordStr = str;
		}
	}

	/*this requires establishing a new word() as soon as
	the iterator gives a new matched expression*/

	bool operator == (const word &obj) const {

		int flag = 0;
		int i, thisLen = wordStr.length(), objLen = obj.wordStr.length();
		for (i = 0; i < thisLen && i < objLen; i++) {

			if (wordStr[i] != obj.wordStr[i]) {

				if (isalpha(wordStr[i]) || isalpha(obj.wordStr[i])) {
					flag = 1;
					break;
				}
			}
		}
		return (flag == 0) ? true : false;
	}

	void printWord(ofstream &output) {
		output << wordStr << "\t" << freq << endl;
	}
};

bool wordCompare(word former, word latter) {
	return former.getFreq() > latter.getFreq();
}

void examineNewWord(vector<word> &wvec, word &newWord) {

	vector<word>::iterator beg = wvec.begin(), end = wvec.end(), itr;
	itr = find(beg, end, newWord);    //is there any repition?

	if (itr != end) {                 // this word already exists in wvec
		itr->resetWordStr(newWord.getWordStr());
		itr->addFreq();
	}
	else {
		wvec.push_back(newWord);
	}
}


/* collect all expressions that match the definition of word in the parameter string */
void getNewExpr(string &str,vector<word> &wvec) {

	word newWord;
	string wordPattern("[[:alnum:]]{4}[[:alnum:]]*");
	regex reg(wordPattern);
	

	for (sregex_iterator it(str.begin(), str.end(), reg), end_it;
		it != end_it; it++) {
		
		newWord = word(it->str());
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

		if (getline(input, str)) {

			lines++;
		}
	}

	return lines;
}



/*
  process one file, update the amount of characters and the amount of lines, 
  collect all expressions that match the word definition into wvec.
*/
void fileProcess(string filename, unsigned long &charNum, unsigned long &lineNum, vector<word> &wvec) {

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
		return ;
	}
	

	if (input.eof())
		return;

		buffer << input.rdbuf();
		srcStr = buffer.str();

		// update the amount of characters
		charNum += getCharNum(srcStr);

		//update the amount of lines
		lineNum += getLineNum(filename);

		//update the wvec
		getNewExpr(srcStr, wvec);
	

	input.close();
}


/* print the results in the required format*/
void getResult(const char* resfile, unsigned long &charNum, unsigned long &lineNum, vector<word> &wvec) {

	//sort wvec in ASCII order
	vector<word>::iterator beg = wvec.begin(), end = wvec.end(), itr;
	sort(beg, end, wordCompare);

	ofstream output(resfile);
	 
	auto wvecSize = wvec.size();
	output << "char_number :" << charNum << endl;
	output << "line_number :" << lineNum << endl;
	output << "word_number :" << wvecSize << endl;

	if(wvecSize){

		if (wvecSize < 10) {
			for (itr = beg; itr != end; itr++) {
				itr->printWord(output);
			}
		}
		
		else {
			vector<word>::iterator last = beg + 10;
			for (itr = beg; itr != last; itr++) {
				itr->printWord(output);
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

	unsigned long charNum = 0;
	unsigned long lineNum = 0;
	vector<word> wvec;
	int dirFlag;
	vector<string> fvec;

	//string path = "D:/Visual Studio/testDir";
	
	const char* resFile = "Result.txt";

	dirFlag = getAllFiles(argv[1], fvec);

	if (dirFlag == 0) {

		vector<string>::iterator end = fvec.end(), it;
		for (it = fvec.begin(); it != end; it++) {
			fileProcess(*it, charNum, lineNum, wvec);
		}
	}
	else {
		fileProcess(argv[1], charNum, lineNum, wvec);
	}
	
	getResult(resFile, charNum, lineNum, wvec);

	system("pause");
}
