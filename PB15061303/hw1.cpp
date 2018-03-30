
#include <fstream>  
#include<string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdio.h>


#include <vector>
#include <sstream>

#include<functional>
#include <time.h>
#include<unordered_map>




using namespace std;



class word_count;

void getAllFiles(string path, vector<string>& files);
int fin_to_s(string &str, vector<string> &files, int i);

class word_classifier {
public:
	string* str;
	//string* temp;//
	string* num_rear;
	int num;
	word_classifier();
	~word_classifier();
	int judge(char c, word_count* word);//
										//void classify(char c);//
	void clear();
	void set(word_count* word);

};

class word_count
{
public:
	//string* temp;//
	string* str;//	
	string* num;//
	int num_rear;//
	int str_count;//
	int flag;//
	int size;//
	string* word;//

	word_count* next_ptr;
	word_count();
	~word_count();
};


class phrase_count {
public:
	word_count* phrase1;
	word_count* phrase2;
	int phr_count;//
	int flag;//
	phrase_count* next_ptr;
	phrase_count();
	~phrase_count();


};


void word_to_word(word_count* word, word_count* word1);
int freq_count(word_count* &arr1, word_count* temp, int flag);
int compare(word_count* word, word_count* word1, int& flag);
void phrase_to_phrase(phrase_count* phrase, phrase_count* phrase1);//
int freq_countP(phrase_count* &arr1, phrase_count* temp, int flag);//
int compareP(phrase_count* phrase, phrase_count* phrase1, int& flag1, int &flag2);//



#ifdef WIN32
#include <io.h>
void getAllFiles(string path, vector<string>& files)
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
					getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
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
#include <dirent.h>
void getAllFiles(string path, vector<string>& files)
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
				getAllFiles(name, files);
			}
		}

	}
	closedir(dir);

}
#endif


int fin_to_s(string &str, vector<string> &files, int i) {
	ifstream infile;
	infile.open(files[i]);
	infile >> str;
	infile.close();
	return 0;
}



void word_to_word(word_count* word, word_count* word1) {
	//
	*(word1->str) = *(word->str);

	*(word1->num) = *(word->num);

	word1->num_rear = word->num_rear;
	//
	word1->str_count = word->str_count;
	//
	word1->flag = word->flag;
	word1->size = word->size;
	*(word1->word) = *(word->word);
	word1->next_ptr = word->next_ptr;

}


word_classifier::word_classifier() {
	//
	num = 0;
	str = new string();
	num_rear = new string();
	//temp = new string();

}

word_classifier::~word_classifier() {
	//
	delete str;
	delete num_rear;


}

void word_classifier::set(word_count* word) {
	//
	string stri;
	//word->temp = temp;
	stri = *str + *num_rear;
	*(word->str) = stri;
	*(word->word) = *str;
	word->str_count = 1;
	word->size = num;
	word->num = num_rear;
	word->num_rear = num - num_rear->size();//
	return;

}
void word_classifier::clear() {
	//
	num = 0;

	str->clear();
	num_rear->clear();

}

int word_classifier::judge(char c, word_count* word) {
	//

	if (c >= 'a'&&c <= 'z') c = c - 32;//
	if (c >= '0'&&c <= '9' || c <= 'Z'&&c >= 'A') {
		//*
		if (c >= '0'&&c <= '9') {
			// **
			if (num < 4) {
				// ***
				clear();
				return 0;
			}
			else {
				//
				num_rear->append(1, c);

				num++;
				return 2;
			}
		}
		else {
			//
			if (num_rear->empty()) {

				str->append(1, c);

				num++;
				return 2;
			}
			else {

				str->append(*num_rear);
				num += num_rear->size();
				str->append(1, c);
				num_rear->clear();
				return 2;
			}
		}
	}
	else {

		if (num < 4) {

			clear();
			return 0;
		}
		else {

			set(word);
			clear();
			return 1;
		}
	}

}

word_count::word_count()
{
	str = new string();
	num = new string();
	word = new string();
	next_ptr = NULL;
	size = 0;
	str_count = 0;
	num_rear = 0;
	flag = 0;
}


