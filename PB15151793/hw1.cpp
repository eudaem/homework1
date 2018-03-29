//#include "stdafx.h"
#include<iostream>
#include<vector>
#include <time.h>
#include<string>
#include<unordered_map>
#include <cctype>
#include <algorithm>
#include<queue>
#include <io.h>
#include <fstream>  
#include <sstream>
#include <streambuf>  

#include <string>

using namespace std;


struct strint
{
	string str;
	int fre;
	strint()
	{
		str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";;
		fre = 0;
	}
	strint(string str0, int fre0)
	{
		str = str0;
		fre = fre0;
	}
};

bool operator < (const strint n1, const strint n2)
{
	if (n1.fre > n2.fre)
		return true;
	else if (n1.fre < n2.fre)
		return false;
	else
		return n1.str > n2.str;
}







void GetTop10Phrase(unordered_map<string, int> &Phrasefre)
{
	priority_queue <strint> q;
	int i = 0;
	strint cur;
	strint Maxnow;
	strint output[10];
	int index = 0;
	auto it = Phrasefre.begin();
	for (; i < 10 && it != Phrasefre.end(); ++i, ++it)
		q.push(strint(it->first,it->second));
		
	for (auto it2 = it; it2 != Phrasefre.end(); ++it2)
	{
		Maxnow = q.top();
//		cout << "debuging" << endl;
//		cout << Maxnow.first << Maxnow.second << endl;
		cur = strint(it2->first,it2->second);
		if (cur < Maxnow)
		{
			q.pop();
			q.push(cur);
		}
	}

	while (!q.empty())
	{
		cur = q.top();
		q.pop();
		output[index] = cur;
		++index;
	}
	cout << "Top10" << endl;
	for (index = 9; index >= 0; --index)
		cout << output[index].str << " " << output[index].fre << endl;

}

void GetTop10word(unordered_map<string, strint> Wordfre)
{
	priority_queue <strint> q;
	int i = 0;
	strint cur;
	strint Maxnow;
	strint output[10];
	int index = 0;
	auto it = Wordfre.begin();
	for (; i < 10 && it != Wordfre.end(); ++i, ++it)
		q.push(it->second);

	for (auto it2 = it; it2 != Wordfre.end(); ++it2)
	{
		Maxnow = q.top();
		//		cout << "debuging" << endl;
		//		cout << Maxnow.first << Maxnow.second << endl;
		cur = it2->second;
		if (cur < Maxnow)
		{
			q.pop();
			q.push(cur);
		}
	}

	while (!q.empty())
	{
		cur = q.top();
		q.pop();
		output[index] = cur;
		++index;
	}
	cout << "Top10" << endl;
	for (index = 9; index >= 0; --index)
		cout << output[index].str << " " << output[index].fre << endl;

}




void Updatephrase(unordered_map<string, int> &result, unordered_map<string, int> Phrasefre, unordered_map<string, strint> &Wordfre)
{
	int pos;
	string cur;
	string fir;
	string sec;
	string phrase;
	int len;

	for (auto it = Phrasefre.begin(); it != Phrasefre.end(); it++)
	{
		cur = it->first;
		len = (cur).size();
		pos = cur.find("_");

		fir = cur.substr(0, pos);
		fir = Wordfre[fir].str;

		sec = cur.substr(pos + 1, len);
		sec = Wordfre[sec].str;

		phrase = fir +" " + sec;
		result[phrase] = it->second;
		

	}
}
void Travelword(unordered_map<string, strint> fre, int &Wordnum)
{
	for (auto i = fre.begin(); i != fre.end(); i++)
	{
		//cout << "key  " << i->first << "  " << "val  " << (i->second).str << "  " << "fre   " << (i->second).fre << endl;
		Wordnum = Wordnum + (i->second).fre;
	}
	
}

