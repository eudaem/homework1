//please swithch the Unicode character set to MBCS(Multi-Byte Character System before compiling!!!!

//header files
#ifdef __linux__
#include <dirent.h>
#endif


//result structure
typedef struct {
	long charaNum;
	long lineNum;
	long wordNum;
	map<string, int> dict;
	map<string, int> phrase;
}fileProp;

//declarations
void fileRead(string p, fileProp *result);

void folderTraverse(string p, fileProp *result);
void DFS(string p, fileProp *result);
bool dirScan(string p, fileProp *result);

bool isSourceFile(string p);
bool isValidDir(string p);

long charaStat(string p);
long lineStat(string p);
bool isWord(char *ch);
long wordStat(string p, fileProp *result);
void phraseStat(string p,fileProp *result);

void resultPrint(fileProp *result);

#ifdef WIN32
void DFS(string p, fileProp *result) {
	_finddata_t file_info;
	string current_path = p + "/*.*";
	long handle = _findfirst(current_path.c_str(), &file_info);
	if (-1L == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}
	do {
		if (file_info.attrib & _A_SUBDIR) {
			if ((strcmp(file_info.name, "..") != 0) && (strcmp(file_info.name, ".") != 0))
				DFS(p + '/' + file_info.name, result);
		}
		else {
			cout << p << "\\" << file_info.name << endl;
			fileRead(p + "\\" + file_info.name, result);
		}
	} while (_findnext(handle, &file_info) == 0);
	_findclose(handle);
}
#endif

#ifdef __linux__
void DFS(string path, fileProp *result)
{
	string name;
	DIR* dir = opendir(path.c_str());
	dirent* p = NULL;
	while ((p = readdir(dir)) != NULL)
	{
		if (p->d_name[0] != '.')
		{
			string name = path + "/" + string(p->d_name);
			fileRead(name, result);
			if (p->d_type == 4) {
				GetAllFiles(name, files);
			}
		}

	}
	closedir(dir);

}
#endif

//main
int main(int argc,char *argv[])
{
	fileProp *result = new fileProp;
	result->charaNum = 0;
	result->lineNum = 0;
	result->wordNum = 0;
	string fileName;
	fstream fileEx;
	fileName = argv[1];
//	cout << "Please input the name of the file/folder." << endl;
//	getline(cin, fileName);
	fileEx.open(fileName, ios::in);
	if (isSourceFile(fileName)) {
		fileRead(fileName, result);
		resultPrint(result);
	}
	else if (isValidDir(fileName)) {
		folderTraverse(fileName, result);
		resultPrint(result);
	}
	else
		cout << "It's not a valid file!" << endl;
	system("pause");
	return 0;
}

//statistic for different types of file
void fileRead(string p, fileProp *result) {
	result->charaNum += charaStat(p);
	result->lineNum += lineStat(p);
	result->wordNum += wordStat(p, result);
	phraseStat(p, result);
}


bool dirScan(string p, fileProp *result) {
	_finddata_t file_info;
	string current_path = p + "/*.*";
	int handle = _findfirst(current_path.c_str(), &file_info);
	if (-1 == handle)
		return false;
	do {
		if (file_info.attrib != _A_SUBDIR)
			fileRead(file_info.name, result);
	} while (!_findnext(handle, &file_info));
	_findclose(handle);
	return true;
}


void folderTraverse(string p, fileProp *result) {
	if (!dirScan(p, result))
		cout << "Traverse failed" << endl;
	DFS(p, result);
}


//judge the type of the file
bool isSourceFile(string p) {
	_finddata_t file_info;
	string current_path = p;
	LPCSTR ext;
	char name[256];
	int handle = _findfirst(current_path.c_str(), &file_info);
	if (file_info.attrib == _A_SUBDIR)
		return false;

	strcpy_s(name, p.c_str());
	ext = PathFindExtension(name);
	if (ext != ".jpg")
		return true;
	else
		return false;
}


bool isValidDir(string p) {
	if (true == (bool)PathIsDirectory(p.c_str()))
		return true;
	else
		return false;
}


//statistic functions
long charaStat(string p) {
	long n = 0;
	char ch;
	ifstream infile(p);
	while (infile.get(ch)) {
		if ((ch >= 32) && (ch <= 126))
			n++;
	}return n;
}

long lineStat(string p) {
	char c;
	long n = 0;
	ifstream infile(p);
	while (infile.get(c)) {
		if (c == '\n')
			n++;
	}
	if (c != '\n')
		return n + 1;
	else
		return n;
}

long wordStat(string p, fileProp *result) {
	int i = 0;
	char c;
	char str[256] = { 0 };
	string word1, word2;
	ifstream infile(p);
	while (infile.get(c)) {
		if ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z'))
		{
			str[i] = c;
			i++;
		}
		else if (isWord(str)) {
			result->wordNum++;
			word1 = str;
			transform(word1.begin(),word1.end(),word1.begin(), ::toupper);
			result->dict[word1] += 1;  //word frequency
			for (i = 0; i < 256; i++)
				str[i] = 0;
			i = 0;
		}
		else {
			for (i = 0; i < 256; i++)
				str[i] = 0;
			i = 0;
		}
	}
	return 0;
}