word_count::~word_count()
{
	delete str;
}

bool operator==(const word_count& word1, const word_count& word2)
{

	return (*(word1.word) == *(word2.word)) && (word1.num_rear == word2.num_rear);

}

int freq_count(word_count* &arr1, word_count* temp, int flag) {

	word_count* arr = arr1;
	if (arr == NULL) {
		//
		arr = new word_count();
		arr->str_count = -1;
	}

	if (arr->next_ptr == NULL)
	{
		//
		arr->next_ptr = temp;
		flag = 1;
		temp->flag = 1;
	}
	else
	{
		//
		if (temp->flag == 1) return 0;//
		word_count* parent = arr;
		word_count* change = NULL;
		arr = arr->next_ptr;
		int i = 0;//
		int t = temp->str_count;//
		int result = -100;//
		int flag_equal = -1;//
							//

		while (i < 10 && arr->next_ptr != NULL) {
			//
			if (t > arr->str_count) {
				//）    *
				flag_equal = -1;
				if (change == NULL)
					change = parent;//  **
				else {
					//**
					int j = change->str_count - arr->str_count;
					if (j>0) {
						//change  **
						change = parent;

					}
					else if (j == 0) {
						//j
						int k = compare(change, arr, flag);
						if (k == -1) change = parent;
					}
					//else 
				}

			}//end if
			else if (t == arr->str_count) {
				//
				if (change == NULL) {
					//
					result = compare(arr, temp, flag);
					if (result == -1) {
						//
						change = parent;
						flag_equal = 1;//
					}//end if	
					 //result=0,
				}
				else {
					//
					if (flag_equal == 1) {
						//
						result = compare(change, temp, flag);
						if (result == -1) {
							//
							change = parent;
						}//end if	
					}//end if
					 //flag_equal!=1,

				}//end else


			}//end else if


			i++;
			parent = arr;
			arr = arr->next_ptr;

		}//end while

		if (i<10) {
			//
			arr->next_ptr = temp;
			temp->flag = 1;
		}//end if
		else if (i == 10) {
			//10
			if (change != NULL) {
				//change
				temp->next_ptr = change->next_ptr->next_ptr;
				change->next_ptr->flag = 0;
				temp->flag = 1;
				change->next_ptr = temp;
			}//end if
		}//end else if
	}//end else

}//end freq_count

int freq_countP(phrase_count* &arr1, phrase_count* temp, int flag) {

	phrase_count* arr = arr1;
	if (arr == NULL) {

		arr = new phrase_count();
		arr->phr_count = -1;
	}

	if (arr->next_ptr == NULL)
	{

		arr->next_ptr = temp;
		flag = 1;
		temp->flag = 1;
	}
	else
	{

		if (temp->flag == 1) return 0;
		phrase_count* parent = arr;
		phrase_count* change = NULL;
		arr = arr->next_ptr;
		int i = 0;//
		int t = temp->phr_count;//
		int para1, para2 = 0;//
		int result = -100;//
		int flag_equal = -1;

		while (i < 10 && arr->next_ptr != NULL) {
			//
			if (t > arr->phr_count) {
				//    *
				flag_equal = -1;
				if (change == NULL)
					change = parent;//  **
				else {
					//    **
					int j = change->phr_count - arr->phr_count;
					if (j>0) {
						//  **
						change = parent;

					}
					else if (j == 0) {
						//
						int k = compareP(change, arr, para1, para2);
						if (k == -1) change = parent;
					}
					//else 
				}

			}//end if
			else if (t == arr->phr_count) {
				//
				if (change == NULL) {
					//
					result = compareP(arr, temp, para1, para2);
					if (result == -1) {
						//
						change = parent;
						flag_equal = 1;//
					}//end if	
					 //result=0,
				}
				else {
					//
					if (flag_equal == 1) {
						//
						result = compareP(change, temp, para1, para2);
						if (result == -1) {
							//
							change = parent;
						}//end if	
					}//end if

				}//end else


			}//end else if

			 //
			i++;
			parent = arr;
			arr = arr->next_ptr;

		}//end while

		if (i<10) {
			//
			arr->next_ptr = temp;
			temp->flag = 1;
		}//end if
		else if (i == 10) {

			if (change != NULL) {
				//
				temp->next_ptr = change->next_ptr->next_ptr;
				change->next_ptr->flag = 0;
				temp->flag = 1;
				change->next_ptr = temp;
			}//end if
		}//end else if
	}//endelse

}//end freq_count

