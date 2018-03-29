
#include "word_count.h"
#include "octree.h"
#include <time.h>
#include<unordered_map>

using namespace std;



int main(int argc, char* argv[])
{
	unordered_map<string, word_count> wordmap;//Hash table for word
	unordered_map<string, phrase_count> phrasemap;//hash table for phrase
	vector<string> files;
	string path;
	if (argv[1] == NULL) {
		cout<<"wrong file name!"<<endl;
    return 1;
	}
	else {
		path.append(argv[1]);
	}
	
	time_t start, stop;
	start = time(NULL);
	getAllFiles(path, files);

	
	ifstream infile;//ptr of file
	int size = files.size();//length of file
	int con = 1;//parameter;
	int line_count=0;//count of line
	int char_count = 0;//count of character
	int word_all_count = 0;//count of word
	word_classifier classifier_word;//char-word analyzer
	word_count* word_temp=NULL;
	word_count* word_temp1 = NULL;
	word_count* arr=new word_count();
	word_count* word_test = NULL;//using for pointing to word_count in H_table;
	string str_test ;//used to store *(word_temp->word)
	int flag=0; //flag of judging word
	int phrase_all_count = 0;//count of phrase
	phrase_count* phrase_temp = NULL;
	phrase_count* arrp = new phrase_count();
	phrase_count* phrase_test = NULL;//using for pointing to phrase_count in H_table;
	word_temp = new word_count();
	word_temp1 = new word_count();
	phrase_temp = new phrase_count();
	
	
	
	int phr_flag = 0;//fag of phrase,judging if should get phrase
	
	
	char c=0,optr=0;//optr is a copy of c


	for (int i = 0; i < size; i++) {
		//going to all files
		infile.open(files[i]);
		//judge if the path is a folder or document
		if (infile.fail()) continue;// fail,meaning a folder
		else{
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
				for (int j = 0; j < FileSize;j++) {
					//operation in each File
					//get a char and count
					infile.get(c);
					if (32 <= c&&c <= 126) char_count =char_count + 1;
					if (c == '\n') line_count = line_count + 1;
					optr = c;
					c = 0;// clear c,avoiding mistakes										
					//end of counting char and line
					flag=classifier_word.judge(optr,word_temp);
					//judging if the word is ok
					if (flag == 1) {
						//get a word,
						flag = 0;
						//cout << *(word_temp->str) << endl;
						word_all_count += 1;
						str_test = *(word_temp->word);
						if (wordmap.find(str_test) == wordmap.end()) {
							//if don't exist
							word_to_word(word_temp,&wordmap[str_test]);
							word_test=&wordmap[str_test];
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
  
	cout << "The number of character is:"<<char_count << endl;
	cout << "The number of line is:" << line_count << endl;
	cout << "The number of word is:" << word_all_count << endl;
	word_count* ptr_temp = arr;
	phrase_count* ptr_tempp = arrp;
	int i = 0;
	while (ptr_temp->next_ptr!=NULL&&i<10) {
		//print the top 10 word
		ptr_temp = ptr_temp->next_ptr;
		cout << *(ptr_temp->str) +'	';
		cout << ptr_temp->str_count << endl;
		i++;
	}
	i = 0;
	while (ptr_tempp->next_ptr != NULL&&i<10) {
		//print the top 10 phrase
		ptr_tempp = ptr_tempp->next_ptr;
		cout << *(ptr_tempp->phrase1->str)+' '+*(ptr_tempp->phrase2->str)+'	';
		i++;
	}
	stop = time(NULL);
	printf("Use Time:%ld\n", (stop - start));
	system("pause");
}//end main
