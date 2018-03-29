#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <ctime>
#include <cstring>
#include <dirent.h>

using namespace std;
long long TotalNum_chars = 0;
long long TotalNum_lines = 0;
long long TotalNum_words = 0;
struct my_word
{
    string sort_word = "zzzzzzzzzzzzzzzzzz";
    size_t appear_count = 0;
};

my_word ten_word[11];
struct my_phrase
{
    string sort_phrase = "zzzzzzzzzzzzzzzzzz";
    size_t appear_count = 0;
};

my_phrase ten_phrase[11];
unordered_map<string, my_word>word_count;
unordered_map<string, size_t>phrase_count;

string transform_word(string raw_word)
{
    size_t len = raw_word.length();
    string simple_word;
    string temp_word = raw_word;
    transform(temp_word.begin(), temp_word.end(), temp_word.begin(), ::tolower);
    bool is_start = false;
    for (int i = (int)len - 1; i >= 0; i--)
    {
        if (isalpha(temp_word[i]))
        {
            is_start = true;
            simple_word = temp_word.substr(0, i + 1);
            break;
        }
    }
    return simple_word;
}

void EnterMap(string last_word, string current_word)
{
    string simple_last_word;
    string simple_current_word;
    size_t len = last_word.length();
    string temp_word = last_word;
    transform(temp_word.begin(), temp_word.end(), temp_word.begin(), ::tolower);
    bool is_start = false;
    for (size_t i = len - 1; i >= 0; i--)
    {
        if (isalpha(temp_word[i]))
        {
            is_start = true;
            simple_last_word = temp_word.substr(0, i + 1);
            break;
        }
    }
    len = current_word.length();
    temp_word = current_word;
    transform(temp_word.begin(), temp_word.end(), temp_word.begin(), ::tolower);
    is_start = false;
    for (size_t i = len - 1; i >= 0; i--)
    {
        if (isalpha(temp_word[i]))
        {
            is_start = true;
            simple_current_word = temp_word.substr(0, i + 1);
            break;
        }
    }
    word_count[simple_current_word].appear_count++;
    if (current_word<word_count[simple_current_word].sort_word)
    {
        word_count[simple_current_word].sort_word = current_word;
    }


    string simple_phrase = simple_last_word + '_' + simple_current_word;
    phrase_count[simple_phrase]++;

}

void NumOfCharsLinesInFile(const string FileLocation)
{//Read the file, count the number of characters, lines, and words, and add it to the global variable. The word is processed and added to the map dictionary.
    //int NumberChars = 0;
    int NumberLines = 1;
    int NumberWords = 0;
    char last_char = ' ';
    char current_char;
    bool wordbegin = false;
    string current_word;
    string last_word;

    long sz;
    FILE * fp = fopen(FileLocation.c_str(), "rb");
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);

    rewind(fp);
    char*buf;
    buf = (char*)malloc(sz * sizeof(char));

    size_t len = fread(buf, sizeof(char), (size_t)sz, fp);
    for(int i = 0;i<len;i++)
    {
        current_char = buf[i];
        if (current_char == '\n') {
            NumberLines++;
        }
        if (current_char < 32 || current_char>126)
        {
            current_char = ' ';
            TotalNum_chars--;
        }
        //判断是否为单词
        if ((!isalpha(last_char)) && (!isdigit(last_char)) && (isalpha(current_char)))
        {
            wordbegin = true;
            current_word = current_char;
        }
        else if (wordbegin)
        {
            if ((isalpha(current_char)) || (isdigit(current_char)))
            {
                //current_word.push_back(current_char);
                current_word.push_back(current_char);
                if (i == len-1) {
                    goto panduan;
                }
            }
            else
            {
                panduan:				wordbegin = false;
                //Determines whether the current current word meets the word requirement: the first four characters are all letters
                if (isalpha(current_word[1]) && isalpha(current_word[2]) && isalpha(current_word[3]))
                {

                    //that current_word meets the requirements
                    NumberWords++;
                    EnterMap(last_word, current_word);
                    last_word = current_word;  //NumberWords++，word，last_word=current_word
                    current_word.clear();

                }
            }
        }
        last_char = current_char;
    }

    free(buf);


    TotalNum_chars += sz;
    TotalNum_lines += NumberLines;
    TotalNum_words += NumberWords;
    fclose(fp);
    fp = nullptr;
}

void listDir(char *path)
{
    DIR              *pDir ;
    struct dirent    *ent  ;
    int               i=0  ;
    char              childpath[512];

    pDir=opendir(path);
    memset(childpath,0,sizeof(childpath));


    while((ent=readdir(pDir))!=nullptr)
    {

        if(ent->d_type & DT_DIR)
        {

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
                continue;

            sprintf(childpath,"%s/%s",path,ent->d_name);
            listDir(childpath);

        }
        else
        {
            sprintf(childpath,"%s/%s",path,ent->d_name);
            NumOfCharsLinesInFile(childpath);

        }
    }

}
bool compare(my_word a, my_word b)
{
    return a.appear_count>b.appear_count;
}

bool phrase_compare(my_phrase a, my_phrase b)
{
    return a.appear_count>b.appear_count;
}

void Getten_word() {

    my_word temporary_word;
    for (const auto &w : word_count)
    {
        ten_word[10] = w.second;
        for (int i = 0; i <= 9; i++)
        {
            if (ten_word[i].appear_count < ten_word[i + 1].appear_count)
            {
                temporary_word = ten_word[i];
                ten_word[i] = ten_word[i + 1];
                ten_word[i + 1] = temporary_word;
            }
        }
    }
    sort(ten_word, ten_word + 10, compare);
}

void Getten_phrase()
{
    my_phrase temporary_phrase;
    for (const auto &w : phrase_count)
    {
        ten_phrase[10].appear_count = w.second;
        ten_phrase[10].sort_phrase = w.first;
        for (int i = 0; i <= 9; i++)
        {
            if (ten_phrase[i].appear_count < ten_phrase[i + 1].appear_count)
            {
                temporary_phrase = ten_phrase[i];
                ten_phrase[i] = ten_phrase[i + 1];
                ten_phrase[i + 1] = temporary_phrase;
            }
        }
    }
    sort(ten_phrase, ten_phrase + 10, phrase_compare);
}

int main(int argc, char *argv[])
{
    //clock_t tStart = clock();
    listDir(argv[1]);
    Getten_word();
    Getten_phrase();
    ofstream result;
    result.open("result.txt");

    result << "char_number :" << TotalNum_chars << endl;
    result << "line_number :" << TotalNum_lines << endl;
    result << "word_number :" << TotalNum_words << endl;

    result <<endl<< "the top ten frequency of word : " << endl;
    for (int i = 0; i < 10; i++)
    {
        result << ten_word[i].sort_word << "  " << ten_word[i].appear_count << endl;

    }

    result <<"\n\n"<< "the top ten frequency of phrase :" << endl;
    for (int i = 0; i < 10; i++)
    {
        string phrase_now = ten_phrase[i].sort_phrase;
        string temp1, temp2;
        unsigned long x = phrase_now.length();
        unsigned long k = phrase_now.find('_');
        string xx = phrase_now.substr(0, k);
        result << word_count[phrase_now.substr(0, k)].sort_word << ' ' << word_count[phrase_now.substr(k + 1, x - k - 1)].sort_word <<" "<< ten_phrase[i].appear_count << endl;
    }
    result.close();
    //printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
    return 0;
}

