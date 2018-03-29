#include<stdio.h>
#include<iostream>
#include<io.h>
#include<stdlib.h>
#include<direct.h>
#include<string.h>
#include<cstdio>
#include<fstream>
#include<math.h>

#define N 200//
#define word_MAX 1024
#define TOP_WORD 10//
#define TOP_WORD_WORD 10

struct handle_chain {
	int change;
	long handle;
	handle_chain* before;
	handle_chain* next;
};
struct word {
	char name[20];
	int num;
	word* next_in_name;
	word* next_in_num;
};
struct word_word {
	char* name1;
	char* name2;
	int num;
	word_word* next_in_name;
	word_word* next_in_num;
};
struct top_word {
	int num;
	word* chain;
};
struct top_word_word {
	int num;
	word_word* chain;
};

word index_word[26][26][26][26] = {};
top_word wordlist[TOP_WORD] = {};


char OLD[word_MAX] = {}, NEW[word_MAX] = {};//
int char_num = 0, word_num = 0, line_num = 0, file_num = 0;
char to_search[N] = {};
long handle;//
handle_chain* head = (handle_chain*)malloc(sizeof(handle_chain)), *work = head;

using namespace std;

void creatnode_handle(int change,long handle) {
	handle_chain* tem = (handle_chain *)malloc(sizeof(handle_chain));
	work->next = tem;
	tem->change = change;
	tem->handle = handle;
	tem->next = NULL;
	tem->before = work;
	work = tem;
	return;
}
//
void next_file(long &handle, _finddata_t &fileinfo) {//
	if (handle == -1)
		return;
	while (1) {
		if (!_findnext(handle, &fileinfo)) {
			if (strcmp(fileinfo.name, ".") && strcmp(fileinfo.name, ".."))
				break;
		}
		else {
			handle = work->handle;//
			work = work->before;
			int length = strlen(to_search);
			char tem[6] = "\\*.*";
			to_search[length - 5 - work->next->change] = '\0';
			free(work->next);
			if (work&&work == head)
				work->next = work;		
			strcat(to_search, tem);
			if (handle == -1)
				break;
		}  
	}
	return;
}
//
void copy(char* dis,char* scr) {
	int i = 0;
	while (dis[i] = scr[i] && i < 20) {
		i++;
	}
	return;
}
//
float cmp_word(char* out_chain, char* in_chain) {//A<a
	int length_out_chain = strlen(out_chain), length_in_chain = strlen(in_chain), i = 0;
	float witch = 0;//
	while (i < length_out_chain && i < length_in_chain) {
		if (out_chain[i] == in_chain[i]) {
			if (fabs(witch)>1 && (out_chain[i] > '9'))
				return 2 * witch;
		}
		else if (out_chain[i] == in_chain[i] + 'a' - 'A'&&in_chain[i] > '9') {
			if (!witch)
				witch = 1;//in_chain
			else if (fabs(witch) > 1)
				return 2 * witch;
		}
		else if (out_chain[i] == in_chain[i] - 'a' + 'A'&&out_chain[i] > '9') {
			if (!witch)
				witch = -1;//out_chain
			else if (fabs(witch) > 1)
				return 2 * witch;
		}
		else if ('0' <= out_chain[i] && out_chain[i] <= '9'&&'0' <= in_chain[i] && in_chain[i] <= '9') {
			if (out_chain[i] < in_chain[i] && fabs(witch) <= 1)
				witch = -1.5;
			else if (out_chain[i] > in_chain[i] && fabs(witch) <= 1)
				witch = 1.5;
		}
		else {
			if (witch) {
				return 2 * witch;//
			}
			else {
				if (out_chain[i] < in_chain[i])
					return -2;//
				else
					return 2;//判  out_chain在in_chain之后
			}
		}
		i++;
	}

	if (!out_chain[i] && !in_chain[i]) {
		return (int)witch;
	}

	if (!out_chain[i]) {
		while (in_chain[i]) {
			if (in_chain[i] > '9')
				return -2;//
			i++;
		}
		if (!witch) {
			return -1;
		}
		else
			return (int)witch;
	}

	else {
		while (out_chain[i]) {
			if (out_chain[i] > '9')
				return 2;//
			i++;
		}
		if (witch) {
			return 1;
		}
		else
			return (int)witch;
	}
	//return witch;
}
//
word* creat_word(char* scr) {
	if (!scr)
		return NULL;
	word* tem = (word*)malloc(sizeof(word));
	if (!tem)
		return NULL;
	//tem->name = (char*)malloc(sizeof(char)*strlen(scr));
	copy(tem->name, scr);
	tem->num = 1;
	tem->next_in_name = NULL;
	tem->next_in_num = NULL;
	return tem;
}
//
void upgrade(word* src) {
	int i = TOP_WORD - 1;//
	while (i >= 0) {
		word* work = wordlist[i].chain;

		if (work&&src->num < work->num) {
			if (work == wordlist[TOP_WORD - 1].chain)
				break;
			else {
				int j = TOP_WORD - 1;
				while (j > i + 1) {
					wordlist[j] = wordlist[j - 1];
					j--;
				}
				if (j == TOP_WORD - 1) {
					src->next_in_num = wordlist[TOP_WORD - 1].chain;
					wordlist[TOP_WORD - 1].chain = src;
					wordlist[TOP_WORD - 1].num++;
				}
				else if (j == TOP_WORD - 2) {
					wordlist[j].chain = src;
					src->next_in_num = NULL;
					wordlist[j].num = 1;
					wordlist[j + 1].num--;
				}
				else {
					word* subwork = wordlist[j + 1].chain, *before_subwork = subwork;
					while (subwork != src) {
						before_subwork = subwork;
						subwork = subwork->next_in_num;
					}
					before_subwork->next_in_num = src->next_in_num;
					wordlist[j].chain->next_in_num = src;
					wordlist[j].num = 1;
					src->next_in_num = NULL;
					wordlist[j + 1].num--;
				}
			}
		}
		else if (work && src->num == work->num) {
			if (work == src) {
				if (!i) {//
					int j = TOP_WORD - 1;
					while (j > i + 1) {
						wordlist[j] = wordlist[j - 1];
						j--;
					}
					wordlist[1].chain = src->next_in_num;
					wordlist[1].num--;
					wordlist[0].num = 1;
					src->next_in_num = NULL;
				}
				else {
					if (src->num == wordlist[i - 1].chain->num) {//
						wordlist[i].chain = src->next_in_num;
						wordlist[i].num--;
						wordlist[i + 1].num++;
						src->next_in_num = wordlist[i + 1].chain;
					}
					else {//
						int j = TOP_WORD - 1;
						if (!src->next_in_num) {
							while (j > i + 1) {
								wordlist[j] = wordlist[j - 1];
								j--;
							}
						}
						if (i < TOP_WORD - 3) {
							wordlist[i + 1].chain = src->next_in_num;
							wordlist[i + 1].num--;
						}
						wordlist[i].num = 1;
						src->next_in_num = NULL;
					}
				}
			}
			else {
				if (i == TOP_WORD - 1) {
					wordlist[i].num++;
					src->next_in_num = wordlist[i].chain;
					wordlist[i].chain = src;
				}
				else {
					word* subwork = wordlist[i + 1].chain;
					if (subwork) {
						while (subwork->next_in_num != src) {
							subwork = subwork->next_in_num;
						}
						subwork->next_in_num = src->next_in_num;
						wordlist[i + 1].num--;
						src->next_in_num = wordlist[i].chain;
						wordlist[i].chain = src;
						wordlist[i].num++;
						break;
					}
					else {
						wordlist[i].num++;
						src->next_in_num = wordlist[i].chain;
						wordlist[i].chain = src;
					}
				}
			}
		}
		else {
			i--;
			if (i < 0) {
				int j = TOP_WORD - 1;
				while (j) {
					wordlist[j] = wordlist[j - 1];
					j--;
				}
				word* subwork = wordlist[0].chain;
				if (!subwork) {
					wordlist[0].num = 1;
					wordlist[0].chain = src;
					src->next_in_num = NULL;
				}
				else {
					while (subwork->next_in_num != src) {
						subwork = subwork->next_in_num;
					}
					subwork->next_in_num = src->next_in_num;
					src->next_in_num = NULL;
					wordlist[0].num = 1;
					wordlist[0].chain = src;
					wordlist[1].num--;
					break;
				}
			}
		}
	}
}
//
void input_word(word &head) {
	if (!head.next_in_name) {
		head.next_in_name = creat_word(NEW);
		//upgrade(head.next_in_name);
		return;
	}
	word* work = &head;
	int cmp;
	while (work->next_in_name) {
		cmp = cmp_word(NEW, work->next_in_name->name);
		cmp = -1;
		if (cmp >= 2) {
			work = work->next_in_name;
		}
		else
			break;
	}
	if (!work->next_in_name)
		work->next_in_name = creat_word(NEW);
	else if (cmp <= -2) {
		word* tem = creat_word(NEW);
		tem->next_in_name = work->next_in_name;
		work->next_in_name = tem;
	}
	else {
		work->next_in_name->num++;
		if (cmp == -1) {
			//work->next_in_name->name=(char*)realloc(work->next_in_name->name, sizeof(char)*strlen(NEW));
			copy(work->next_in_name->name, NEW);
		}
		//upgrade(work->next_in_name);//work->next_in_name
	}
	return;
}
//
int WORD() {//
	int a, b, c, d;
	a = NEW[0];
	b = NEW[1];
	c = NEW[2];
	d = NEW[3];
	a = a > 96 ? (a - 97) : (a - 65);
	b = b > 96 ? (b - 97) : (b - 65);
	c = c > 96 ? (c - 97) : (c - 65);
	d = d > 96 ? (d - 97) : (d - 65);
	input_word(index_word[a][b][c][d]);
	return 0;
}
//
void WORD_WORD() {//
	if (!OLD[0])
		return;

}
int judge_word(char* src) {
	if (strlen(src) < 4)
		return 0;
	if (!((src[0] > 64 && src[0] < 91) || (src[0] > 96 && src[0] < 123)))
		return 0;
	if (!((src[1] > 64 && src[1] < 91) || (src[1] > 96 && src[1] < 123)))
		return 0;
	if (!((src[2] > 64 && src[2] < 91) || (src[2] > 96 && src[2] < 123)))
		return 0;
	if (!((src[3] > 64 && src[3] < 91) || (src[3] > 96 && src[3] < 123)))
		return 0;
	word_num++;
	return 1;
}
//
void calculate(FILE* fp) {
	if (!fp) {
		cout << "打开文件失败！" << endl;
		return;
	}
	//bool nonempty = false;//
	char c;//
	int NEW_work = 0;

	while (1) {//c
		c = fgetc(fp);
		if (c == EOF) {//！！！！！！！！！！！！！！！
			//line_num+=nonempty;//
			line_num++;
			if (judge_word(NEW)) {//
				WORD();
				WORD_WORD();
			}
			memset(OLD, 0, sizeof(char)*word_MAX);
			memset(NEW, 0, sizeof(char)*word_MAX);
			return;
		}

		//
			//nonempty = true;//
		if (c == '\n' || c == '\r') {
			line_num++;//
			//nonempty = false;//
		}
		if ((c > 31 && c < 127))
			char_num++;

		if ((c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && c < 123)) {
			NEW[NEW_work] = c;
			NEW_work++;
			/*if (NEW_work >word_MAX ) {
				cout << "" << endl;
			}*/
		}
		else {//
			NEW_work = 0;
			if (judge_word(NEW)) {//
				WORD();
				WORD_WORD();
				copy(OLD, NEW);//
			}
			memset(NEW, 0, sizeof(char)*word_MAX);
		}
	} 
}
//
void findfile(_finddata_t &fileinfo) {//
	while (handle!=-1) {
		if (!strcmp(".", fileinfo.name) || !strcmp("..", fileinfo.name)) {
			next_file(handle, fileinfo);
			if (fileinfo.attrib & 0x00000020)
				break;
		}
		int a = fileinfo.attrib & 0x00000020;//

		if (a) {
			next_file(handle,fileinfo);
			if (fileinfo.attrib & 0x00000020)
				break;
		}
		else if (!a) {//
			int length = strlen(to_search);
			char tem[6] = "\\*.*";
			to_search[length - 3] = '\0';
			strcat(to_search, fileinfo.name);
			strcat(to_search, tem);
			creatnode_handle(strlen(fileinfo.name), handle);//
			handle = _findfirst(to_search, &fileinfo);
			if (!strcmp(".", fileinfo.name) || !strcmp("..", fileinfo.name)) {
				next_file(handle, fileinfo);
				if (fileinfo.attrib & 0x00000020)
					break;
			}

		}
	}
	return;
}
//
void file_output(char* result) {
	ofstream out(result);
	out << "char_num :" << char_num << endl;
	out << "line_num :" << line_num << endl;
	out << "word_num :" << word_num << endl;
	out << endl;
	out << "the top "<< TOP_WORD <<" frequency of word :" << endl;
	/*
	int i = TOP_WORD, j = 0, k = 0;
	while (i>0) {
		word* work = (wordlist[j]).chain;
		k = wordlist[j].num;
		while (work&&k) {
			out << work->name << "\t" << work->num << endl;
			work = work->next_in_num;
			k--;
		}
		i -= wordlist[j].num;
		j++;
	}
	
	out << endl << endl;
	out << "the top " << TOP_WORD_WORD << " frequency of word :" << endl;
	i = TOP_WORD_WORD, j = 0;
	while (i>0) {
		word_word* work = (word_wordlist[j]).chain;
		k=word_wordlist[j].num;
		while (work&&k) {
			out << work->name << "\t" << work->num << endl;
			work = work->next_in_num;
			k--;
		}
		i -= word_wordlist->num;
		j++;
	}
	out << endl << endl;
	*/
	out.close();
	return;
}
int main(int argc char **argv) {
	
	head->before = head->next = head;
	head->change = 0;
	head->handle = -1;
	int a=0;
	memset(OLD, 0, sizeof(char)*word_MAX);
	memset(NEW, 0, sizeof(char)*word_MAX);
	memset(index_word, 0, sizeof(word)*456976);
	//memset(index_word_word, 0, sizeof(word_word)*456976);
	memset(wordlist, 0, sizeof(top_word)*TOP_WORD);
	//memset(word_wordlist, 0, sizeof(top_word_word)*TOP_WORD_WORD);
	struct _finddata_t fileinfo;//
	//gets(to_search);//！！！！！！！！！！！！！！！！！！！！！！！！
	to_search=argv[1];//！！！！！！！！！！！！！！！！！！！！！
	
	handle = _findfirst(to_search, &fileinfo);
	if (handle == -1)
		return -1;
	while (!strcmp(".", fileinfo.name) || !strcmp("..", fileinfo.name))
		findfile(fileinfo);//
	while (handle != -1) {
		char tem[N];
		strcpy(tem, to_search);
		int length = strlen(tem);
		tem[length - 3] = '\0';
		strcat(tem, fileinfo.name);
		FILE* fp = fopen(tem, "r");
		if (fp) {
			calculate(fp);//
			fclose(fp);
		}
		file_num++;
	//	cout << fileinfo.name << endl;
		findfile(fileinfo);
	}
	char result[13] = "result.txt";
	file_output(result);
	_findclose(handle);
	cout << file_num << endl;
//	system("pause");
	return 0;
}
