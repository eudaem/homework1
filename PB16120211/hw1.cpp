#ifdef  WIN32
#include <io.h>
#endif //  Win32
#ifdef __linux__
#include <dirent.h>  
#include <sys/stat.h>  
#endif // __linux__


//#include <io.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <time.h>
//#include <iomanip>
using namespace std;


class word_time {
public:
	string word;
	int time;
public:
	word_time() {
		this->word = "";
		this->time = 0;
	}
};

class word_word_time : public word_time {
public:
	string word_s;
	word_word_time() {
		this->time = 0;
		this->word = "";
		this->word_s = "";
	}
	void operator=(const word_word_time &another) {
		this->time = another.time;
		this->word = another.word;
		this->word_s = another.word_s;
	}
};

class bi_word {
public:
	string str1;
	string str2;
	bi_word() {
		this->str1 = "";
		this->str2 = "";
	}

	bi_word(const bi_word &another) {
		this->str1 = another.str1;
		this->str2 = another.str2;
	}

	bool operator==(const bi_word &another) {
		if (this->str1 == another.str1&&this->str2 == another.str2)
			return true;
		return false;
	}

	void operator=(const bi_word &another) {
		this->str1 = another.str1;
		this->str2 = another.str2;
	}
};

unordered_map<string, word_time> word_list;
unordered_map<string, word_word_time> bi_word_list;


bool is_letter(char m) {
	if ((m >= 65 && m <= 90) || (m >= 97 && m <= 122))
		return true;
	return false;
}

bool is_fengefu(char m) {
	if ((m >= 65 && m <= 90 )||( m >= 97 && m <= 122 )|| (m >= 48 && m <= 57))
		return false;
	return true;
}


void add_a_word(string word) {
	if (!is_letter(word[0]))
		return;                                                            
	string word_ = word;
	string::iterator it;
	word_time word__time;
	it = word.end();
	it--;
	while (!is_letter(*it)) {
		it--;
	};																		
	word.erase(it + 1, word.end());											
																			
																			
																																					//���itǰ��λ���Ǵ���ĸ��ֱ��
	transform(word.begin(), word.end(), word.begin(), ::toupper);           
																			
	word_list[word].time++;													
	if (word_list[word].word == "" || word_list[word].word.compare(word_)>0) {
		word_list[word].word = word_;
	}															

}


int count_char_sum(string str) {
	return(str.length());
}


void add_a_bi_word(bi_word b_word);



int sum = 0;
string str_temp, str_now;
void insert_into_wordlist(string &line) {
	vector<vector<string>> wordlist_of_a_line_vec;
	vector<bi_word> bi_wordlist_of_a_line;
	bi_word temp;

	int it_last = 0, it1;
	bool flag = false;

	line.append(" ");
	for (; is_fengefu(line[it_last]) && (size_t)it_last<line.length(); it_last++);
	for (int it = it_last; line[it] != '\0' && (size_t)it<line.length(); it++) {
		if (is_fengefu(line[it])) {

			for (it1 = it_last; it1 - it_last < 4 && (size_t)it1<line.length(); it1++) {
				if (!is_letter(line[it1])) {
					flag = true;
					break;
				}
			}
			if (flag == false) {
								
				sum++;
				str_now = line.substr(it_last, it - it_last);
				add_a_word(str_now);
				if (str_temp != "") {
					temp.str1 = str_temp;
					temp.str2 = str_now;
					add_a_bi_word(temp);
				}
				str_temp = str_now;
			}
			

			flag = false;
			it_last = it + 1;
		}

	}
}



vector<word_time> the_most_ten() {
	vector<word_time> most_ten(10);
	unordered_map<string, word_time>::iterator it = word_list.begin();
	while (it != word_list.end()) {
		if (it->second.time > most_ten[9].time) {
			if (it->second.time > most_ten[0].time)
				most_ten.insert(most_ten.begin(), it->second);
			else
				for (int ii = 1; ii <= 9; ii++) {
					if (it->second.time > most_ten[ii].time && it->second.time <= most_ten[ii - 1].time) {
						most_ten.insert(most_ten.begin() + ii, it->second);
						break;
					}
				}
		}
		it++;
	}
	most_ten.erase(most_ten.begin() + 10, most_ten.end());
	return most_ten;
}



void add_a_bi_word(bi_word b_word) {
	if (!is_letter(b_word.str1[0]) || !is_letter(b_word.str2[0]))
		return;                                                           
	bi_word b_word_ = b_word;
	string::iterator it1, it2;
	word_word_time word_word__time;
	it1 = b_word.str1.end();
	it2 = b_word.str2.end();
	it1--; it2--;
	while (!is_letter(*it1)) {
		it1--;
	};																		
	while (!is_letter(*it2)) {
		it2--;
	};
	b_word.str1.erase(it1 + 1, b_word.str1.end());											
	b_word.str2.erase(it2 + 1, b_word.str2.end());

	for (string::iterator itfirst = b_word.str1.begin(); itfirst < b_word.str1.end(); itfirst++) {
		if (*itfirst >= 'a') *itfirst -= 32;
	}
	for (string::iterator itsecond = b_word.str2.begin(); itsecond < b_word.str2.end(); itsecond++) {
		if (*itsecond >= 'a') *itsecond -= 32;
	}
	string temp = b_word.str1 + b_word.str2;
	bi_word_list[temp].time++;													
	if (bi_word_list[temp].word == "" || (bi_word_list[temp].word + bi_word_list[temp].word_s).compare(b_word_.str1 + b_word_.str2)>0) {
		bi_word_list[temp].word = b_word_.str1;
		bi_word_list[temp].word_s = b_word_.str2;
	}															

}







