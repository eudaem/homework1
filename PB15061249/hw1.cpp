#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include <cctype>
#include <algorithm>
#include<queue>
#include <fstream>  
#include <sstream>
#include <streambuf>  
#include <string>

using namespace std;

#ifdef __linux__
#include <dirent.h>

void GetAllFiles(string path, vector<string>& files) {
	string name;
	DIR* dir = opendir(path.c_str());
	dirent* p = NULL;
	while ((p = readdir(dir)) != NULL)
	{
		if (p->d_name[0] != '.')
		{
			string name = path + "/" + string(p->d_name);
			files.push_back(name);
			//cout << name << endl;
			if (p->d_type == 4) {
				GetAllFiles(name, files);
			}
		}

	}
	closedir(dir);

}

#endif

#ifdef _WIN32
#include<io.h>

void GetAllFiles(string path, vector<string>& files) {

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
struct strnum
{
	string str;
	int num;
	strnum()
	{
		str = "undefine";
		num = 0;
	}
	strnum(string str0, int num0)
	{
		str = str0;
		num = num0;
	}
};

string numtostring(int num)
{
	stringstream ss;
	ss << num;
	return ss.str();
}

bool operator < (const strnum sn1, const strnum sn2)
{
	if (sn1.num > sn2.num)
		return true;
	else if (sn1.num < sn2.num)
		return false;
	else
		return sn1.str > sn2.str;
}

int sameword(string a, string b)
{
	transform(a.begin(), a.end(), a.begin(), ::tolower);
	transform(b.begin(), b.end(), b.begin(), ::tolower);
	int alen, blen;//not inlude last num
	for (int i = 0; i < a.size(); i++)
	{
		if (isalpha(a[i]))
		{
			alen = i;
		}
	}
	for (int i = 0; i < b.size(); i++)
	{
		if (isalpha(b[i]))
		{
			blen = i;
		}
	}
	if (alen != blen)
	{
		return 0;
	}
	else
	{
		string suba, subb;
		suba = a.substr(0, alen);
		subb = b.substr(0, blen);
		if (suba != subb)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

void GetToptenPhrase(unordered_map<string, int> &Phrase, vector<string> &outline)
{
	priority_queue <strnum> queuestr;
	strnum sn1;
	strnum Maxsn;
	strnum output[10];
	int index = 0;
	auto it = Phrase.begin();
	for (int i = 0; i < 10 && it != Phrase.end(); ++i, ++it)
		queuestr.push(strnum(it->first, it->second));
	for (auto it2 = it; it2 != Phrase.end(); ++it2)
	{
		Maxsn = queuestr.top();
		sn1 = strnum(it2->first, it2->second);
		if (sn1 < Maxsn)
		{
			queuestr.pop();
			queuestr.push(sn1);
		}
	}
	while (!queuestr.empty())
	{
		sn1 = queuestr.top();
		queuestr.pop();
		output[index] = sn1;
		++index;
	}
	outline.push_back("the top ten frequency of phrase :");
//	cout << "the top ten frequency of phrase :" << endl;
	for (index = 9; index >= 0; --index)
//		cout << output[index].str << " " << output[index].num << endl;
		outline.push_back(output[index].str + " " + numtostring(output[index].num));
}

void GetToptenword(unordered_map<string, strnum> Word, vector<string> &outline)
{
	priority_queue <strnum> queuestr;
	strnum sn1;
	strnum Maxsn;
	strnum output[10];
	int index = 0;
	auto it = Word.begin();
	for (int i = 0; i < 10 && it != Word.end(); ++i, ++it)
		queuestr.push(it->second);

	for (auto it2 = it; it2 != Word.end(); ++it2)
	{
		Maxsn = queuestr.top();
		sn1 = it2->second;
		if (sn1 < Maxsn)
		{
			queuestr.pop();
			queuestr.push(sn1);
		}
	}

	while (!queuestr.empty())
	{
		sn1 = queuestr.top();
		queuestr.pop();
		output[index] = sn1;
		++index;
	}
	outline.push_back("the top ten frequency of word :");
//	cout << "the top ten frequency of word :" << endl;
	for (index = 9; index >= 0; --index)
		outline.push_back(output[index].str + " " + numtostring(output[index].num));
//		cout << output[index].str << " " << output[index].num << endl;
}

void phrasechange(unordered_map<string, int> &result, unordered_map<string, int> Phrasefre, unordered_map<string, strnum> &Wordfre)
{
	int pos;
	string curstr;
	string firststr;
	string secondstr;
	string phrase;
	int len;
	for (auto it = Phrasefre.begin(); it != Phrasefre.end(); it++)
	{
		curstr = it->first;
		len = (curstr).size();
		pos = curstr.find("_");

		firststr = curstr.substr(0, pos);
		firststr = Wordfre[firststr].str;

		secondstr = curstr.substr(pos + 1, len);
		secondstr = Wordfre[secondstr].str;

		phrase = firststr + " " + secondstr;
		result[phrase] = it->second;
	}
}

bool cmp_by_value(const pair<string, int>& lhs, const pair<string, int>& rhs) {
	return lhs.second > rhs.second;
}

void Overword(unordered_map<string, strnum> fre, int &Wordnum)
{
	for (auto i = fre.begin(); i != fre.end(); i++)
	{
		Wordnum = Wordnum + (i->second).num;
	}
}

/*
void GetFilename(string path, vector<string>& files)
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
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
					GetFilename(p.assign(path).append("/").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0); 
		_findclose(hFile);
	}
}
*/

int main(int argc, char *argv[])
{
	vector<string> Filename;
	string path;
	path = argv[1];
	GetAllFiles(path, Filename);
	int Filelen = Filename.size();
	int charnum = 0;
	int Linenum = 0;
	int Phrasenum = 0;
	int Wordnum = 0;
	unordered_map<string, strnum> Word;
	unordered_map<string, int> Phrase;
	unordered_map<string, int> Resultphrase;
	string Lastword;
	string Received;
	string Receivedkey;
	string Tempsum;
	string Tempsumkey;
	int j;
	char cur;
	int i;
	int mylen;
	int Textlen;
	string temp;
	fstream infile;
	for (i = 0; i < Filelen; i++)
	{
		infile.open(Filename[i]);
		if (infile.fail())
		{
			continue;
		}
		infile.close();
		ifstream text(Filename[i]);
		string Text((istreambuf_iterator<char>(text)), istreambuf_iterator<char>());
		int len = Text.size();
		if (len == 0)
		{
			++Linenum;
			continue;
		}
		if (Text[len - 1] != '\n')
			Text = Text + '\n';
		Textlen = Text.size();
		for (j = 0; j < Textlen; j++)
		{
			cur = Text[j];
			if (cur == '\n') ++Linenum;
			if (cur >= 32 && cur <= 126) ++charnum;
			else cur = ' ';
			if (cur >= 'a'&& cur <= 'z' || cur >= 'A'&&cur <= 'Z')
				Received = Received + cur;
			else if (cur >= '0' && cur <= '9')
			{
				if (Received.size() >= 4) Received = Received + cur;
				else Received.resize(0);
			}
			else
			{
				if (Received.empty() == false)
				{
					if (Received.size() < 4)
					{
						Received.resize(0);
						continue;
					}
					mylen = Received.length();
					temp = Received;
					transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
					for (int s = mylen - 1; s >= 0; s--)
					{
						if (isalpha(temp[s]))
						{
							Receivedkey = temp.substr(0, s + 1);
							break;
						}
					}
					++Word[Receivedkey].num;
					Word[Receivedkey].str = min(Word[Receivedkey].str, Received);
					if (Lastword.empty() == false)
					{
						Tempsumkey = Lastword + "_" + Receivedkey;
						++Phrase[Tempsumkey];
					}
					Lastword = Receivedkey;
					Received.resize(0);
				}
			}
		}
	}
	phrasechange(Resultphrase, Phrase, Word);
	Overword(Word, Wordnum);
	vector<string> outline;
	outline.push_back("char_number  " + numtostring(charnum));
	outline.push_back("line_number  " + numtostring(Linenum));
	outline.push_back("word_number  " + numtostring(Wordnum));
	GetToptenword(Word, outline);
//	cout << endl;
	GetToptenPhrase(Resultphrase, outline);
	ofstream outfile("result.txt");
	if (outfile.is_open())
	{
		for (int i = 0; i < outline.size(); i++)
		{
			outfile << outline[i] << endl;
		}
	}
	outfile.close();
	return 0;
}
