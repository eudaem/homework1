#include <iostream>
#include <assert.h>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <time.h>
#include <dirent.h>

using namespace std;


class CharCounter {

private:
	int char_num;
	int line_num;

public:
	CharCounter();
	void count(char c);
	void count_eof();
	int get_char_num();
	int get_line_num();

};


const int WORD_SIZE		= 1024;
const int PHRASE_SIZE		= WORD_SIZE * 2 + 5;
const int WORD_HASH_SIZE	= 150000;
const int PHRASE_HASH_SIZE	= 2000000;


struct wnode {
	std::string format = "";
	std::string exp = "";
	int count = 0;
	wnode *next = NULL;

	wnode(const std::string& f, const std::string& e) {
		format = f;
		exp = e;
	}
	~wnode() { if (next) next->~wnode(); }
};


struct pnode {
	const wnode *w1;
	const wnode *w2;
	std::string format;
	int count = 0;
	pnode *next = NULL;

	pnode(const wnode *pw1, const wnode *pw2, const std::string& f) {
		w1 = pw1;
		w2 = pw2;
		format = f;
	}
	~pnode() { if (next) next->~pnode(); }
};


class WordPool {

private:
	std::string exp_to_format(const std::string& e);
	void word_sort_insert(const wnode * p);
	void word_sort();
	void phrase_sort_insert(const pnode * p);
	void phrase_sort();

public:
	WordPool();
	~WordPool();
	wnode* add_word(const char* exp);
	void add_phrase(const wnode* pw1, const wnode* pw2);
	int get_max_word(int i, std::string &e);
	int get_max_phrase(int i, std::string &e);
};


class WordCounter {

private:

	int num = 0;
	wnode *pre = NULL;
	wnode *now = NULL;

	char exp[WORD_SIZE + 2] = "";
	int exp_len = 0;
	bool overflow = false;
	bool pre_divisor = true;

	WordPool pool;

public:
	void count(char c);
	void count_eof();
	int get_word_num() { return num; }
	int get_max_word(int i, std::string& e);
	int get_max_phrase(int j, std::string& e);

};


class Counter {

private:
	CharCounter cc;
	WordCounter wc;

public:
	bool count(std::string filepath);
	int get_char_num();
	int get_line_num();
	int get_word_num();
	int get_max_word(int i, std::string& e);
	int get_max_phrase(int j, std::string& e);

};


Counter counter;


void traverse_dir(char *path) {
	DIR *pDir;
	struct dirent *ent;
	char filepath[512];

	pDir = opendir(path);
	memset(filepath, 0, sizeof(filepath));

	while((ent = readdir(pDir)) != NULL) {
		if(ent->d_type & DT_DIR) {
			if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0) continue;
			sprintf(filepath, "%s/%s", path, ent->d_name);
			traverse_dir(filepath);
		}
		else {
			sprintf(filepath, "%s/%s", path, ent->d_name);
			counter.count(filepath);
		}
	}
}


int main(int argc, char **argv) {
	if (argc > 1) traverse_dir(argv[1]);
	else return 1;
	
	ofstream result("result.txt", ios::ate | ios::out);
	result << "char_number :" << counter.get_char_num() << endl;
	result << "line_number :" << counter.get_line_num() << endl;
	result << "word_number :" << counter.get_word_num() << endl;
	result << "\nthe top ten frequency of word :\n";
	for (int i = 0; i < 10; i++) {
		int num;
		string s;
		num = counter.get_max_word(i, s);
		if (num <= 0) break;
		result << s << '\t' << num << endl;
	}
	result << "\nthe top ten frequency of phrase :\n";
	for (int i = 0; i < 10; i++) {
		int num;
		string s;
		num = counter.get_max_phrase(i, s);
		if (num <= 0) break;
		result << s << "\t" << num << endl;
	}
	result.close();

	return 0;
}


wnode *wtable[WORD_HASH_SIZE] = { NULL };
const wnode *wmax[10] = { NULL };

pnode *ptable[PHRASE_HASH_SIZE] = { NULL };
const pnode *pmax[10] = { NULL };


bool pool_occupied = false;
bool word_sorted = false;
bool phrase_sorted = false;


