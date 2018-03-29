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

class W_Counter;

void getAllFiles(string path, vector<string>& files);
int fin_to_s(string &str, vector<string> &files, int i);

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

class W_fenlei {
public:
	string* str;
	//string* temp;//
	string* num_rear;
	int num;
	W_fenlei();
	~W_fenlei();
	int judge(char c, W_Counter* word);//
										//void classify(char c);//
	void clear();
	void give(W_Counter* word);

};

class W_Counter
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

	W_Counter* next_ptr;
	W_Counter();
	~W_Counter();
};


class Ph_Counter {
public:
	W_Counter* phrase1;
	W_Counter* phrase2;
	int phr_count;//
	int flag;//
	Ph_Counter* next_ptr;
	Ph_Counter();
	~Ph_Counter();

};



int fin_to_s(string &str, vector<string> &files, int i) {
	ifstream infile;
	infile.open(files[i]);
	infile >> str;
	infile.close();
	return 0;
}



void W_W(W_Counter* word, W_Counter* word1) {
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


W_fenlei::W_fenlei() {
	//
	num = 0;
	str = new string();
	num_rear = new string();
	//temp = new string();

}

W_fenlei::~W_fenlei() {
	//
	delete str;
	delete num_rear;


}

void W_fenlei::give(W_Counter* word) {
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
void W_fenlei::clear() {
	//
	num = 0;

	str->clear();
	num_rear->clear();

}

int phraseCmp(char *phrase1, char *phrase2) {
	char *cp1, *cp2;
	for (cp1 = phrase1, cp2 = phrase2; *cp1!=' '&&*cp1++ == *cp2++;);
	if (*cp1 == ' ') return -1;
	else if (*cp2 == ' ') return 1;
	else {
		for (; *cp1++ == *cp2++ ;);
		if (*cp1 == '\0') return -1;
		else return 1;
	}
}

int W_fenlei::judge(char c, W_Counter* word) {
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

			give(word);
			clear();
			return 1;
		}
	}

}

W_Counter::W_Counter()
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


W_Counter::~W_Counter()
{
	delete str;
}

bool operator==(const W_Counter& word1, const W_Counter& word2)
{

	return (*(word1.word) == *(word2.word)) && (word1.num_rear == word2.num_rear);

}

