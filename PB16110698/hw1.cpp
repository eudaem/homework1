//#include "WordsFrequency.h"
#include<iostream>
#include<unordered_map>
#include<string>
#include<fstream>
#include<algorithm>
#include<time.h>
#include<iomanip>
#include<string.h>
//#include<io.h> //if Windows, use this and dfsFolder()
#include<dirent.h>//if linux, use this and traverseFile() and 
#include<sys/stat.h>

using namespace std;
class Word {
public:	
	string value;
	int repeatTimes;
public:
	Word();
	~Word();
};
Word::Word() {
	this->value = "";
	this->repeatTimes = 0;
}
Word::~Word(){}

class Phrase {
public:
	string firstWord;
	string secondWord;
	int repeatTimes;
public:
	Phrase();
	~Phrase();
};
Phrase::Phrase() {
	this->firstWord = "";
	this->secondWord = "";
	this->repeatTimes = 0;
}
Phrase::~Phrase(){}

void dfsFolder(string folderPath, int depth);
void dfsFolderLinux(string folderPath);
int wordType(char n);
bool IsChar(char n);
void wordOperate(string &str);
void wordMapInsert(string word);
vector<Word> wordTopTen();
void phraseMapInsert(Phrase phrase);
vector<Phrase> phraseTopTen();
//above: something move in from WordsFrequency.h 

string strNew, strOld;
int charSum=0,lineSum=0,wordSum=0;
unordered_map<string, Word> wordMap;
unordered_map<string, Phrase> phraseMap;

//dfs traverse files in Windows, consult https://blog.csdn.net/qq289665044/article/details/48623325 
/*
void dfsFolder(string folderPath, int depth)
{
	ifstream infile;
	string strReg;
	_finddata_t fileInfo;
	string strFind = folderPath + "/*.*";     
	long handle = _findfirst(strFind.c_str(), &fileInfo);

	//if fail
	if (handle == -1)
	{
		cout << "cannot match the folder path" << endl;
		return ;
	}

	do
	{ 
		//prevent to dfs files on its same depth(".") or from its root("..")  
		if (fileInfo.attrib == _A_SUBDIR){
			int depthTemp = depth;
			if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)    
				dfsFolder(folderPath + '/' + fileInfo.name, depthTemp + 1);
		}
		else{
			infile.open(folderPath + '/' + fileInfo.name, ios::in);
			
			while (getline(infile, strReg)) {  
				lineSum++;
				wordOperate(strReg);
			} 
			infile.close();
		}
	} while (!_findnext(handle, &fileInfo));    
											      
	_findclose(handle);
}
*/

//traverseFile in Linux
void dfsFolderLinux(string folderPath)
{
	DIR *dir_ptr;
	struct stat infobuf;
	struct dirent *direntp;
	string name, temp;
	ifstream infile;
	string  strReg;
	if ((dir_ptr = opendir(folderPath.c_str())) == NULL)
		perror("can not open");
	else
	{
		while ((direntp = readdir(dir_ptr)) != NULL)
		{
			temp = "";
			name = direntp->d_name;
			if (name == "." || name == "..")
			{
				;
			}
			else
			{
				temp += folderPath;
				temp += "/";
				temp += name;
 
				if ((stat(temp.c_str(), &infobuf)) == -1)
					printf("#########\n");
				if ((infobuf.st_mode & 0170000) == 0040000)
				{

					dfsFolderLinux(temp);
				}
				else
				{  
					infile.open(temp, ios::in);
					while (getline(infile, strReg)) {
						lineSum++;
						wordOperate(strReg);
					}
					infile.close();
				}
			}
		}
	}
	closedir(dir_ptr);
}


//judge wordType,1:alpha,2:symbol,0:others
int wordType(char n) {
	if ((n >= 65 && n <= 90 )|| (n >= 97 && n <= 122) || (n >= 48 && n <= 57)) {
		if ((n >= 65 && n <= 90) || (n >= 97 && n <= 122)) return 1;
		else return 0;
	}
	else return 2;
}

