
#include <ctime>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <dirent.h>
#include <cstring>

#define MAX_PATH 100
#define MOSTNUM  10

clock_t a = clock();

using namespace std;

typedef struct
{
    string value;
    int appearNum = 0;
}wordInfo;
typedef struct
{
    string Aword;
    string Bword;
    int appearNum = 0;
}phraselink;

typedef unordered_map<string, wordInfo> wMap;
typedef unordered_map<string, int> npMap;

long charNum = 0;
long lineNum = 0;
long wordNum = 0;
wMap wordsDic;
npMap phraseDic;
wordInfo Mwords[MOSTNUM+1];
phraselink Mphrases[MOSTNUM+1];


void addWord(string &word, string &word_pre, string &word_r, string &word_pre_r)
{

    int wordlen = word.length() - 1;
    int pfixlen = 0;
    string postfix, phraseKey;
    wMap::iterator tempit;
    for (int i = wordlen;; i--)
    {
        if (word[i]<'0' || word[i]>'9')
        {
            pfixlen = wordlen - i;
        
            break;
        }
    }
    postfix = word.substr(wordlen - pfixlen+1);
    word = word.substr(0, wordlen - pfixlen+1);
    wordsDic[word].appearNum++;

    if (wordsDic[word].value.empty() || wordsDic[word].value>word_r)
        wordsDic[word].value = word_r;

    if (!word.empty() && !word_pre.empty()) {
        phraseKey = word_pre;
        phraseKey.push_back('-');
        phraseKey += word;
        phraseDic[phraseKey]++;
    }
}

bool sortWords()
{
    wordInfo temp;

    for (const auto &w : wordsDic) {
        Mwords[10].appearNum = w.second.appearNum;
        Mwords[10].value = w.second.value;
        for (int i = 0; i <= 9; i++) {
            if (Mwords[i].appearNum < Mwords[i + 1].appearNum) {
                temp = Mwords[i];
                Mwords[i] = Mwords[i + 1];
                Mwords[i + 1] = temp;
            }
        }
    }

    return true;
}
bool sortPhrase()
{
    phraselink temp;

    for (const auto &w : phraseDic) {
        Mphrases[10].appearNum = w.second;
        Mphrases[10].Aword = w.first;
        for (int i = 0; i <= 9; i++) {
            if (Mphrases[i].appearNum < Mphrases[i + 1].appearNum) {
                temp = Mphrases[i];
                Mphrases[i] = Mphrases[i + 1];
                Mphrases[i + 1] = temp;
            }
        }

    }
    return true;
}
bool isWord(string word)
{
    for (int i = 0; i < 4; i++)
    {
        if (word[i]<'a' || word[i]>'z')
            return false;
    }
    return true;
}

string tolower(string & str)
{
    int len = str.length();
    for (int i = 0; i < len; i++)
        if (str[i] >= 'A'&&str[i] <= 'Z')
            str[i] = str[i] + 32;
    return str;
}

void operate(char * path)
{
    string word_Breal, word_Areal, word_A, word_B;

    char ch;
    size_t sz;
    FILE*fp;

    {
        /*suffix = files[i].substr(files[i].find_last_of('.') + 1);
        auto itstr = find(mysuffix.begin(), mysuffix.end(), suffix);
        if (itstr == mysuffix.end())
        continue;*/

        //file_test.open(files[i], ios::in);
        //fp =fopen(files[i].c_str(),"r");
        fp=fopen(path, "rb");
        if (fp==NULL)
        {
            cout << "cant open file " << path << endl;
            return;
        }
        fseek(fp, 0L, SEEK_END);
        sz = ftell(fp);
        rewind(fp);
        char*buf;
        buf = new char[sz];
        int len = fread(buf, sizeof(char), sz, fp);
        if (len) {
            //lineNum++;
            //the file in linux seem to have a '\n' added at the end automatically
            //comment out this in linux
        }
        else
            return;

        for (int i = 0; i < len; i++)
        {
            ch = buf[i];
            if (ch >= 32 && ch <= 126)
                charNum++;
            //cout << ch;
            if (ch >= 'A'&&ch <= 'Z')
            {
                word_Breal.push_back(ch);
                word_B.push_back(ch + 32);
            }
            else if ((ch >= 'a'&&ch <= 'z') || (ch <= '9'&&ch >= '0'))
            {
                word_B.push_back(ch);
                word_Breal.push_back(ch);
            }
            else
            {
                if (ch == '\n')
                    lineNum++;	
                //transform(word_B.begin(), word_B.end(), word_B.begin(), _tolower_l);
                //transform(word_B.begin(), word_B.end(), word_B.begin(), std::tolower);
                //word_B = tolower(word_B);
                if (isWord(word_B))
                {
                    wordNum++;
                    addWord(word_B, word_A, word_Breal, word_Areal);
                    word_A = word_B;
                    word_Areal = word_Breal;
                }
                word_B.clear();
                word_Breal.clear();
            }

        }

        if (isWord(word_B))
        {
            wordNum++;
            addWord(word_B, word_A, word_Breal, word_Areal);
            word_A = word_B;
            word_Areal = word_Breal;
        }
        word_B.clear();
        word_Breal.clear();

        delete[]buf;
        if (fp)
        {
            if (fclose(fp))
            {
                cout << path << " is not closed " << endl;
            }
        }
        /*	file_test.close();
        file_test.clear();*/
    }
}
void listDir(char *path) 
{
    string pastr=path;
    DIR *pDir; 
    struct dirent *ent;
    int i = 0;
    char childpath[512]; 
    pDir = opendir(path); 
    memset(childpath, 0, sizeof(childpath)); 
    while ((ent = readdir(pDir)) != NULL)
    {
        if (ent->d_type & DT_DIR)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
               
                continue;
            sprintf(childpath, "%s/%s", path, ent->d_name);
           
            listDir(childpath);
          
        }
        else
           
        {
         
            sprintf(childpath, "%s/%s", path, ent->d_name);
          
            operate(childpath);
        }
    }
}



int main(int argc, char** argv)
{
    char filePath[] = "/home/rui-chun/test//ssss";
    vector<string> files;
    string suffix;
    if (argc < 2)
        listDir(filePath);
    else
    {
        listDir(argv[1]);
    }



    sortWords();
    sortPhrase();

    //out File
    ofstream fileout("result.txt");
    if(!fileout) {
        cout << "cant open result.txt" << endl;
        return 0;
    }
    fileout << "characters:" << charNum << endl;
    fileout << "words:" << wordNum << endl;
    fileout << "lines:" << lineNum << endl;

    for (int i = 0; i < MOSTNUM; i++)
    {
        if (Mwords[i].appearNum == 0)continue;
        fileout << "<" << Mwords[i].value << ">:" << Mwords[i].appearNum << endl;
    }
    for (int i = 0; i < MOSTNUM; i++)
    {
        if (Mphrases[i].appearNum == 0)continue;
        string ph=Mphrases[i].Aword;
        int pl=ph.find('-');
        Mphrases[i].Aword=ph.substr(0,pl);
        Mphrases[i].Bword=ph.substr(pl+1);
        fileout << "<" << wordsDic[Mphrases[i].Aword].value << "-" << wordsDic[Mphrases[i].Bword].value << ">:" << Mphrases[i].appearNum << endl;
    }
    fileout.close();

    clock_t b = clock();
    //cout << (double)(b - a) / CLOCKS_PER_SEC << "s" << endl;
	//time compute
    return 0;
}