int weihu_W(W_Counter* &arr1, W_Counter* temp, int flag) {

	W_Counter* arr = arr1;
	if (arr == NULL) {
		//
		arr = new W_Counter();
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
		W_Counter* parent = arr;
		W_Counter* change = NULL;
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
						int k = judge(change, arr, flag);
						if (k == -1) change = parent;
					}
					//else 
				}

			}//end if
			else if (t == arr->str_count) {
				//
				if (change == NULL) {
					//
					result = judge(arr, temp, flag);
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
						result = judge(change, temp, flag);
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

}//end weihu_W

int weihu_Ph(Ph_Counter* &arr1, Ph_Counter* temp, int flag) {

	Ph_Counter* arr = arr1;
	if (arr == NULL) {

		arr = new Ph_Counter();
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
		Ph_Counter* parent = arr;
		Ph_Counter* change = NULL;
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
						int k = judgeP(change, arr, para1, para2);
						if (k == -1) change = parent;
					}
					//else 
				}

			}//end if
			else if (t == arr->phr_count) {
				//
				if (change == NULL) {
					//
					result = judgeP(arr, temp, para1, para2);
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
						result = judgeP(change, temp, para1, para2);
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

}//end weihu_W

Ph_Counter::Ph_Counter() {
	phrase1 = new W_Counter();
	phrase2 = new W_Counter();
	next_ptr = NULL;
	phr_count = 0;

}

Ph_Counter::~Ph_Counter() {
	delete phrase1;
	delete phrase2;

}

void PH_PH(Ph_Counter* phrase, Ph_Counter* phrase1) {

	phrase1->flag = phrase->flag;
	phrase1->next_ptr = phrase->next_ptr;
	phrase1->phr_count = phrase->phr_count;
	W_W((phrase->phrase1), (phrase1->phrase1));
	W_W((phrase->phrase2), (phrase1->phrase2));

}

bool isNeededFile(char *filename) {
	char *cp;
	for (cp = filename; *cp; cp++);
	for (; *cp != '.'; cp--) if (*cp == '\\') return false;
	return false;
}

int judge(W_Counter* word, W_Counter* word1, int& flag) {
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
}//end judge
int judgeP(Ph_Counter* phrase, Ph_Counter* phrase1, int& flag1, int &flag2) {
	//
	int s1, s2 = 0;
	s1 = judge(phrase->phrase1, phrase1->phrase1, flag1);
	if (s1<0) {
		return -1;
	}
	else if (s1 > 0) {
		return 1;
	}
	else {
		s2 = judge(phrase->phrase2, phrase1->phrase2, flag2);
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

}//end judgeP


int main(int argc, char* argv[])
{
	unordered_map<string, W_Counter> wordmap;//Hash table for word
	unordered_map<string, Ph_Counter> phrasemap;//hash table for phrase
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
	if (path.find(".") != string::npos) {// if the path is a file path
		files.push_back(path);

	}
	else {
		getAllFiles(path, files);   //get all file paths  

	}



	int size = files.size();//length of file
	int con = 1;//parameter;
	int line_count = 0;//count of line
	int char_count = 0;//count of character
	int word_all_count = 0;//count of word
	W_fenlei classifier_word;//char-word analyzer
	W_Counter* temp_word = NULL;
	W_Counter* temp_word1 = NULL;
	W_Counter* arr = new W_Counter();
	W_Counter* testing_W = NULL;//using for pointing to W_Counter in H_table;
	string str_test;//used to store *(temp_word->word)
	int flag = 0; //flag of judging word
	int phrase_all_count = 0;//count of phrase
	Ph_Counter* temp_phrase = NULL;
	Ph_Counter* arrp = new Ph_Counter();
	Ph_Counter* testing_PH = NULL;//using for pointing to Ph_Counter in H_table;
	temp_word = new W_Counter();
	temp_word1 = new W_Counter();
	temp_phrase = new Ph_Counter();



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
				char_count--;
				word_all_count++;
				for (int j = 0; j < FileSize; j++) {
					//operation in each File
					//get a char and count
					infile.get(c);
					if (32 <= c&&c <= 126) char_count = char_count + 1;
					if (c == '\n') line_count = line_count + 1;
					optr = c;
					c = 0;// clear c,avoiding mistakes										
						  //end of counting char and line
					flag = classifier_word.judge(optr, temp_word);
					//judging if the word is ok
					if (flag == 1) {
						//get a word,
						flag = 0;
						//cout << *(temp_word->str) << endl;
						word_all_count += 1;
						str_test = *(temp_word->word);
						if (wordmap.find(str_test) == wordmap.end()) {
							//if don't exist
							W_W(temp_word, &wordmap[str_test]);
							testing_W = &wordmap[str_test];
							weihu_W(arr, testing_W, 1);
						}
						else {
							//if exist
							wordmap[str_test].str_count++;
							testing_W = &wordmap[str_test];
							//change the rear
							con = (*(testing_W->num) > *(temp_word->num));
							if (con == 1) {
								//temp_word has a smaller rear
								*(testing_W->num) = *(temp_word->num);
								*(testing_W->str) = *(testing_W->word) + *(testing_W->num);

							}
							weihu_W(arr, testing_W, 1);

						}
						if (phr_flag == 0) {
							phr_flag = 1;
							W_W(testing_W, temp_word1);
						}
						else if (phr_flag == 1) {

							temp_phrase->phrase2 = temp_word;
							temp_phrase->phrase1 = temp_word1;
							str_test = *(temp_word1->word) + *(temp_word->word);
							if (phrasemap.find(str_test) == phrasemap.end()) {
								//if don;t exist
								PH_PH(temp_phrase, &phrasemap[str_test]);
								phrasemap[str_test].phr_count = 1;
								testing_PH = &phrasemap[str_test];
								weihu_Ph(arrp, testing_PH, 1);
								W_W(temp_word, temp_word1);

							}
							else {
								//if exist
								phrasemap[str_test].phr_count++;
								testing_PH = &phrasemap[str_test];
								weihu_Ph(arrp, testing_PH, 1);
								W_W(temp_word, temp_word1);

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
	W_Counter* ptr_temp = arr;
	Ph_Counter* ptr_tempp = arrp;
	W_Counter* q = arr;
	Ph_Counter*qp = arrp;

	int i = 0, j = 0;//

	ofn << "The top 10 words is:" << endl;

	while (ptr_temp->next_ptr != NULL&&i<10) {

		//print the top 10 word
		ptr_temp = ptr_temp->next_ptr;
		ofn << *(ptr_temp->str) + '	';
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
	ofn << "Use Time :" << stop - start << endl;

	infile.close();
	ofn.close();
}//end main