//judge if n is a char.(ascii 32~126)
bool IsChar(char n) {
	if (n >= 32 && n <= 126) return true;
	return false;
}

//when getline, operate the string and realize function:find Words' and Phrases' frequency.
void wordOperate(string &strx) {
	Phrase phTemp;
	size_t length = 1+strx.length();
	int wordBegin, wordEnd;
	bool flag = true;

	strx += " ";//avoid overflow or miss

	for(int i=0;(size_t)i<length-1;i++)
		if (IsChar(strx[i])) charSum++;//------charSum----------

	for (wordBegin = 0; wordType(strx[wordBegin]) == 2 && (size_t)wordBegin < length; wordBegin++) {
		//if (IsChar(strx[wordBegin])) charSum++;
	}//find the first alpha

	for (int it = wordBegin; strx[it] != '\0' && (size_t)it < length; it++) {
		if (wordType(strx[it])==2) {
			for (wordEnd = wordBegin; wordEnd - wordBegin < 4 && (size_t)wordEnd < length; wordEnd++) {
				if (wordType(strx[wordEnd])!=1) {
					flag = false;
					break;
				}
			}//when flag==false,it means the thing we find is NOT a word
			if (wordEnd == length && wordEnd-wordBegin<4) flag = false;//boundary condition

			if (flag == true) {
				wordSum++;
				strNew = strx.substr(wordBegin, it - wordBegin);
				wordMapInsert(strNew);

				if (strOld != "") {
					phTemp.firstWord = strOld;
					phTemp.secondWord = strNew;
					phraseMapInsert(phTemp);
				}
				strOld = strNew;
			}//if it's a word, insert into wordMap; if it can be used to build a phrase,
			//insert into phraseMap, update strNew
			flag = true;
			wordBegin = it + 1;
		}
	}
}

//deal with word and insert into wordMap
void wordMapInsert(string word) {
	if (wordType(word[0])!=1)	return;
	
	string wordTemp = word;
	string::iterator it=word.end();

	while (wordType(*it) != 1) it--; //find alpha

	word.erase(it + 1, word.end());//throw tails                     
		
	transform(word.begin(), word.end(), word.begin(), ::toupper); 
	wordMap[word].repeatTimes++;//transform to upper and repeatTimes++               

	if (wordMap[word].value == "" || wordMap[word].value.compare(wordTemp)>0) 
			wordMap[word].value = wordTemp;
			//if the origin shape of word < value, update value=word.originShape
}

//rank words by repeatTimes ,rank 10
vector<Word>wordTopTen() {
	vector<Word> rankTen(10);//at first, rankTen[i].repeatTimes=0
	unordered_map<string,Word>::iterator it = wordMap.begin();

	//insert rank to select top10
	for (; it != wordMap.end(); it++){
		if (it->second.repeatTimes > rankTen[9].repeatTimes) {
			if (it->second.repeatTimes > rankTen[0].repeatTimes)
				rankTen.insert(rankTen.begin(), it->second);
			else {
				for (int i = 1; i <= 9; i++) {
					if ((it->second.repeatTimes >= rankTen[i].repeatTimes) && (it->second.repeatTimes <= rankTen[i - 1].repeatTimes)) {
						rankTen.insert(rankTen.begin() + i, it->second);
						break;
					}
				}
			}
		}
	}
	rankTen.erase(rankTen.begin() + 10, rankTen.end());
	return rankTen;
}