phrase_count::phrase_count() {
	phrase1 = new word_count();
	phrase2 = new word_count();
	next_ptr = NULL;
	phr_count = 0;

}

phrase_count::~phrase_count() {
	delete phrase1;
	delete phrase2;

}

void phrase_to_phrase(phrase_count* phrase, phrase_count* phrase1) {

	phrase1->flag = phrase->flag;
	phrase1->next_ptr = phrase->next_ptr;
	phrase1->phr_count = phrase->phr_count;
	word_to_word((phrase->phrase1), (phrase1->phrase1));
	word_to_word((phrase->phrase2), (phrase1->phrase2));

}

int compare(word_count* word, word_count* word1, int& flag) {
	//
	int w0 = word->num_rear, w1 = word1->num_rear;
	flag = 0;
	string s0(*(word->str), 0, w0), s1(*(word1->str), 0, w1);
	if (s0 < s1)  return -1;//
	else if (s0 > s1) return 1;//
	else {


		if (word->num < word1->num) flag = -1;
		else if (word->num > word1->num) flag = 1;

		return 0;
	}
}//end compare
int compareP(phrase_count* phrase, phrase_count* phrase1, int& flag1, int &flag2) {
	//
	int s1, s2 = 0;
	s1 = compare(phrase->phrase1, phrase1->phrase1, flag1);
	if (s1<0) {
		return -1;
	}
	else if (s1 > 0) {
		return 1;
	}
	else {
		s2 = compare(phrase->phrase2, phrase1->phrase2, flag2);
		if (s2 < 0) {
			return -1;
		}
		else if (s2 > 0) {
			return 1;
		}
		else {
			return 0;
		}
	}

}//end compareP


