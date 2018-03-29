#include <iostream>
#include <string>
#include <fstream>
//#include <io.h>
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

my_word ten_word[11];/* NOLINT */
struct my_phrase
{
    string sort_phrase = "zzzzzzzzzzzzzzzzzz";
    size_t appear_count = 0;
};

my_phrase ten_phrase[11];/* NOLINT */
unordered_map<string, my_word>word_count;
unordered_map<string, size_t>phrase_count;

string transform_word(string raw_word)
{
    size_t len = raw_word.length();
    string simple_word;
    //string temp_word = raw_word;
    transform(raw_word.begin(), raw_word.end(), raw_word.begin(), ::tolower);
    bool is_start = false;
    for (int i = (int)len - 1; i >= 0; i--)
    {
        if (isalpha(raw_word[i]))
        {
            is_start = true;
            simple_word = raw_word.substr(0, (unsigned long)i + 1);
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
    //string temp_word = last_word;
    transform(last_word.begin(), last_word.end(), last_word.begin(), ::tolower);
    bool is_start = false;
    for (size_t i = len - 1; i >= 0; i--)
    {
        if (isalpha(last_word[i]))
        {
            is_start = true;
            simple_last_word = last_word.substr(0, i + 1);
            break;
        }
    }
    len = current_word.length();
    //temp_word = current_word;
    transform(current_word.begin(), current_word.end(), current_word.begin(), ::tolower);
    is_start = false;
    for (size_t i = len - 1; i >= 0; i--)
    {
        if (isalpha(current_word[i]))
        {
            is_start = true;
            simple_current_word = current_word.substr(0, i + 1);
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
    bool word_begin = false;
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
    //if (len) {
    //	NumberLines++;
    //}

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
            word_begin = true;
            current_word = current_char;
        }
        else if (word_begin)
        {
            if ((isalpha(current_char)) || (isdigit(current_char)))
            {
                //current_word.push_back(current_char);
                current_word.push_back(current_char);
                if (i == len-1) {
                    goto exam;
                }
            }
            else
            {
                exam:				word_begin = false;
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
    DIR              *pDir ;  //定义一个DIR类的指针
    struct dirent    *ent  ;   //定义一个结构体 dirent的指针，dirent结构体见上
    int               i=0  ;
    char              childpath[512];  //定义一个字符数组，用来存放读取的路径

    pDir=opendir(path); //  opendir方法打开path目录，并将地址付给pDir指针
    memset(childpath,0,sizeof(childpath)); //将字符数组childpath的数组元素全部置零


    while((ent=readdir(pDir))!= nullptr) //读取pDir打开的目录，并赋值给ent, 同时判断是否目录为空，不为空则执行循环体
    {

        if(ent->d_type & DT_DIR)  //读取 打开目录的文件类型 并与 DT_DIR进行位与运算操作，即如果读取的d_type类型为DT_DIR (=4 表示读取的为目录)
        {

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
//如果读取的d_name为 . 或者.. 表示读取的是当前目录符和上一目录符, 用contiue跳过，不进行下面的输出
                continue;

            sprintf(childpath,"%s/%s",path,ent->d_name);  //如果非. ..则将 路径 和 文件名d_name 付给childpath, 并在下一行prinf输出
            //printf("path:%s\n",childpath);

            listDir(childpath);  //递归读取下层的字目录内容， 因为是递归，所以从外往里逐次输出所有目录（路径+目录名），
//然后才在else中由内往外逐次输出所有文件名

        }
        else  //如果读取的d_type类型不是 DT_DIR, 即读取的不是目录，而是文件，则直接输出 d_name, 即输出文件名
        {
            //cout<<ent->d_name<<endl;  //cout<<childpath<<"/"<<ent->d_name<<endl;  输出文件名 带上了目录
            sprintf(childpath,"%s/%s",path,ent->d_name);
            NumOfCharsLinesInFile(childpath);

        }
    }

}
bool compare(my_word a, my_word b)
{
    return a.appear_count>b.appear_count;   //升序排列
}

bool phrase_compare(my_phrase a, my_phrase b)
{
    return a.appear_count>b.appear_count;   //升序排列
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
    clock_t tStart = clock();
    char LOCATION[1050] = "/home/ruizhao/Documents/newsample";
    listDir(LOCATION);
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
    printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
    return 0;
}
