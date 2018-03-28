#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <unordered_map>
#include <dirent.h>

using namespace std;


struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};



class CountInFile{
private:
    char path[512];
    unsigned long charNum;
    unsigned long lineNum;
    unsigned long wordNum;
    unordered_map<string, pair<string, int> > singleWordFreq;
    unordered_map<pair<string, string>, int, pairhash> doubleWordFreq;
    void procFile(char p[]);
    void addWord(vector<string> &result, string src);
    void calFreq(vector<string> &result);
    string normalizeString(string a);
    void selectSinW();
    void selectDouW();
    void procDir(char path[]);
public:
    CountInFile(const char p[]){
        strcpy(path,p);
        charNum = 0;
        lineNum = 0;
        wordNum = 0;
    }
    void read();
    void write();
};


void CountInFile::procDir(char p[]){
    DIR* pDir;
    struct dirent* ent;
    char childpath[512];
    pDir=opendir(p);
    memset(childpath,0,sizeof(childpath));
    while((ent=readdir(pDir)) != nullptr) {
        if (ent->d_type & DT_DIR) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            sprintf(childpath, "%s/%s", p, ent->d_name);
            printf("path:%s\n", childpath);
            procDir(childpath);
        }
        else{
            sprintf(childpath, "%s/%s", p, ent->d_name);
            printf("path:%s\n", childpath);
            procFile(childpath);
        }
        memset(childpath,0,sizeof(childpath));
    }
}


void CountInFile::read() {
    regex reg1("^[\\.]{0,2}(\\/[\\w^ ]+)*\\/([\\w^ ]+\\.)+[\\w^ ^.]+$");
    regex reg2("^[\\.]{0,2}(\\/[\\w^ ]+)*\\/?$");
    cmatch cm;

    if(regex_match(path, cm, reg1))
        procFile(path);
    if(regex_match(path, cm, reg2))
        procDir(path);
}


void CountInFile::addWord(vector<string> &result, string src){
    int length = src.size();
    string dst;
    for(int i = 0; i < length - 3;){
        if(!isalpha(src[i])){
            i = i + 1;
            continue;
        }
        if(!isalpha(src[i+1])){
            i = i + 2;
            continue;
        }
        if(!isalpha(src[i+2])){
            i = i + 3;
            continue;
        }
        if(!isalpha(src[i+3])){
            i = i + 4;
            continue;
        }
        if(length - i <= 0){
            return;
        }
        dst = src.substr(i, length);
        result.push_back(dst);
        return;
    }
}


void CountInFile::procFile(char p[]){
    ifstream input;
    try{
        input.open(p);
        if (!input.is_open()) {
            string err = "Cannot open file ";
            err += p;
            err += "!";
            throw runtime_error(err);
        }
    }catch(runtime_error error) {
        cout << error.what();
        return;
    }


    string line, temp;
    unsigned long length = 0;
    vector<string> result;
    while(!input.eof()){
        getline(input, line);
        lineNum++;
        int key = 0;
        length = line.size();
        for(int i = 0; i < length; i++){
            if(line[i] >= 32 && line[i] <=126){
                charNum++;
            }
            if(isdigit(line[i]) || isalpha(line[i])){
                temp += line[i];
                key = 1;
            }
            else{
                addWord(result, temp);
                temp = "";
                key = 0;
            }
        }
        if(key == 1){
            addWord(result, temp);
            temp = "";
        }
    }
    wordNum += result.size();

    calFreq(result);
}


string CountInFile::normalizeString(string a){
    string str = a;
    while(isdigit(str.back())){
        str.erase(str.end() - 1);
    }
    unsigned long length = str.size();
    for(int i = 0; i < length; i++){
        if(str[i] >= 'a'){
            str[i] -= 32;
        }
    }
    return str;
}


void CountInFile::calFreq(vector<string> &result){
    string srcStr2, norStr1, norStr2;
    unsigned long wordNumt = result.size();
    unordered_map<string, pair<string, int> >::iterator its, prev;
    unordered_map<pair<string, string>, int, pairhash>::iterator itd;


    if(wordNumt != 0){
        srcStr2 = result[0];
        norStr2 = normalizeString(srcStr2);
        //cout << srcStr2 << ' ' << norStr2 << endl;
        its = singleWordFreq.find(norStr2);
        if(its == singleWordFreq.end()){
            singleWordFreq[norStr2] = make_pair(srcStr2, 1);
        }
        else{
            its->second.second++;
            if(its->second.first > srcStr2){
                its->second.first = srcStr2;
            }
        }
    }



    for(int i = 1; i < wordNumt; i++){
        norStr1 = norStr2;
        srcStr2 = result[i];
        norStr2 = normalizeString(srcStr2);
        //cout << srcStr2 << ' ' << norStr2 << endl;



        its = singleWordFreq.find(norStr2);
        if(its == singleWordFreq.end()){
            singleWordFreq[norStr2] = make_pair(srcStr2, 1);
        }
        else{
            its->second.second++;
            if(its->second.first > srcStr2){
                its->second.first = srcStr2;
            }
        }


        itd = doubleWordFreq.find(make_pair(norStr1, norStr2));
        if(itd == doubleWordFreq.end()){
            doubleWordFreq[make_pair(norStr1, norStr2)] = 1;
        }
        else{
            itd->second++;
        }
    }
}


void CountInFile::selectSinW(){
    int size = int(singleWordFreq.size());
    int length = min(10, size);
    unordered_map<string, pair<string, int> > temp;
    for(int i = 0; i < length; i++){
        auto key = singleWordFreq.begin();
        for (auto iter = singleWordFreq.begin(); iter != singleWordFreq.end(); iter++){
            if(key->second.second < iter->second.second){
                key = iter;
            }
        }
        cout << key->first << ": " << key->second.first << ' ' << key->second.second << endl;
        temp.insert(*key);
        singleWordFreq.erase(key);
    }
    for (auto iter = temp.begin(); iter != temp.end(); iter++){
        singleWordFreq.insert(*iter);
    }
}


void CountInFile::selectDouW(){
    int size = int(doubleWordFreq.size());
    int length = min(10, size);
    for(int i = 0; i < length; i++){
        auto key = doubleWordFreq.begin();
        for (auto iter = doubleWordFreq.begin(); iter != doubleWordFreq.end(); iter++){
            if(key->second < iter->second){
                key = iter;
            }
        }
        cout << key->first.first << ' ' << key->first.second << ": ";
        cout << singleWordFreq[key->first.first].first << ' ' << singleWordFreq[key->first.second].first << ' ' << key->second << endl;
        doubleWordFreq.erase(key);
    }
}


void CountInFile::write(){

    cout << charNum << ' ' << lineNum << ' ' << wordNum << endl;

    cout << endl;
    selectSinW();
    selectDouW();
}


int main(int argc,char* argv[]) {
    if(argc != 2){
        cout << "Invalid input path!" << endl;
        //return 0;
    }
    const char input[]  = "/Users/xiuyuanc/CLionProjects/wordCounting";

    CountInFile path(input);
    try{
        path.read();
    }catch(invalid_argument error){
        cout << error.what();
        return 0;
    }
    path.write();
    return 0;
}

