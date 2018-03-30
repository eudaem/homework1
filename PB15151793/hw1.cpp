#include<iostream>
#include<vector>
#include <time.h>
#include<string>
#include<unordered_map>
#include <cctype>
#include <algorithm>
#include<queue>
#include <fstream>  
#include <sstream>
#include <streambuf>  
#include <string>


#define _CRT_SECURE_NO_WARNINGS


using namespace std;


struct strint
{
	string str;
	int fre;
	strint()
	{
		str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
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







void GetTop10Phrase(unordered_map<string, int> &Phrasefre,FILE *fout)
{
	priority_queue <strint> q;
	strint cur;
	strint Maxnow;
	strint output[10];
	int i = 0;
	int index = 0;
	auto it = Phrasefre.begin();
	for (; i < 10 && it != Phrasefre.end(); ++i, ++it)
		q.push(strint(it->first, it->second));

	for (auto it2 = it; it2 != Phrasefre.end(); ++it2)
	{
		Maxnow = q.top();
		cur = strint(it2->first, it2->second);
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
	cout << "Top10 Phrase" << endl;
	for (index = 9; index >= 0; --index)
	    fprintf(fout, "%s :%d\n", output[index].str.c_str(), output[index].fre);

}

void GetTop10word(unordered_map<string, strint>& Wordfre, FILE* &fout)
{
	priority_queue <strint> q;
	strint cur;
	strint Maxnow;
	strint output[10];
	int index = 0;
	int i = 0;
	auto it = Wordfre.begin();
	for (; i < 10 && it != Wordfre.end(); ++i, ++it)
		q.push(it->second);

	for (auto it2 = it; it2 != Wordfre.end(); ++it2)
	{
		Maxnow = q.top();
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
		fprintf(fout, "%s: %d\n", output[index].str.c_str(), output[index].fre);

}




void Updatephrase(unordered_map<string, int> &result, unordered_map<string, int> &Phrasefre, unordered_map<string, strint> &Wordfre)
{
	int pos;
	int len;
	string cur;
	string fir;
	string sec;
	

	for (auto it = Phrasefre.begin(); it != Phrasefre.end(); it++)
	{
		cur = it->first;
		len = cur.size();
		pos = cur.find("_");

		fir = cur.substr(0, pos);
		fir = Wordfre[fir].str;
		sec = cur.substr(pos + 1, len);
		sec = Wordfre[sec].str;

		fir += " " + sec;
		result[fir] = it->second;
	}
}
void Travelword(unordered_map<string, strint> &fre, int &Wordnum)
{
	for (auto i = fre.begin(); i != fre.end(); i++)
		Wordnum += (i->second).fre;

}

#ifdef WIN32
#include <io.h>
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
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetFilename(p.assign(path).append("\\").append(fileinfo.name), files);
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
#include<dirent.h>
void GetFilename(string path, vector<string>& files)
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
			//cout << name << endl;
			if (p->d_type == 4) {
				GetFilename(name, files);
			}
		}

	}
	closedir(dir);

}
#endif



int main(int argc, char* argv[])
{
	//file
	vector<string> Filename;
        
	string path="";
        
	if (argv[1] == NULL) {
		path.append("/media/chencan/D45CEB905CEB6C24/test");
              
	}
	else {
		path.append(argv[1]);
               
	}
        

        if (path.find('.') != string::npos) {
 Filename.push_back(path);
}
        else
        {
	GetFilename(path, Filename);

        }
	int Filelen = Filename.size();
	

	//time
	time_t start, stop;
	start = time(NULL);
        
      
	//common
	int i;
	int j;
	int Ascnum = 0;
	int Linenum = 0;
	int Phrasenum = 0;
	int Wordnum = 0;
	char cur;
	unordered_map<string, strint> Wordfre;
	unordered_map<string, int> Phrasefre;
	unordered_map<string, int> Resultphrase;	
	string Lastword;
	string Received;
	string Receivedkey;
	string Tempsum;
	string Tempsumkey;

	int Textlen;
	int Receivedsize;
	string temp;
        fstream infile;
	for (i = 0; i < Filelen; ++i)
	{
		
		infile.open(Filename[i]);
		if (infile.fail()) continue;
		infile.close();
                
		
		ifstream text(Filename[i]);
		string Text((istreambuf_iterator<char>(text)), istreambuf_iterator<char>());
		Textlen = Text.size();
                		
               
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



		for (j = Textlen; j >=1; --j)
		{
			//cout << i << endl;
			//cout << text[i] << endl;
			cur = Text[Textlen - j];

			if (cur == '\n') ++Linenum;

			if (cur >= 32 && cur <= 126) ++Ascnum;



			Receivedsize = Received.size();
			if (cur >= 'a'&& cur <= 'z' || cur >= 'A'&&cur <= 'Z')
				Received += cur;
			else if (cur >= '0' && cur <= '9')
			{
				if (Receivedsize >= 4) Received += cur;
				else Received.resize(0);
			}
			else
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
					if (Wordfre[Receivedkey].str > Received)
						Wordfre[Receivedkey].str = Received;
					//Wordfre[Receivedkey].str = min(Wordfre[Receivedkey].str, Received);
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

	cout << "Asc  " << Ascnum << endl;
	cout << "line  " << Linenum << endl;

	cout << "word" << endl;
	Travelword(Wordfre, Wordnum);
	cout << "sum " << Wordnum << endl;

	

	
	

	//write
	FILE *fout;
	
	if ((fout = fopen("result.txt", "w")) == NULL) {
		printf("cannot open this file result.txt\n");
		exit(0);
	}

	fprintf(fout, "char_number : %d\n", Ascnum);
	fprintf(fout, "line_number : %d\n", Linenum);
	fprintf(fout, "word_number : %d\n", Wordnum);
	fprintf(fout, "\n");

	GetTop10word(Wordfre,fout);
	fprintf(fout, "\n");

	GetTop10Phrase(Resultphrase,fout);
	fclose(fout);
	stop = time(NULL);
	cout<<"Use Time"<<(stop - start)<<endl;

	system("pause");
}//end main