int main(int argc, char* argv[])
{
	unordered_map<string, word_count> wordmap;//Hash table for word
	unordered_map<string, phrase_count> phrasemap;//hash table for phrase
	vector<string> files;
	string path;
	if (argv[1] == NULL) {
		
		path.append("c:/result/newsample");
	}
	else {
		path.append(argv[1]);
		
	}

	time_t start, stop;
	start = time(NULL);
	if(path.find(".")!=string::npos){// if the path is a file path
		files.push_back(path);
		   
		}
	else	{        
		getAllFiles(path, files);   //get all file paths  
		
	}

	
	
	int size = files.size();//length of file
	int con = 1;//parameter;
	int line_count = 0;//count of line
	int char_count = 0;//count of character
	int word_all_count = 0;//count of word
	word_classifier classifier_word;//char-word analyzer
	word_count* word_temp = NULL;
	word_count* word_temp1 = NULL;
	word_count* arr = new word_count();
	word_count* word_test = NULL;//using for pointing to word_count in H_table;
	string str_test;//used to store *(word_temp->word)
	int flag = 0; //flag of judging word
	int phrase_all_count = 0;//count of phrase
	phrase_count* phrase_temp = NULL;
	phrase_count* arrp = new phrase_count();
	phrase_count* phrase_test = NULL;//using for pointing to phrase_count in H_table;
	word_temp = new word_count();
	word_temp1 = new word_count();
	phrase_temp = new phrase_count();



	int phr_flag = 0;//fag of phrase,judging if should get phrase


	char c = 0, optr = 0;//optr is a copy of c

	fstream infile;//ptr of file
	for (int i = 0; i < size; i++) {
		//going to all files
		infile.open(files[i]);
		//judge if the path is a folder or document
		if (infile.fail()) continue;// fail,meaning a folder
		else {
			//get the length of file,and store in FileSize
			int begin = infile.tellg();
			int end = begin;
			int FileSize = 0;
			infile.seekg(0, ios_base::end);
			end = infile.tellg();
			infile.seekg(0, ios_base::beg);
			FileSize = end - begin;
			//end of getting file
			if (FileSize != 0) {
				line_count += 1;
				for (int j = 0; j <= FileSize; j++) {
					//operation in each File
					//get a char and count
					infile.get(c);
					if (32 <= c&&c <= 126) char_count = char_count + 1;
					if (c == '\n') line_count = line_count + 1;
					optr = c;
					c = 0;// clear c,avoiding mistakes										
						  //end of counting char and line
					flag = classifier_word.judge(optr, word_temp);
					//judging if the word is ok
					if (flag == 1) {
						//get a word,
						flag = 0;
						//cout << *(word_temp->str) << endl;
						word_all_count += 1;
						str_test = *(word_temp->word);
						if (wordmap.find(str_test) == wordmap.end()) {
							//if don't exist
							word_to_word(word_temp, &wordmap[str_test]);
							word_test = &wordmap[str_test];
							freq_count(arr, word_test, 1);
						}
						else {
							//if exist
							wordmap[str_test].str_count++;
							word_test = &wordmap[str_test];
							//change the rear
							con = (*(word_test->num) > *(word_temp->num));
							if (con == 1) {
								//word_temp has a smaller rear
								*(word_test->num) = *(word_temp->num);
								*(word_test->str) = *(word_test->word) + *(word_test->num);

							}
							freq_count(arr, word_test, 1);

						}
						if (phr_flag == 0) {
							phr_flag = 1;
							word_to_word(word_test, word_temp1);
						}
						else if (phr_flag == 1) {

							phrase_temp->phrase2 = word_temp;
							phrase_temp->phrase1 = word_temp1;
							str_test = *(word_temp1->word) + *(word_temp->word);
							if (phrasemap.find(str_test) == phrasemap.end()) {
								//if don;t exist
								phrase_to_phrase(phrase_temp, &phrasemap[str_test]);
								phrasemap[str_test].phr_count = 1;
								phrase_test = &phrasemap[str_test];
								freq_countP(arrp, phrase_test, 1);
								word_to_word(word_temp, word_temp1);

							}
							else {
								//if exist
								phrasemap[str_test].phr_count++;
								phrase_test = &phrasemap[str_test];
								freq_countP(arrp, phrase_test, 1);
								word_to_word(word_temp, word_temp1);

							}
							if (optr == 0) phr_flag = 0;
						}

					}//end if else 


				}//end for 2
				infile.get(optr);
				if (infile.eof()) infile.close();

			}//end if
		}//end else

	}//end for1
	string dist = "Result.txt";
	ofstream ofn(dist);
	ofn << "The number of character is:" << char_count << endl;
	ofn << "The number of line is:" << line_count << endl;
	ofn << "The number of word is:" << word_all_count << endl;
	word_count* ptr_temp = arr;
	phrase_count* ptr_tempp = arrp;
	word_count* q = arr;
	phrase_count*qp=arrp;
	
	int i = 0,j=0;//
	
	ofn << "The top 10 words is:" << endl;
	
	while (ptr_temp->next_ptr != NULL&&i<10) {
		
		//print the top 10 word
		ptr_temp = ptr_temp->next_ptr;
		ofn << *(ptr_temp->str) +'	';
		ofn << ptr_temp->str_count << endl;
		i++;
	}
	i = 0;
	
	while (ptr_tempp->next_ptr != NULL&&i<10) {
		//print the top 10 phrase
		
		ptr_tempp = ptr_tempp->next_ptr;
		ofn << *(ptr_tempp->phrase1->str) + ' ' + *(ptr_tempp->phrase2->str) + '	';
		ofn << ptr_tempp->phr_count << endl;
		i++;
	}
	stop = time(NULL);
	ofn << "Use Time :"<<stop-start << endl;
	
	infile.close();
	ofn.close();
}//end main
