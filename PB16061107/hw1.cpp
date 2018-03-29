#include<stdio.h>
#include<iostream>
#include<io.h>
#include<stdlib.h>
#include<direct.h>
#include<string.h>
#include<cstdio>
#include<fstream>
#include<math.h>

#define N 200//绝对地址的长度
#define word_MAX 1024
#define TOP_WORD 10//想要查找前10个单词
#define TOP_WORD_WORD 10//想要查找前10个词组
//文件夹最后
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

word index_word[26][26][26][26] = {};//26个字母，前四个必为字母
//word_word index_word_word[26][26][26][26] = {};//26个字母，前四个必为字母
top_word wordlist[TOP_WORD] = {};
//top_word_word word_wordlist[TOP_WORD_WORD] = {};

char OLD[word_MAX] = {}, NEW[word_MAX] = {};//储存之前的那个单词
int char_num = 0, word_num = 0, line_num = 0, file_num = 0;
char to_search[N] = {};
long handle;//用栈哟 已实现
handle_chain* head = (handle_chain*)malloc(sizeof(handle_chain)), *work = head;

using namespace std;
/*
int judge_file(char* name) {//判断是否可读 0为不可读 1为可读
	if (!name)
		return 0;
	//file_num++;
	int length = strlen(name),now=length;
	while (now) {//其实也许可以不要这个判定条件
		if (*(name + now - 1) == '.')
			break;
		now--;
	}
	if (!now)
		return 0;
	switch (*(name + now)) {//txt cs cpp js java py h php html
	case 't':
		if (now + 3 == length && *(name + now + 1) == 'x'&&*(name + now + 2) == 't')
			return 1;
		else 
			return 0;
	case 'c':
		if (now + 3 == length && *(name + now + 1) == 'p'&&*(name + now + 2) == 'p') {
			return 1;
		}
		else if (now + 2 == length && *(name + now + 1) == 's')
			return 1;
		else
			return 0;
	case 'h':
		if (now + 4 == length && *(name + now + 1) == 't'&&*(name + now + 2) == 'm'&&*(name + now + 3) == 'l') {
			return 1;
		}
		else if (now + 1 == length)
			return 1;
		else
			return 0;
	case 'j':
		if (now + 4 == length && *(name + now + 1) == 'a'&&*(name + now + 2) == 'v'&&*(name + now + 3) == 'a') {
			return 1;
		}
		else if (now + 2 == length && *(name + now + 1) == 's')
			return 1;
		else
			return 0;
	case 'p':
		if (now + 3 == length && *(name + now + 1) == 'h'&&*(name + now + 2) == 'p') {
			return 1;
		}
		else if (now + 2 == length && *(name + now + 1) == 'y')
			return 1;
		else
			return 0;
	default:
		return 0;
	}
	return 0;
}
//判断是否可读 0为不可读 1为可读(tested)
*/
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
//进栈(tested)
void next_file(long &handle, _finddata_t &fileinfo) {//直接找到下一个文件或者文件夹
	if (handle == -1)
		return;
	while (1) {
		if (!_findnext(handle, &fileinfo)) {
			if (strcmp(fileinfo.name, ".") && strcmp(fileinfo.name, ".."))
				break;
		}
		else {
			handle = work->handle;//出栈
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
//找下一个文件或文件夹(tested)
void copy(char* dis,char* scr) {
	int i = 0;
	while (dis[i] = scr[i] && i < 20) {
		i++;
	}
	return;
}
//将NEW赋值给OLD(tested)
float cmp_word(char* out_chain, char* in_chain) {//A<a
	int length_out_chain = strlen(out_chain), length_in_chain = strlen(in_chain), i = 0;
	float witch = 0;//确定保留谁
	while (i < length_out_chain && i < length_in_chain) {
		if (out_chain[i] == in_chain[i]) {
			if (fabs(witch)>1 && (out_chain[i] > '9'))
				return 2 * witch;
		}
		else if (out_chain[i] == in_chain[i] + 'a' - 'A'&&in_chain[i] > '9') {
			if (!witch)
				witch = 1;//保留in_chain
			else if (fabs(witch) > 1)
				return 2 * witch;
		}
		else if (out_chain[i] == in_chain[i] - 'a' + 'A'&&out_chain[i] > '9') {
			if (!witch)
				witch = -1;//保留out_chain
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
				return 2 * witch;//判定一定不是同一个单词
			}
			else {
				if (out_chain[i] < in_chain[i])
					return -2;//判定一定不是同一个单词  out_chain在in_chain之前
				else
					return 2;//判定一定不是同一个单词  out_chain在in_chain之后
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
				return -2;//判定一定不是同一个单词  out_chain在in_chain之前
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
				return 2;//判定一定不是同一个单词  out_chain在in_chain之后
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
//比较俩单词，输出比较情况 0：俩单词一样，保留原来的name 1:保留原来的name -1：保留新的name -2或-3:out_chain在in_chain之前 2或3：out_chain在in_chain之后(tested)
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
//创建word结点
void upgrade(word* src) {
	int i = TOP_WORD - 1;//计数器
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
				if (!i) {//无路可进
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
					if (src->num == wordlist[i - 1].chain->num) {//前进
						wordlist[i].chain = src->next_in_num;
						wordlist[i].num--;
						wordlist[i + 1].num++;
						src->next_in_num = wordlist[i + 1].chain;
					}
					else {//保留
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
//升级
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
		//upgrade(work->next_in_name);//work->next_in_name要进行升级
	}
	return;
}
//按字典顺序插入单词
int WORD() {//单词的种种处理
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
//对单词的种种处理
void WORD_WORD() {//词组的种种处理
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
//判断一个字符串是不是单词 1是 0不是(tested)
void calculate(FILE* fp) {
	if (!fp) {
		cout << "打开文件失败！" << endl;
		return;
	}
	//bool nonempty = false;//空
	char c;//初始化为'1'
	int NEW_work = 0;

	while (1) {//c属于合法符号
		c = fgetc(fp);
		if (c == EOF) {//文末处理！！！！！！！！！！！！！！！
			//line_num+=nonempty;//文件之间的行数增加
			line_num++;
			if (judge_word(NEW)) {//有剩余
				WORD();
				WORD_WORD();
			}
			memset(OLD, 0, sizeof(char)*word_MAX);
			memset(NEW, 0, sizeof(char)*word_MAX);
			return;
		}

		//判断是字符
			//nonempty = true;//非空
		if (c == '\n' || c == '\r') {
			line_num++;//统计行数
			//nonempty = false;//下一行为空
		}
		if ((c > 31 && c < 127))
			char_num++;

		if ((c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && c < 123)) {
			NEW[NEW_work] = c;
			NEW_work++;
			/*if (NEW_work >word_MAX ) {
				cout << "出现超长单词" << endl;
			}*/
		}
		else {//出现分隔符
			NEW_work = 0;
			if (judge_word(NEW)) {//有更新
				WORD();
				WORD_WORD();
				copy(OLD, NEW);//词组的传递
			}
			memset(NEW, 0, sizeof(char)*word_MAX);
		}
	} 
}
//统计单词，词组
void findfile(_finddata_t &fileinfo) {//找到下一个文件!
	while (handle!=-1) {
		if (!strcmp(".", fileinfo.name) || !strcmp("..", fileinfo.name)) {
			next_file(handle, fileinfo);
			if (fileinfo.attrib & 0x00000020)
				break;
		}
		int a = fileinfo.attrib & 0x00000020;//文件>0，文件夹==0

		if (a) {
			next_file(handle,fileinfo);
			if (fileinfo.attrib & 0x00000020)
				break;
		}
		else if (!a) {//可以去掉！
			int length = strlen(to_search);
			char tem[6] = "\\*.*";
			to_search[length - 3] = '\0';
			strcat(to_search, fileinfo.name);
			strcat(to_search, tem);
			creatnode_handle(strlen(fileinfo.name), handle);//进栈
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
//找到下一个文件(tested)
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
int main() {
	//const char *to_search =  "E:\\vs\\软工作业1\\test and result\\test and result\\newsample\\docs\\csharp\\*.*" ;
	//读文件用栈
	//E:\vs\软工作业1\test\test\newsample\*.*
	//E:\vs\软工作业1\test and result\test and result\*.*
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
	struct _finddata_t fileinfo;//初始化
	//gets(to_search);//真·程序！！！！！！！！！！！！！！！！！！！！！！！！
	cin >> to_search;//假·程序！！！！！！！！！！！！！！！！！！！！！！！！
	
	handle = _findfirst(to_search, &fileinfo);
	if (handle == -1)
		return -1;
	while (!strcmp(".", fileinfo.name) || !strcmp("..", fileinfo.name))
		findfile(fileinfo);//一定得输出文件
	while (handle != -1) {
		char tem[N];
		strcpy(tem, to_search);
		int length = strlen(tem);
		tem[length - 3] = '\0';
		strcat(tem, fileinfo.name);
		FILE* fp = fopen(tem, "r");
		if (fp) {
			calculate(fp);//统计开始
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