//void Travelphrase(unordered_map<string, int> fre, int& Phrasenum)
//{
//	for (auto i = fre.begin(); i != fre.end(); i++)
//	{
//		//cout << "key  " << i->first << "  fre   " << i->second << endl;
//		Phrasenum = Phrasenum + i->second;
//	}
//		
//
//}


void GetFilename(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;  //很少用的文件信息读取结构
	string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //比较文件类型是否是文件夹
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
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}




int main()
{
	vector<string> Filename;
	string path ;
	path.append("D:/test1");


	time_t start, stop;
	start = time(NULL);


	GetFilename(path, Filename);
	int Filelen = Filename.size();
	int i;
	ifstream infile;
	int Ascnum = 0;
	int Linenum = 0;
	int Phrasenum = 0;
	int Wordnum = 0;
	int len;
	unordered_map<string, strint> Wordfre;
	unordered_map<string, int> Phrasefre;
	unordered_map<string, int> Resultphrase;

	int j;
	char cur;

	string Lastword;
	string Received;
	string Receivedkey;
	string Tempsum;
	string Tempsumkey;

	int Textlen;
	int Receivedsize;
	string temp;

	for (i = 0; i < Filelen; i++)
	{
		//判断是否为文件夹
		infile.open(Filename[i]);
		if (infile.fail()) continue;
		infile.close();

		//走到这一步说明是文件
		ifstream text(Filename[i]);
		string Text((istreambuf_iterator<char>(text)),istreambuf_iterator<char>());	
		Textlen = Text.size();
		//如果len是0,只会改变行数
		if (Textlen == 0)
		{
			++Linenum;
			continue;
		}
		if (Text[Textlen - 1] != '\n')
		{
			Text = Text + '\n';
			Textlen = Textlen + 1;
		}
			
		
		//cout << "Processing " << Filename[i] << endl;
		


		for (j = 0; j < Textlen; j++)
		{
			//cout << i << endl;
			//cout << text[i] << endl;
			cur = Text[j];

			if (cur == '\n') ++Linenum;

			if (cur >= 32 && cur <= 126) ++Ascnum;
			


			Receivedsize = Received.size();
			if (cur >= 'a'&& cur <= 'z' || cur >= 'A'&&cur <= 'Z')
				Received = Received + cur;
			else if (cur >= '0' && cur <= '9')
			{
				if (Receivedsize >= 4) Received = Received + cur;
				else Received.resize(0);
			}
			else//非数字字母
			{
				if (Receivedsize != 0)
				{
					if (Receivedsize < 4)
					{
						Received.resize(0);
						continue;
					}
							
					
					temp = Received;
					transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
						for (int s = Receivedsize - 1; s >= 0; s--)
						{
							if (isalpha(temp[s]))
							{
								Receivedkey = temp.substr(0, s + 1);
								break;
							}
						}

					






					++Wordfre[Receivedkey].fre;
					Wordfre[Receivedkey].str = min(Wordfre[Receivedkey].str, Received);
					//取ASCLL码值小的
					if (Lastword.empty() == false)
					{
						Tempsumkey = Lastword + "_" + Receivedkey;
						++Phrasefre[Tempsumkey];
					}
					Lastword = Receivedkey;

					Received.resize(0);
				}

			}

		}
		
	}
	Updatephrase(Resultphrase, Phrasefre, Wordfre);
	
	cout <<"Asc字符数目  "<< Ascnum << endl;
	cout<< "行数  " <<Linenum << endl;

	cout << "单词信息" << endl;
	Travelword(Wordfre, Wordnum);
	cout << "单词总数  " <<Wordnum<<endl;

	//cout << "词组" << endl;
	//Travelphrase(Resultphrase);
	//cout << "词组数目  " <<Phrasenum<< endl;

	GetTop10Phrase(Resultphrase);
	GetTop10word(Wordfre);

	stop = time(NULL);
	printf("Use Time:%ld\n", (stop - start));

	system("pause");
}//end main