int elfhash(const string& s) {
	unsigned long h = 0;
	unsigned long g;
	for (int i = 0; i < s.size(); i++) {
		h = (h << 4) + s[i];
		g = h & 0xF0000000L;
		if (g) {
			h ^= g >> 24;
			h &= ~g;
		}
	}
	return (h & 0x7FFFFFFF);
}


int jshash(const string& s) {
	int hash = 0;

	for (int i = 0; i < s.size(); i++) {
		hash ^= (hash << 5) + (int)s[i] + (hash >> 2);
	}

	return (hash & 0x7FFFFFFF);
}


int fnvhash(const string& s) {
	int fnvprime = 0x811C9DC5;
	int hash = 0;

	for (int i = 0; i < s.size(); i++) {
		hash *= fnvprime;
		hash ^= (int)s[i];
	}

	return hash & 0x7FFFFFFF;
}


int bknrhash(const string& s){
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	for (int i = 0; i < s.size(); i++) {
		hash = hash * seed + s[i];
	}

	return (hash & 0x7FFFFFFF);
}


int (*my_hash)(const string& s) = jshash;


WordPool::WordPool() {
	assert(pool_occupied == false);
	for (int i = 0; i < WORD_HASH_SIZE; i++) {
		wtable[i] = NULL;
	}
	for (int i = 0; i < PHRASE_HASH_SIZE; i++) {
		ptable[i] = NULL;
	}
	for (int i = 0; i < 10; i++) {
		wmax[i] = NULL;
		pmax[i] = NULL;
	}
	pool_occupied = true;
}


WordPool::~WordPool() {
	for (int i = 0; i < WORD_HASH_SIZE; i++) {
		if (wtable[i]) delete wtable[i];
	}
	for (int i = 0; i < PHRASE_HASH_SIZE; i++) {
		if (ptable[i]) delete ptable[i];
	}
	pool_occupied = false;
	word_sorted = false;
	phrase_sorted = false;
}


string WordPool::exp_to_format(const string& e) {
	string f = "";
	int j = e.size() - 1;
	while (isdigit(e[j])) j--;
	for (int i = 0; i <= j; i++) f += tolower(e[i]);
	return f;
}


wnode * WordPool::add_word(const char * e) {
	if (strlen(e) > WORD_SIZE) return NULL;

	string exp = e;
	string format = exp_to_format(exp);

	int key = my_hash(format) % WORD_HASH_SIZE;
	wnode * p = wtable[key];
	if (p) {
		while (p->format != format) {
			if (p->next) p = p->next;
			else {
				p = p->next = new wnode(format, exp);
			}
		}
	}
	else {
		p = wtable[key] = new wnode(format, exp);
	}
	p->count++;
	if (p->exp > exp) p->exp = exp;

	return p;
}


void WordPool::add_phrase(const wnode * pw1, const wnode * pw2) {
	if (pw1 == NULL || pw2 == NULL) return;

	string f = pw1->format;
	f += " ";
	f += pw2->format;

	int key = my_hash(f) % PHRASE_HASH_SIZE;
	pnode * p = ptable[key];
	if (p) {
		while (p->format != f) {
			if (p->next) p = p->next;
			else {
				p = p->next = new pnode(pw1, pw2, f);
			}
		}
	}
	else {
		p = ptable[key] = new pnode(pw1, pw2, f);
	}
	p->count++;
}


void WordPool::word_sort_insert(const wnode * p) {
	for (int i = 0; i < 10; i++) {
		if (wmax[i] == NULL || wmax[i] == p) {
			wmax[i] = p;
			break;
		}
		else {
			int ci = wmax[i]->count;
			int cp = p->count;
			if ((ci < cp) || (ci == cp && p->format < wmax[i]->format)) {
				int j;
				for (j = i; j < 10 - 1; j++) if (wmax[j] == p) break;
				for (; j > i; j--) {
					wmax[j] = wmax[j - 1];
				}
				wmax[i] = p;
				break;
			}
		}
	}
}


void WordPool::word_sort() {
	const wnode * p;
	for (int i = 0; i < WORD_HASH_SIZE; i++) {
		for (p = wtable[i]; p; p = p->next) word_sort_insert(p);
	}
}


