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
int begFlag = 1;

typedef struct {
	unsigned int charNum;
	unsigned int lineNum;
	unsigned int wordNum;
}amount;

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
	/*
	bool operator == (const word &obj) const {

		int flag = 0;
		int i, thisLen = wordStr.length(), objLen = obj.wordStr.length();
		for (i = 0; i < thisLen && i < objLen; i++) {

			if (wordStr[i] != obj.wordStr[i] && abs(wordStr[i] - obj.wordStr[i]) != 32) {

				if (isalpha(wordStr[i]) || isalpha(obj.wordStr[i])) {
					flag = 1;
					break;
				}
			}
		}
		return (flag == 0) ? true : false;
	}
	*/
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


class phrase {

private:
	//string phrStr;
	unsigned int freq;
	word part1, part2;

public:
	//lack a default constructor

	phrase(word part1, word part2) {
		this->part1 = part1;
		this->part2 = part2;
		//phrStr = str;
		freq = 1;
	}
	/*
	string getPhrStr() {
		return phrStr;
	}
	*/
	word getPart1() {
		return part1;
	}

	word getPart2() {
		return part2;
	}

	unsigned int getFreq() {
		return freq;
	}

	void addFreq() {
		freq++;
	}

	void resetPhrase(phrase &obj) {

		//string objStr = obj.getPhrStr();

		word objPart1 = obj.getPart1();   
		word objPart2 = obj.getPart2();  
		// '||' is a short circuit operator
		if (objPart1.getWordStr() < this->part1.getWordStr() || objPart2.getWordStr() < this->part2.getWordStr()) {
			this->part1 = objPart1;
			this->part2 = objPart2;
		}
	}

	bool operator == (const phrase &obj) const {

		word objPart1 = obj.part1, objPart2 = obj.part2;
		return (part1 == objPart1 && part2 == objPart2);
	}

	void printPhrase(ofstream &output) {
		string word1 = part1.getWordStr(), word2 = part2.getWordStr();
		word1 < word2 ?
			output << word1 + " " + word2 << "\t" << freq << endl :
			output << word2 + " " + word1 << "\t" << freq << endl;
	}
};


bool wordCompare(word former, word latter) {
	return former.getFreq() > latter.getFreq();
}

bool phraseCompare(phrase former, phrase latter) {
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

void examineNewPhr(vector<phrase> &pvec, phrase &newPhrase) {

	vector<phrase>::iterator beg = pvec.begin(), end = pvec.end(), itr;
	itr = find(beg, end, newPhrase);   ////is there any repition?

	if (itr != end) {
		itr->resetPhrase(newPhrase);
		itr->addFreq();
	}
	else {
		pvec.push_back(newPhrase);
	}
}


/* collect all expressions that match the definition of word in the parameter string */
void getNewExpr(string &str, vector<word> &wvec, vector<phrase> &pvec, amount &result) {

	word newWord;
	string wordPattern("[[:alpha:]]{4}[[:alnum:]]*");
	regex reg(wordPattern);

	//intermediate variables in generating a new phrase
	
	//string::size_type pos1, pos2;
	string newPhrStr = "\0";
	word part1("\0"), part2("\0");
	phrase newPhrase( part1, part2);
	/*
	  collect a word in advance, then combine two words and the substring 
	  between them into  a phrase
	*/
	for (sregex_iterator it(str.begin(), str.end(), reg), end_it;
		it != end_it; it++) {
		
		result.wordNum++;
		newWord = word(it->str());
		examineNewWord(wvec, newWord);

		if (begFlag) {
			wvec.push_back(newWord);
			begFlag = 0;
			part1 = newWord;
		}
		else {
			                  
			part2 = newWord;
			newPhrase = phrase(part1, part2);
			examineNewPhr(pvec, newPhrase);
			                  //pos1 = pos2;
			part1 = part2;
		}
		
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
void fileProcess(string filename, amount &result, vector<word> &wvec, vector<phrase> &pvec) {

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
		result.charNum += getCharNum(srcStr);

		//update the amount of lines
		result.lineNum += getLineNum(filename);

		//update the wvec
		getNewExpr(srcStr, wvec, pvec,result);
	

	input.close();
}


/* print the results in the required format*/
void getResult(const char* resfile, amount &result, vector<word> &wvec, vector<phrase> &pvec) {

	auto wvecSize = wvec.size();
	auto pvecSize = pvec.size();
	ofstream output(resfile);

	output << "char_number :" << result.charNum << endl;
	output << "line_number :" << result.lineNum << endl;
	output << "word_number :" << result.wordNum << endl;

	//sort wvec in descending frequency order
	vector<word>::iterator wbeg = wvec.begin(), wend = wvec.end(), witr;
	sort(wbeg, wend, wordCompare);

	output << " " << endl;
	output << "the top ten frequency of words" << endl;
	if(wvecSize){

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

	//sort pvec in descending frequency order
	vector<phrase>::iterator pbeg = pvec.begin(), pend = pvec.end(), pitr;
	sort(pbeg, pend, phraseCompare);

	output << " " << endl;
	output << "the top ten frequency of phrases" << endl;
	if (pvecSize) {

		if (pvecSize < 10) {
			for (pitr = pbeg; pitr != pend; pitr++) {
				pitr->printPhrase(output);
			}
		}
		else {
			vector<phrase>::iterator plast = pbeg + 10;
			for (pitr = pbeg; pitr != plast; pitr++) {
				pitr->printPhrase(output);
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
	vector<word> wvec;
	vector<phrase> pvec;
	int dirFlag;
	vector<string> fvec;

	string path = "2消失的密室.txt";
	
	const char* resFile = "AllFiles.txt";

	dirFlag = getAllFiles(path, fvec);

	if (dirFlag == 0) {

		vector<string>::iterator end = fvec.end(), it;
		for (it = fvec.begin(); it != end; it++) {
			fileProcess(*it, result, wvec, pvec);
		}
	}
	else {
		fileProcess(path, result, wvec, pvec);
	}
	
	getResult(resFile, result, wvec, pvec);
	system("pause");
}