void phraseStat(string p, fileProp *result) {
	int i = 0, j = 0;
	char c;
	ifstream infile(p);
	bool isPhrase;
	string phr1,phr2,phr;
	char wordA[1024] = { '\0' };
	char wordB[1024] = { '\0' };
	infile.get(c);
	while (!infile.eof()) {
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z')||(c>='0')&&(c<='9'))
		{
			if ((!infile.get(c)) ||
				((c < '0') || (c > '9'&&c < 'A')||(c>'Z'&&c<'a')||(c>'z')))
				break;
			wordA[i] = c;
			i++;
		}
		i = 0;
		infile.get(c);
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordB[j] = c;
			j++;
		}
		j = 0;
		infile.get(c);
		isPhrase = isWord(wordA) && isWord(wordB);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+ phr2;
			result->phrase[phr] += 1;
		}
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordA[i] = c;
			i++;
		}
		i = 0;
		infile.get(c);
		isPhrase = isWord(wordA) && isWord(wordB);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+ phr2;
			result->phrase[phr] += 1;
		}
		while ((c >= 'A'&&c <= 'Z') || (c >= 'a'&&c <= 'z') || (c >= '0') && (c <= '9'))
		{
			if (!infile.get(c) ||
				((c < '0') || (c > '9'&&c < 'A') || (c>'Z'&&c<'a') || (c>'z')))
				break;
			wordB[j] = c;
			j++;
		}
		j = 0;
		infile.get(c);
		if (isPhrase)
		{
			phr1 = wordA;
			phr2 = wordB;
			phr = phr1 +" "+phr2;
			result->phrase[phr] += 1;
		}
	}
	
}

bool isWord(char *ch) {
	if (
		((ch[0] >= 'A'&&ch[0] <= 'Z') || (ch[0] >= 'a'&&ch[0] <= 'z'))
		&& ((ch[1] >= 'A'&&ch[1] <= 'Z') || (ch[1] >= 'a'&&ch[1] <= 'z'))
		&& ((ch[2] >= 'A'&&ch[2] <= 'Z') || (ch[2] >= 'a'&&ch[2] <= 'z'))
		&& ((ch[3] >= 'A'&&ch[3] <= 'Z') || (ch[3] >= 'a'&&ch[3] <= 'z'))
		)
		return true;
	else
		return false;
}


//output function
bool cmp(const pair<string, int> &p1, const pair<string, int> &p2)
{
	if (p1.second > p2.second)
		return true;
	else if (p1.second == p2.second)
		return false;
	else
		return false;
}

void resultPrint(fileProp *result) {
	ofstream outfile("result.txt");
	map<string, int> ::iterator it;
	map<string, int>::iterator it1;
	vector<pair<string, int>> arr;
	vector<pair<string, int>> arr1;

	cout << "characters:" << result->charaNum << endl;
	cout << "word:" << result->wordNum << endl;
	cout << "line:" << result->lineNum << endl;
	cout << endl;

	outfile << "characters:" << result->charaNum << endl;
	outfile << "word:" << result->wordNum << endl;
	outfile << "line:" << result->lineNum << endl;
	outfile << endl;


	for (it = result->dict.begin(); it != result->dict.end(); it++)
		arr.push_back(make_pair(it->first, it->second));
	sort(arr.begin(), arr.end(), cmp);
	if ((arr.end() - arr.begin()) >= 10) {
		for (vector<pair<string, int> >::iterator it = arr.begin(); it < (arr.begin() + 10); it++)
			cout << it->first << ':' << it->second << endl;
		cout << endl;
	}
	else {
		for (vector<pair<string, int> >::iterator it = arr.begin(); it < arr.end(); it++)
			cout << it->first << ':' << it->second << endl;
		cout << endl;
	}
	if ((arr.end() - arr.begin()) >= 10) {
		for (vector<pair<string, int> >::iterator it = arr.begin(); it < (arr.begin() + 10); it++)
			outfile << it->first << ':' << it->second << endl;
	}
	else {
		for (vector<pair<string, int> >::iterator it = arr.begin(); it < arr.end(); it++)
			outfile << it->first << ':' << it->second << endl;
	}

	for (it1 = result->phrase.begin(); it1 != result->phrase.end(); it1++)
		arr1.push_back(make_pair(it1->first, it1->second));
	sort(arr1.begin(), arr1.end(), cmp);
	if ((arr1.end() - arr1.begin()) >= 10) {
		for (vector<pair<string, int> >::iterator it2 = arr1.begin(); it2 < (arr1.begin() + 10); it2++)
			cout << it2->first << ':' << it2->second << endl;
	}
	else {
		for (vector<pair<string, int> >::iterator it2 = arr1.begin(); it2 < arr1.end(); it2++)
			cout << it2->first << ':' << it2->second << endl;
	}
	if ((arr1.end() - arr1.begin()) >= 10) {
		for (vector<pair<string, int> >::iterator it2 = arr1.begin(); it2 < (arr1.begin() + 10); it2++)
			outfile << it2->first << ':' << it2->second << endl;
	}
	else {
		for (vector<pair<string, int> >::iterator it2 = arr1.begin(); it2 < arr1.end(); it2++)
			outfile << it2->first << ':' << it2->second << endl;
	}



}
