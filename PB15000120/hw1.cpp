/**
 * Count character numbers, line numbers, words(specially defined) numbers.
 * Count word frequency and double word phrase frequency.
 * Process a file or directory and write result in ./result.txt.
 * Created by Xiuyuan Chen on 29 Mar. 2018.
 * Copyright © 2018 Xiuyuan Chen. All rights reserved.
 * Just a joke, please feel free to copy and write.
 */

#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <unordered_map>
#include <dirent.h>
using namespace std;
//Hash function for a pair<a,b>
struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};


class FileWordCounter{
private:
    char path[512];
    unsigned long charNum;//total character number
    unsigned long lineNum;//total line number
    unsigned long wordNum;//total word number
    unordered_map<string, pair<string, int> > singleWordFreq;//stores (standard word, <word for display, word frequency>)
    unordered_map<pair<string, string>, int, pairhash> doubleWordFreq;//stores (<standard word1, word2>, phrase frequency)

    void procFile(char p[]);
    void procDir(char path[]);
    void addWord(vector<string> &result, string src);
    void calFreq(vector<string> &result);
    string normStr(string a);

    void writeSinW();
    void writeDouW();
public:
    FileWordCounter(const char p[]){
        strcpy(path,p);
        charNum = 0;
        lineNum = 0;
        wordNum = 0;
    }
    void read();
    void write();
};

//use regex to judge path a file or a directory
void FileWordCounter::read() {
    regex reg1("^[\\.]{0,2}(\\/[\\w-]+)*\\/([\\w-]+\\.)+[\\w-]+$");
    regex reg2("^[\\.]{0,2}(\\/[\\w-]+)*\\/?$");
    cmatch cm;
    if(regex_match(path, cm, reg1)){
        procFile(path);
        //cout << "File" << endl;
    }
    else if(regex_match(path, cm, reg2)) {
        procDir(path);
        //cout << "Folder" << endl;
    }
    else{
        cout << "ERROR: No such file or directory" << endl;
    }
}

//write file
void FileWordCounter::write(){
    ofstream outf("result.txt");
    outf << "char_number: " << charNum << endl;
    outf << "line_number: " << lineNum << endl;
    outf << "word_number: " << wordNum << endl;
    outf.close();
    writeSinW();//write top 10 words
    writeDouW();//write top 10 phrases
}

//traverse directory in recursion
void FileWordCounter::procDir(char p[]){
    DIR* pDir;
    struct dirent* ent;
    char childpath[512];
    pDir=opendir(p);
    memset(childpath,0,sizeof(childpath));
    while((ent=readdir(pDir)) != nullptr) {
        if (ent->d_type & DT_DIR) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            sprintf(childpath, "%s/%s", p, ent->d_name);//combine together
            //printf("path:%s\n", childpath);
            procDir(childpath);
        }
        else{
            sprintf(childpath, "%s/%s", p, ent->d_name);//combine together
            //printf("path:%s\n", childpath);
            procFile(childpath);
        }
        memset(childpath,0,sizeof(childpath));
    }
}

//process a single file
void FileWordCounter::procFile(char p[]){
    ifstream input;
    input.open(p);
    if (!input.is_open()) {
        cout << "Cannot open file " << p << endl;
        return;
    }

    string line, temp;
    unsigned long length = 0;
    vector<string> result;//result stores all words in this file
    int key;
    while(!input.eof()){
        getline(input, line);//read line by line
        lineNum++;//line_number + 1
        key = 0;
        length = line.size();
        for(int i = 0; i < length; i++){
            if(line[i] >= 32 && line[i] <=126){//char_number + 1
                charNum++;
            }
            if(isdigit(line[i]) || isalpha(line[i])){//to judge whether it is a word element
                temp += line[i];
                key = 1;
            }
            else{
                if(temp.size() > 3){
                    addWord(result, temp);//judge whether temp is a word and add it to vector(result)
                }
                temp = "";
                key = 0;
            }
        }
        if(key == 1){//to deal with the last word in a line
            if(temp.size() > 3){
                addWord(result, temp);
            }
            temp = "";
        }
    }//use getline so we don't have to do line_number++ at the end of a file
    wordNum += result.size();

    calFreq(result);
}