vector<word_word_time> the_most_ten_bi() {
	vector<word_word_time> most_ten_bi(10);
	word_word_time temp;
	unordered_map<string, word_word_time>::iterator it = bi_word_list.begin();
	while (it != bi_word_list.end()) {

		if (it->second.time > most_ten_bi[9].time) {
			if (it->second.time > most_ten_bi[0].time)
				most_ten_bi.insert(most_ten_bi.begin(), it->second);
			else
				for (int ii = 1; ii <= 9; ii++) {
					if (it->second.time > most_ten_bi[ii].time && it->second.time <= most_ten_bi[ii - 1].time) {
						most_ten_bi.insert(most_ten_bi.begin() + ii, it->second);
						break;
					}
				}
		}
		it++;
	}
	most_ten_bi.erase(most_ten_bi.begin() + 10, most_ten_bi.end());
	return most_ten_bi;
}




long sum1 = 0;
int line_sum = 0;
#ifdef WIN32
void DfsFolder(string path, int layer)
{
	_finddata_t file_info;
	string current_path = path + "/*.*"; 
	int handle = _findfirst(current_path.c_str(), &file_info);
	ifstream infile;
	string temp, text;
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{ 
		if (file_info.attrib == _A_SUBDIR)
		{
			int layer_tmp = layer;
			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  
				DfsFolder(path + '/' + file_info.name, layer_tmp + 1);  
		}
		else
		{
			
			infile.open(path + '/' + file_info.name, ios::in);
			
			while (getline(infile, temp)) {
				
				for (string::iterator it = temp.begin(); it<temp.end(); it++) {
					if (*it > 128 || *it < 32)
						
					{
						*it = ' ';
						sum1--;
					}
				}
				sum1 += temp.length();
				line_sum++;
				insert_into_wordlist(temp);
			}
			if (temp == "")line_sum++;
			
			infile.close();
		}
	} while (!_findnext(handle, &file_info));  
											  
	_findclose(handle);
}
#endif // Win32

#ifdef __linux__
void DfsFolder(string lname)
{
	DIR *dir_ptr;
	struct stat infobuf;
	struct dirent *direntp;
	string name, temp;
	ifstream infile;
	string  text="";
	if ((dir_ptr = opendir(lname.c_str())) == NULL)
		perror("can not open");
	else
	{
		while ((direntp = readdir(dir_ptr)) != NULL)
		{
			temp = "";
			name = direntp->d_name;
			if (name == "." || name == "..")
			{
				;
			}
			else
			{
				temp += lname;
				temp += "/";
				temp += name;

				//strcat(temp, lname);  
				//strcat(temp, "/");  
				//strcat(temp, name);  
				if ((stat(temp.c_str(), &infobuf)) == -1)
					printf("#########\n");
				if ((infobuf.st_mode & 0170000) == 0040000)
				{
					//printf("%s",name);  
					//printf("  this is a directory\n");  
					DfsFolder(temp);
				}
				else
				{
					//printf("%s",name);  
					//printf("  this is a file\n");  
					infile.open(temp, ios::in);
					//line_sum++;  
					if(getline(infile,text)){
						for(string::iterator it=text.begin();it<text.end();it++){
							if(*it > 128 || *it < 32)
								//text.erase(it);
								{
									*it=' ';
									sum1--;
								}
						}
						//if(text == "")
						//line_sum++;
						sum1 += text.length();
						line_sum++;
						insert_into_wordlist(text);
					while (getline(infile, text)) {
						//text.append(temp);  
						//cout << temp << endl;  
						//sum1 += (text.length()-1);
						for(string::iterator it=text.begin();it<text.end();it++){
							if(*it > 128 || *it < 32)
								//text.erase(it);
								{
									*it=' ';
									sum1--;
								}
						}
						//if(text == "")
						//line_sum++;
						sum1 += text.length();
						line_sum++;
						insert_into_wordlist(text);
					}}
					else{
						line_sum++;
					}
					if (temp == "") line_sum++;
					//insert_into_wordlist(text);  
					infile.close();
				}
			}
		}
	}
	closedir(dir_ptr);
}
#endif // __linux__


int main(int argc, char * argv[]) {
	//clock_t startTime, endTime;
	//startTime = clock();
	string path = argv[1];
#ifdef WIN32
	DfsFolder(path, 0);
#endif // Win32

#ifdef __linux__
	DfsFolder(path);
#endif // __linux__

	//DfsFolder(path, 0);
	//DfsFolder("E:/Samples", 0);
	ofstream outfile;
	outfile.open("result.out", ios::out);
	//outfile.flags(ios::left);
	outfile << "char_number :" << sum1 << endl;
	outfile << "line_number :" << line_sum << endl;
	outfile << "word_number :" << sum << endl;
	outfile << endl;
	//outfile.open("result.out", ios::out);
	vector<word_word_time> a = the_most_ten_bi();
	outfile << "the top ten frequency of phrase :" << endl;
	for (int ii = 0; ii < 10; ii++)
		outfile << a[ii].word << ' ' << a[ii].word_s <<"  "<< a[ii].time << endl;
	vector<word_time> b = the_most_ten();
	outfile << endl;
	outfile << "the top ten frequency of word :" << endl;
	for (int ii = 0; ii < 10; ii++)
		outfile << b[ii].word <<' '<< b[ii].time << endl;
	outfile.close();
	//endTime = clock();
	//cout << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	return 0;
}

//deleted all the Chinese tags