//deal with phrase and insert into phraseMap
void phraseMapInsert(Phrase phrase) {
	if (wordType(phrase.firstWord[0])!=1 || wordType(phrase.secondWord[0])!=1)	return; 

	Phrase phTemp;
	phTemp.firstWord = phrase.firstWord;
	phTemp.secondWord = phrase.secondWord;

	string::iterator it1 = phrase.firstWord.end();
	string::iterator it2 = phrase.secondWord.end();
	it1--; it2--;
	while (wordType(*it1)!=1) {
		it1--;
	};                                                               
	while (wordType(*it2)!=1) {
		it2--;
	};
	phrase.firstWord.erase(it1 + 1, phrase.firstWord.end());                                           
	phrase.secondWord.erase(it2 + 1, phrase.secondWord.end());//同wordMapInsert中的操作,对两个单词分别去除尾部无关字符

	transform(phrase.firstWord.begin(), phrase.firstWord.end(), phrase.firstWord.begin(), ::toupper);         
	transform(phrase.secondWord.begin(), phrase.secondWord.end(), phrase.secondWord.begin(), ::toupper);
	string upperStr = phrase.firstWord +" "+ phrase.secondWord;
	phraseMap[upperStr].repeatTimes++;//转化为大写，统计词频

	if (phraseMap[upperStr].firstWord == "" || (phraseMap[upperStr].firstWord + phraseMap[upperStr].secondWord).compare(phTemp.firstWord + phTemp.secondWord)>0) {
		phraseMap[upperStr].firstWord = phTemp.firstWord;
		phraseMap[upperStr].secondWord = phTemp.secondWord;
	}//词组原型与map中存的value比较，留下字典序较小的                                                          
}

//rank phrases by repeatTimes ,rank 10
vector<Phrase> phraseTopTen() {
	vector<Phrase> rankTen(10);//at first, rankTen[i].repeatTimes=0
	unordered_map<string, Phrase>::iterator it = phraseMap.begin();

	//insert rank to select top10
	for (; it != phraseMap.end();it++) {
		if (it->second.repeatTimes > rankTen[9].repeatTimes) {
			if (it->second.repeatTimes > rankTen[0].repeatTimes)
				rankTen.insert(rankTen.begin(), it->second);
			else {
				for (int i = 1; i <= 9; i++) {
					if ((it->second.repeatTimes >= rankTen[i].repeatTimes) && (it->second.repeatTimes <= rankTen[i - 1].repeatTimes)) {
						rankTen.insert(rankTen.begin() + i, it->second);
						break;
					}
				}
			}
		}
	}

	rankTen.erase(rankTen.begin() + 10, rankTen.end());
	return rankTen;
}

int main(int argc, char * argv[]) {
	string fileName=argv[1];
	ofstream fout;
	fout.open("result.out", ios::out);
	//printf("please enter the filePath:\n");
	//getline(cin, fileName);
	cout << "wait a moment plz....." << endl;

	double timeSum;
	clock_t tStart = clock();
	//dfsFolder(fileName, 0);
	dfsFolderLinux(fileName);
	vector<Word> word= wordTopTen();
	vector<Phrase> phrase = phraseTopTen();
	timeSum =(double)(clock() - tStart) / CLOCKS_PER_SEC;

	//printf("NumOfChar：%d\n", charSum);
	fout << "NumOfChar:" << charSum << endl;
	//printf("NumOfWord：%d\n", wordSum);
	fout << "NumOfWord:" << wordSum << endl;
	//printf("NumOfLine：%d\n\n", lineSum);
	fout << "NumOfLine:" << lineSum << endl;
	//printf("Top10 Words:\n");
	fout << "Top10 Words:" << endl;
	for (int i = 0; i < 10; i++) {
		fout << setw(12) << word[i].value << " " << word[i].repeatTimes << endl;
	}
	fout << endl;
	fout << "Top10 Phrases:" << endl;
	for (int i = 0; i < 10; i++) {
		fout << setw(12) << phrase[i].firstWord << " " << setw(12) << phrase[i].secondWord << " " << phrase[i].repeatTimes << endl;
	}
	fout << endl;
	fout << "TimeSum:" << setprecision(4)<< timeSum <<"S"<< endl;
	fout.close();
}