//judge whether src is a word and add it to vector(result)
void FileWordCounter::addWord(vector<string> &result, string src){
    int length = src.size();
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
        result.push_back(src.substr(i, length));
        return;
    }
}

//process all words in a file
void FileWordCounter::calFreq(vector<string> &result){
    string srcStr2, norStr1, norStr2;//norStr2 means normalized string for string_2
    unsigned long wordNumt = result.size();
    unordered_map<string, pair<string, int> >::iterator its, prev;
    unordered_map<pair<string, string>, int, pairhash>::iterator itd;
    //process the first word
    if(wordNumt != 0){
        srcStr2 = result[0];
        norStr2 = normStr(srcStr2);//normalize string_2
        its = singleWordFreq.find(norStr2);
        if(its == singleWordFreq.end()){//if the word is not stored
            singleWordFreq[norStr2] = make_pair(srcStr2, 1);//store it
        }
        else{//update word frequency and word for display
            its->second.second++;
            if(its->second.first > srcStr2){
                its->second.first = srcStr2;
            }
        }
    }
    //process the following word
    for(int i = 1; i < wordNumt; i++){
        norStr1 = norStr2;//copy string_2 to string_1
        srcStr2 = result[i];
        norStr2 = normStr(srcStr2);
        //same as above
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
        //process phrase(norStr1, norStr2), similar as above
        itd = doubleWordFreq.find(make_pair(norStr1, norStr2));
        if(itd == doubleWordFreq.end()){
            doubleWordFreq[make_pair(norStr1, norStr2)] = 1;
        }
        else{
            itd->second++;
        }
    }
}

//normalize a string, to delete the digits at the end and make all letters capitals
string FileWordCounter::normStr(string a){
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

//write top 10 words
void FileWordCounter::writeSinW(){
    int size = int(singleWordFreq.size());
    int length = min(10, size);
    unordered_map<string, pair<string, int> > temp;//to store top 10 words
    ofstream outf("result.txt",ios::app);
    outf << endl << "the top ten frequency of word: " << endl;
    for(int i = 0; i < length; i++){//select top 10
        auto key = singleWordFreq.begin();
        for (auto iter = singleWordFreq.begin(); iter != singleWordFreq.end(); iter++){
            if(key->second.second < iter->second.second){
                key = iter;
            }
        }
        outf << key->second.first << "\t\t" << key->second.second << endl;//write in file
        temp.insert(*key);//copy in temp
        singleWordFreq.erase(key);//and then delete(for select use)
    }
    for (auto iter = temp.begin(); iter != temp.end(); iter++){
        singleWordFreq.insert(*iter);//insert top 10 words back
    }
    outf.close();
}

//write top 10 phrases, similar as above
void FileWordCounter::writeDouW(){
    int size = int(doubleWordFreq.size());
    int length = min(10, size);
    ofstream outf("result.txt",ios::app);
    outf << endl << "the top ten frequency of phrase: " << endl;
    for(int i = 0; i < length; i++){
        auto key = doubleWordFreq.begin();
        for (auto iter = doubleWordFreq.begin(); iter != doubleWordFreq.end(); iter++){
            if(key->second < iter->second){
                key = iter;
            }
        }
        outf << singleWordFreq[key->first.first].first << ' ' << singleWordFreq[key->first.second].first << "\t\t" << key->second << endl;
        doubleWordFreq.erase(key);
    }
}


int main(int argc,char* argv[]) {
    if(argc <= 1){
        cout << "ERROR: No such file or directory" << endl;
        return -1;
    }
    FileWordCounter path(argv[1]);
    path.read();
    path.write();
    return 0;
}
