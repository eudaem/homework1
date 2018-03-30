
#include "iostream"
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <fstream>  
#include <algorithm>  
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
void GetAllFiles(string path, vector<string>& files);
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

using namespace std;

int getcharnum(string filename);

void display(unordered_map<string, double> mmp, string str); 

void displaydic(unordered_map<string, string> mmp, string str);

void Result(); 

void BubbleSort();	

void listFiles(const char * dir);	

int total_char_num = 0;

int total_line_num = 0;

int total_word_num = 0;

int tmp_num = 0;

int i = 0;	

struct dictionary         
{
	string topword;
	int num = 0;
}*d, *p;

unordered_map<string, double> dic;
unordered_map<string, string> diction;
unordered_map<string, double> phr;
unordered_map<string, string> phrtion;



int main(int argc, char *argv[])
{
	clock_t start = clock();
	int num;
	//listFiles(argv[1]);
	vector<string> files;
	string filepath = argv[1];
	//listFiles("C:\\newsample");
	if (filepath.find(".") == string::npos)
	{
		GetAllFiles(filepath, files);
		int size = files.size();
		for (int g = 0; g < size; g++)
		{
			getcharnum(files[g]);
		}
	}
	else 
	{
		getcharnum(filepath);
	}
	BubbleSort();
	clock_t finish = clock();
	Result();
	double consumeTime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\nTime:" << consumeTime << "second！" << endl;
	system("pause");
	return 0;
}

void Result()
{
	string bestdic;
	string str;
	ofstream outfile("Result.txt");
	if (!outfile)
	{
		cout << "Failed to create file...\n";
		exit(0);
	}
	outfile << "char_number:" << total_char_num << endl;
	outfile << "line_number:" << total_line_num << endl;
	outfile << "word_number:" << total_word_num << endl;
	outfile << "\nthe top ten frequency of word :\n";
	for (int k = 0; k < 10; k++)
	{
		str = (d + k)->topword;
		unordered_map<string, string>::const_iterator got = diction.find(str);
		bestdic = got->second;
		outfile << bestdic << "\t" << (d + k)->num << endl;
	}
	outfile << "\n\nthe top ten frequency of phrase :\n";
	for (int r = 0; r < 10; r++)
	{
		str = (p + r)->topword;
		unordered_map<string, string>::const_iterator gwt = phrtion.find(str);
		bestdic = gwt->second;
		outfile << bestdic << "\t" << (p + r)->num << endl;
	}
	outfile.close();
}

void BubbleSort()
{
	d = new dictionary[10];
	p = new dictionary[10];
	if (d == NULL)
	{
		cout << "Error: out of memory." << endl;
		exit(1);
	}
	if (p == NULL)
	{
		cout << "Error: out of memory." << endl;
		exit(1);
	}
	int k;
	for (auto& x : dic)
	{
		if (x.second < (d + 9)->num)
			continue;
		for (k = 8; k >= 0; k--)
		{
			if (x.second >(d + k)->num)
			{
				(d + k + 1)->num = (d + k)->num;
				(d + k + 1)->topword = (d + k)->topword;
				if (k == 0)
				{
					(d + k)->num = x.second;
					(d + k)->topword = x.first;
				}
			}
			else
			{
				(d + k + 1)->num = x.second;
				(d + k + 1)->topword = x.first;
				break;
			}
		}
	}
	for (auto& x : phr)
	{
		if (x.second < (p + 9)->num)
			continue;
		for (k = 8; k >= 0; k--)
		{
			if (x.second >(p + k)->num)
			{
				(p + k + 1)->num = (p + k)->num;
				(p + k + 1)->topword = (p + k)->topword;
				if (k == 0)
				{
					(p + k)->num = x.second;
					(p + k)->topword = x.first;
				}
			}
			else
			{
				(p + k + 1)->num = x.second;
				(p + k + 1)->topword = x.first;
				break;
			}
		}
	}
}

void display(unordered_map<string, double> mmp, string str)
{
	cout << str << endl;
	for (auto& x : mmp)
		cout << x.first << ": " << "\t" << x.second << endl;
	cout << endl;
}

void displaydic(unordered_map<string, string> mmp, string str)
{
	cout << str << endl;
	for (auto& x : mmp)
		cout << x.first << ": " << "\t" << x.second << endl;
	cout << endl;
}

string trans(string str)
{
	int len = str.length();
	for (int e = 0; e < len; e++)
	{
		(str[e]) = toupper(str[e]);
	}
	return str;
}

int getcharnum(string filename)
{
	fstream openbychar;
	openbychar.open(filename, ios::in);
	//FILE *fp = NULL;
	string buf;
	string bigbuf;
	string buff;
	string bigbuff;
	int p = 0;
	int j = 0;
	int m; 
	int flag = 1;
	char c;
	int num = 0;
	if (openbychar.fail()) 
	{
		return 0; 
	}
	c = openbychar.get();
	while (c != EOF)
	{
		if (c == '\n')
		{
			total_line_num++;
		}
		if (c > 31)
			if (c < 127)
				num++;
		if (c >= 65 && c <= 90 || c >= 97 && c <= 122)
		{
			buf += c;
			j++;
			m = j;
			flag = 0;
		}
		else if (!(c >= 65 && c <= 90 || c >= 97 && c <= 122) && flag == 0)
		{
			if (j > 3)
			{
				if (!(c >= 48 && c <= 57))   
				{
					buf.assign(buf, 0, m);
					if (p == 0)
					{
						buff = buf;  
						buff += " ";
						p = 1;
					}
					else
					{
						buff += buf;
						bigbuff = trans(buff);
						unordered_map<string, string>::const_iterator gat = phrtion.find(bigbuff);
						if (gat == phrtion.end())
						{
							pair<string, string> mybuf(bigbuff, buff);
							phrtion.insert(mybuf);
							i++;
						}
						else
						{
							string temps = gat->second;
							if (buff < temps)
							{
								phrtion.at(bigbuff) = buff;
							}
						}
						unordered_map<string, double>::const_iterator got = phr.find(bigbuff);
						if (got == phr.end())
						{
							pair<string, double> mybuf(bigbuff, 1);
							phr.insert(mybuf);
						}
						else
						{
							int tempc = got->second + 1;
							phr.at(bigbuff) = tempc;
						}
						buff.clear();
						bigbuff.clear();
						buff = buf;  
						buff += " ";
					}
					bigbuf = trans(buf);
					unordered_map<string, string>::const_iterator get = diction.find(bigbuf);
					if (get == diction.end())
					{
						pair<string, string> mybuf(bigbuf, buf);
						diction.insert(mybuf);
						i++;
					}
					else
					{
						string temps = get->second;
						if (buf < temps)
						{
							diction.at(bigbuf) = buf;
						}
					}
					//displaydic(diction, "show me :");
					unordered_map<string, double>::const_iterator got = dic.find(bigbuf);
					if (got == dic.end())
					{
						pair<string, double> mybuf(bigbuf, 1);
						dic.insert(mybuf);
						i++;
					}
					else
					{
						int tempc = got->second + 1;
						dic.at(bigbuf) = tempc;
					}
					total_word_num++;
					j = 0;
					buf.clear();
					bigbuf.clear();
					flag = 1;
				}
				else
				{
					buf += c;
					j++;
					flag = 0;
				}
			}
			else
			{
				j = 0;
				buf.clear();
				flag = 1;
			}
		}
		c = openbychar.get();
	}
	openbychar.close();
	total_line_num++;
	total_char_num += num;
	return num;
}