int WordPool::get_max_word(int i, string &e){
	if (word_sorted == false) {
		word_sort();
		word_sorted = true;
	}
	if (i < 0 || i >= 10 || wmax[i] == NULL) {
		return -1;
	}
	else {
		e = wmax[i]->exp;
		return wmax[i]->count;
	}
}


void WordPool::phrase_sort_insert(const pnode * p) {
	for (int i = 0; i < 10; i++) {
		if (pmax[i] == NULL || pmax[i] == p) {
			pmax[i] = p;
			return;
		}
		else {
			int ci = pmax[i]->count;
			int cp = p->count;
			if ((ci < cp) || (ci == cp && p->format < pmax[i]->format)) {
				int j;
				for (j = i; j < 10 - 1; j++) if (pmax[j] == p) break;
				for (; j > i; j--) {
					pmax[j] = pmax[j - 1];
				}
				pmax[i] = p;
				return;
			}
		}
	}
}


void WordPool::phrase_sort() {
	const pnode * p;
	for (int i = 0; i < PHRASE_HASH_SIZE; i++) {
		for (p = ptable[i]; p; p = p->next) phrase_sort_insert(p);
	}
}

int WordPool::get_max_phrase(int i, string &e) {
	if (phrase_sorted == false) {
		phrase_sort();
		phrase_sorted = true;
	}
	if (i < 0 || i >= 10 || pmax[i] == NULL) {
		return -1;
	}
	else {
		e = string(pmax[i]->w1->exp) + ' ' + string(pmax[i]->w2->exp);
		return pmax[i]->count;
	}
}


bool isbeginner(char c) {
	return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}


bool isdivisor(char c) {
	return !((('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')));
}


void WordCounter::count(char c) {
	if (overflow) {
		if (isdivisor(c)) {
			exp_len = 0;
			overflow = false;
		}
	}
	else if (exp_len < 4) {
		if (exp_len == 0 && !pre_divisor) ;
		else if (isbeginner(c)) {
			exp[exp_len++] = c;
		}
		else {
			exp_len = 0;
		}
	}
	else {
		if (isdivisor(c)) {
			num++;
			exp[exp_len] = 0;
			pre = now;
			now = pool.add_word(exp);
			if (pre) pool.add_phrase(pre, now);
			exp_len = 0;
		}
		else {
			if (exp_len > WORD_SIZE) {
				overflow = true;
			}
			else exp[exp_len++] = c;
		}
	}
	pre_divisor = isdivisor(c);
}


void WordCounter::count_eof() {
	if (!overflow && exp_len >= 4) {
		num++;
		exp[exp_len] = 0;
		pre = now;
		now = pool.add_word(exp);
		if (pre) pool.add_phrase(pre, now);
		exp_len = 0;
	}
	pre_divisor = true;
	exp[0] = 0;
	exp_len = 0;
	overflow = false;
	pre = NULL;
	now = NULL;
}


int WordCounter::get_max_word(int i, string &e) {
	return pool.get_max_word(i, e);
}


int WordCounter::get_max_phrase(int i, string &e) {
	return pool.get_max_phrase(i, e);
}


CharCounter::CharCounter() {
	char_num = 0;
	line_num = 0;
}


void CharCounter::count(char c) {
	// TODO: different from official
	if( c == '\n') {
		line_num++;
	}
	else if(c >= 32 && c <= 126) {
		char_num++;
	}
}


void CharCounter::count_eof() {
	line_num++;
}


int CharCounter::get_char_num() {
	return char_num;
}


int CharCounter::get_line_num() {
	return line_num;
}


bool Counter::count(string filepath) {
	ifstream in;
	in.open(filepath.c_str(), ios::in);
	if (in.bad()) return false;

	char c;
	while (!in.eof()) {
		c = in.get();
		cc.count(c);
		wc.count(c);
	}
	cc.count_eof();
	wc.count_eof();
	in.close();

	return true;
}


int Counter::get_char_num() {
	return cc.get_char_num();
}


int Counter::get_line_num() {
	return cc.get_line_num();
}


int Counter::get_word_num() {
	return wc.get_word_num();
}


int Counter::get_max_word(int i, string &e) {
	return wc.get_max_word(i, e);
}


int Counter::get_max_phrase(int i, string &e) {
	return wc.get_max_phrase(i, e);
}
