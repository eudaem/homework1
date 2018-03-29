/****************************************************************************************
Name: Word-frequency statistics

Purpose: This routine recieve a folder path as command line parameter,
count the characters', lines' and words' number, store the frequency
of each word and phrase, write top 10 of them and the three number in
the RESULT.txt

Algorithum: using _finddata_ to travel all paths of files; then read
the word in the file in order, store them in one hashtable, the hash
function is designed for the test set; the same as the phrase which is
defined as two adjacent words. I separate this two parts because I run
out of my memory, it could be faster to 2 times than now orginally if
enough.

Input: a folder path as command line parameter

Output: RESULT.txt including the characters', lines' and words' number
and the top 10 frequency of words and phrases

Author: Zhikai Chen

Date-created: 18/3/29

E-mail: chenzhik98@gmail.com

Statement: there is some problems of my PC's win enviroment when I create
it, any problems please contact with me. thk.
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <cstring>
#include <io.h>
#include <iostream>
#include <vector>
using namespace std;

#define number 0
#define alpha 1
#define separator -1
#define not_asc -2
#define arraysize 50
#define FileEnd -3
#define max_hashsize 400000
#define topten 10

// GLOBAL VARIABLES

int CHARS = 0;
int LINES = 0;
int WORDS = 0;


// STRUCTRUES
// Define a element type including the word's info and frequency
// "word_prefix" to store the prefix which desides the word
// "word_suffix" to store the suffix numbers which doesn't matter
typedef struct
{
	string word_prefix;
	string word_suffix;
}ElemType;

// Define a hashnode to store the word and its info
// "ElemType" to store the element
// "data_count" to accumulate the frequency of a word
// "next" points to the next hashnode
typedef struct HashNode
{
	ElemType data;
	ElemType next_data;
	int data_count;
	HashNode* next;
}HashNode, *Hashptr;

// Define a hashtable to store the hashnodes
// "nodes[]" is a array of hashnodes
// "hashsize" is the size of "nodes[]" which equals to max_size
typedef struct
{
	Hashptr* elem;
	int hashsize;
}HashTable;


/****************************************************************************************/
// FUNCTIONS
// read cmd file, store in the vector 'files'
void GetAllFiles(string path, vector<string>& files);

// Set the char string 'temp' to '' as initialization
void InitTemp(string &temp);

// Get one character from the disk file "stream"
char ReadNextChar(FILE* stream);

// Judge a character or numberif ch is a alpha, return 1;if a number, return 0;else a separator, return -1.
int JudgeCharType(char ch);

// Judge a word from the disk file "stream", if it is a word, return its length.
int Seek1Word(FILE* stream, char flag[]);

// Compare two characters
int Compare2Chars(char c1, char c2);

//Compare two words,
bool Compare2Words(ElemType e1, ElemType e2);

//Select one word form from the same two words, return 1 if e1, else if e2 return 2, else 0
int Select1WordForm(ElemType e1, ElemType e2);

//Copy a word to another place by copy two part of it
void CopyWord(ElemType &dest, ElemType src);

// Separate a word into two parts which are prefix and suffix, return the prefix's length
int SeparateWord(char* temp, ElemType &e);

//Initialize a structure of elemtype by using '\0'
void InitElem(ElemType &e);

// Calculate the hashing result of one element
int HashForWord(ElemType e);

// Calculate the hashing result of two neighbor elements
int HashForPhrase(ElemType e1, ElemType e2);

//  Search one word in hashtable, return its location in "p"
bool SearchHashWord(HashTable H, ElemType e, Hashptr &p);

// Search one phrase in hashtable, return its location in "p"
bool SearchHashPhrase(HashTable H, ElemType e1, ElemType e2, Hashptr &p);

// Create a hashtable
void CreateHash(HashTable &H);

// Insert one word to a hashtable, return true if insert one new word, return false else
bool InsertHashWord(HashTable &H, ElemType e, Hashptr &p);

// Insert one phrase to a hashtable, return true if insert one new phrase, return false else
bool InsertHashPhrase(HashTable H, ElemType e1, ElemType e2, Hashptr &p);

// print the data in this hashnode with its frequency
void VisitHashWord(Hashptr present_node, FILE* fp);

// print the data and next_data in this hashnode with its frequency
void VisitHashPhrase(Hashptr present_node, FILE* fp);

/****************************************************************************************/
// FUNCTIONS' DETAILS
void GetAllFiles(string path, vector<string>& files)
{
	long  handle = 0;  // File Handle
	struct _finddata_t fileinfo;  // File Information
	string p;
	char * location;  // location converts char * into string

	if ((handle = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))  // to check whether it is a folder or a file
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				location = (char *)p.data();
			}
		} while (_findnext(handle, &fileinfo) == 0);
		_findclose(handle);
	}
}

void InitTemp(string &temp)
{
	temp = "";
}

char ReadNextChar(FILE* stream)
{
	return(fgetc(stream));
}

int JudgeCharType(char ch)
{
	if (ch == EOF)
	{
		return FileEnd;
	}
	//count the characters' number
	if (ch > 31 && ch < 127)
	{
		CHARS++;
	}
	//count the lines' number
	if (ch == '\n')
	{
		LINES++;
	}
	if (ch < 32 || ch > 126)
	{
		if (ch != 10 && ch != 9 && ch != 13)
			return separator;
		return not_asc;
	}
	if (isalpha(ch) > 0)
	{
		return alpha;
	}
	else if (ch >= '0' && ch <= '9')
	{
		return number;
	}
	else
	{
		return separator;
	}
}

int Seek1Word(FILE* stream, string &temp)
{
	// variables:
	// 'ch' recieve the character from the file
	// 'prelength' return the lengh of the word
	// 'temp' store the word
	char ch;
	int prelength;

	// initialize the variables
	prelength = 0;
	ch = '\0';
	InitTemp(temp);//unnecessary for this program

	// find a word from a start of separator 
	ch = ReadNextChar(stream);
	if (!feof(stream))
	{
		// Judge a word: the first four alphas
		for (prelength = 0; prelength < 4 && ch != EOF; prelength++)
		{
			if (JudgeCharType(ch) == alpha)
			{
				temp.push_back(ch);
				ch = ReadNextChar(stream);
			}

			//the present ch is not an alpha
			else
			{
				InitTemp(temp);
				prelength = -1;
				//return to loop 'for' then 'prelength' will be added 1
				ch = ReadNextChar(stream);
			}
			//end else if
		}
		//end 'for'
		if (prelength == 4)
		{
			// seek the next separator to give out the length of this word 
			while (JudgeCharType(ch) == number || JudgeCharType(ch) == alpha)
			{
				temp.push_back(ch);
				prelength++;
				ch = ReadNextChar(stream);
			}
		}
		else if (ch == EOF)
		{
			return FileEnd;
		}
		WORDS++;
		return prelength;
	}
	else
	{
		return FileEnd;
	}
}

int SeparateWord(string temp, ElemType &e)
{
	int i;
	int suffix_length;
	int prefix_length;
	suffix_length = 0;

	// caculate the suffix's length
	for (i = temp.length() - 1; i > 0; i--)
	{
		if (JudgeCharType(temp[i]) == number)
		{
			suffix_length++;
		}
		else break;
	}

	// separate the word to prefix and suffix
	prefix_length = temp.length() - suffix_length;
	for (i = 0; i < prefix_length; i++)
	{
		e.word_prefix.push_back(temp[i]);
	}
	for (i = 0; i < suffix_length; i++)
	{
		e.word_suffix.push_back(temp[prefix_length + i]);
	}
	return prefix_length;
}

void InitElem(ElemType &e)
{
	InitTemp(e.word_prefix);
	InitTemp(e.word_suffix);
}

int Compare2Chars(char c1, char c2)
{
	//capital and lower alphas are the same
	if (c1 == c2)
	{
		return 0;
	}
	else if (c1 - c2 == 'A' - 'a')
	{
		return 1;
	}
	else if (c1 - c2 == 'a' - 'A')
	{
		return 2;
	}
	else
	{
		return -1;
	}
}

bool Compare2Words(ElemType e1, ElemType e2)
{
	// initialize a accumulator
	int i;
	i = 0;

	// loop for end condition
	while ((e1.word_prefix[i] != '\0') || (e2.word_prefix[i] != '\0'))
	{
		if (Compare2Chars(e1.word_prefix[i], e2.word_prefix[i]) >= 0)
		{
			i++;
		}
		else
		{
			return false;
		}
	}

	if ((e1.word_prefix[i] == '\0') && (e2.word_prefix[i] == '\0'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Select1WordForm(ElemType e1, ElemType e2)
{
	// initialize a accumulator
	int i;
	i = 0;

	// ensure these two words are the same
	if (Compare2Words(e1, e2) == true)
	{
		// loop for end condition
		if (strcmp(e1.word_prefix.c_str(), e2.word_prefix.c_str()) < 0)
		{
			return 1;
		}
		else if (strcmp(e1.word_prefix.c_str(), e2.word_prefix.c_str()) > 0)
		{
			return 2;
		}
		else
		{
			if (strcmp(e1.word_suffix.c_str(), e2.word_suffix.c_str()) < 0)
			{
				return 1;
			}
			else if (strcmp(e1.word_suffix.c_str(), e2.word_suffix.c_str()) > 0)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}
	}
	else
	{// e1 is different from e2
		return 0;
	}
}

void CopyWord(ElemType &dest, ElemType src)
{
	dest.word_prefix.assign(src.word_prefix);
	dest.word_suffix.assign(src.word_suffix);
}

int HashForWord(ElemType e)
{
	int i;
	int sum;
	int adder;

	sum = 0;
	adder = 0;

	for (i = 0; e.word_prefix[i] != '\0'; i++)
	{
		if (e.word_prefix[i] >= 'a' && e.word_prefix[i] <= 'z')
		{
			adder = e.word_prefix[i] + 'A' - 'a';
			sum = (sum * 31 + adder) % max_hashsize;
		}
		else
		{
			sum = (sum * 31 + e.word_prefix[i]) % max_hashsize;
		}
	}
	sum = sum % max_hashsize;
	return sum;
}

int HashForPhrase(ElemType e1, ElemType e2)
{
	long sum;
	int adder;
	int i;

	sum = HashForWord(e1);
	for (i = 0; e2.word_prefix[i] != '\0'; i++)
	{
		if (e2.word_prefix[i] >= 'a' && e2.word_prefix[i] <= 'z')
		{
			adder = e2.word_prefix[i] + 'A' - 'a';
			sum = (sum * 31 + adder) % max_hashsize;
		}
		else
		{
			sum = (sum * 31 + e2.word_prefix[i]) % max_hashsize;
		}
	}
	sum = sum % max_hashsize;
	return sum;
}

bool SearchHashWord(HashTable H, ElemType e, Hashptr &p)
{
	Hashptr q;
	Hashptr dq;
	int hash_result;
	bool judge_compare;

	q = new HashNode;
	dq = q;

	hash_result = HashForWord(e);
	if (hash_result < 0){
		printf("error");
	}
	p = H.elem[hash_result];

	judge_compare = !Compare2Words(p->data, e);
	while (p != NULL && !Compare2Words(p->data, e))
	{
		q = p;
		p = p->next;
	}
	if (p)
	{
		return true;
	}
	else
	{
		p = q;
		delete dq;
		return false;
	}
}

bool SearchHashPhrase(HashTable H, ElemType e1, ElemType e2, Hashptr &p)
{
	Hashptr q;
	Hashptr dq;//trace q to delete q
	int hash_result;

	q = new HashNode;
	dq = q;
	hash_result = HashForPhrase(e1, e2);
	p = H.elem[hash_result];

	//	judge_compare = Compare2Words(p->data, e1) && Compare2Words(p->data, e2);
	while (p != NULL && !(Compare2Words(p->data, e1) && Compare2Words(p->next_data, e2)))
	{
		q = p;
		p = p->next;
	}
	if (p)
	{
		return true;
	}
	else
	{
		p = q;
		delete dq;
		return false;
	}
}

void CreateHash(HashTable &H)
{
	int i;

	H.elem = new Hashptr[H.hashsize];
	for (i = 0; i<H.hashsize; i++){
		H.elem[i] = new HashNode;
		H.elem[i]->data_count = 0;
		InitElem(H.elem[i]->data);
		InitElem(H.elem[i]->next_data);
		H.elem[i]->next = NULL;
	}
}

bool InsertHashWord(HashTable &H, ElemType e, Hashptr &p)
{
	Hashptr s;

	if (SearchHashWord(H, e, p) == true)
	{
		if (Select1WordForm(p->data, e) == 2)
		{
			CopyWord(p->data, e);
		}
		p->data_count = p->data_count + 1;
		return false;
	}
	else
	{
		s = new HashNode;
		CopyWord(s->data, e);
		p->next = s;
		s->data_count = 1;
		s->next = NULL;
	}
	return true;
}

bool InsertHashPhrase(HashTable H, ElemType e1, ElemType e2, Hashptr &p)
{
	Hashptr s;

	if (SearchHashPhrase(H, e1, e2, p) == true)
	{
		// change the form of the phrase
		if (Select1WordForm(p->data, e1) == 2)
		{
			CopyWord(p->data, e1);
		}
		if (Select1WordForm(p->data, e1) == 2)
		{
			CopyWord(p->data, e1);
		}
		// accumulator plus 1
		p->data_count = p->data_count + 1;
		return false;
	}
	else
	{
		s = new HashNode;
		CopyWord(s->data, e1);
		CopyWord(s->next_data, e2);

		p->next = s;
		s->data_count = 1;
		s->next = NULL;
	}
	return true;
}

void VisitHashWord(Hashptr present_node, FILE* fp)
{
	fprintf(fp, "%s ", present_node->data.word_prefix.c_str());
	fprintf(fp, "%s ", present_node->data.word_suffix.c_str());
	fprintf(fp, ":%d\n", present_node->data_count);
}

void VisitHashPhrase(Hashptr present_node, FILE* fp)
{
	fprintf(fp, "%s", present_node->data.word_prefix.c_str());
	fprintf(fp, "%s", present_node->data.word_suffix.c_str());
	fprintf(fp, " ");
	fprintf(fp, "%s", present_node->next_data.word_prefix.c_str());
	fprintf(fp, "%s", present_node->next_data.word_suffix.c_str());
	fprintf(fp, ":%d\n", present_node->data_count);
}

/****************************************************************************************/
int main(int argc, char* argv[])
{
	//S1: VARIABLES
	vector<string>files;//store file paths
	FILE* fp;//point at the present file stream
	FILE* rfp;//point at the result file stream
	int i;//accumulator
	int j;//accumulator
	int max_frequency;
	int word_length;
	int word_prefix_length;
	int word_suffix_length;
	int sums[10];//temporary array to store the 10 most words and phrases index
	string temp;//temporary string
	char *filepath;//cmd parameter
	ElemType first_elem;
	ElemType second_elem;
	HashTable words;
	HashTable phrases;
	Hashptr hashpoint;
	Hashptr nextpoint;
	Hashptr max_frequency_point;
	Hashptr first_10_nodes[10];

	//S2: INITISLIZE
	i = 0;
	j = 0;
	for (j = 0; j < 10; j++)
	{
		sums[j] = 0;
	}
	filepath = argv[1];
	max_frequency_point = NULL;
	words.hashsize = max_hashsize;
	phrases.hashsize = max_hashsize;
	InitElem(first_elem);
	InitElem(second_elem);
	CreateHash(words);
	CreateHash(phrases);
	GetAllFiles(filepath, files);

	//S3: TRAVELING ALL FILES FOR WORDS
	cout << "Words finding..." << endl;
	cout << "1st-Reading the file: " << files.size() << " in total" << endl;
	while (i < files.size())
	{
		cout << i + 1 << endl;
		if ((fopen_s(&fp, files.at(i).c_str(), "r")) != 0)
		{
			cout << "can not open file:" << endl;
			cout << files.at(i).c_str() << endl;
		}

		// LOOP FOR TRAVELING FILE "i"
		while (!feof(fp))
		{
			//1:insert the first word
			word_length = Seek1Word(fp, temp);
			//if fp goes to the file's end
			if (word_length == FileEnd)
			{
				break;
			}
			word_prefix_length = SeparateWord(temp, first_elem);
			InsertHashWord(words, first_elem, hashpoint);
			//initilize
			InitElem(first_elem);
		}
		fclose(fp);
		i++;
	}

	//S3: TRAVEL HASHTABLE FOR FREQUENCY	
	for (j = 0; j < 10; j++)
	{//every time pick out one top hashpoint
		max_frequency = 0;
		for (i = 0; i < words.hashsize; i++)
		{
			hashpoint = words.elem[i];
			while (hashpoint->next != NULL)
			{
				if (hashpoint->next->data_count > max_frequency)
				{
					max_frequency = hashpoint->next->data_count;
					max_frequency_point = hashpoint->next;
				}
				hashpoint = hashpoint->next;
			}
		}
		first_10_nodes[j] = max_frequency_point;
		first_10_nodes[j]->data_count = 0;
		sums[j] = max_frequency;//flag for getting already
	}

	//S4: PRINT THE WORD RESULT
	fopen_s(&rfp, "Result.txt", "w");
	fprintf(rfp, "char_number:%d\n", CHARS);
	fprintf(rfp, "line_number:%d\n", LINES);
	fprintf(rfp, "word_number:%d\n", WORDS);
	cout << "char_number:" << CHARS << endl;
	cout << "line_number:" << LINES << endl;
	cout << "word_number:" << WORDS << endl;

	for (j = 0; j < 10; j++)
	{
		cout << first_10_nodes[j]->data.word_prefix;
		cout << first_10_nodes[j]->data.word_suffix << ":";
		cout << sums[j] << endl;
		VisitHashWord(first_10_nodes[j], rfp);
	}

	//S5: FREE HASHTABLEWORD "words"
	for (i = 0; i < max_hashsize; i++)
	{
		hashpoint = words.elem[i];
		hashpoint = hashpoint->next;
		while (hashpoint != NULL)
		{
			nextpoint = hashpoint->next;
			delete hashpoint;
			hashpoint = nextpoint;
		}
	}

	//S6: TRAVEL ALL FILES FOR PHRASES
	i = 0;
	cout << "Phrases finding..." << endl;
	cout << "2nd-Reading the file: " << files.size() << " in total" << endl;
	while (i < files.size())
	{
		cout << i + 1 << endl;
		if ((fopen_s(&fp, files.at(i).c_str(), "r")) != 0)
		{
			cout << "can not open file:" << endl;
			cout << files.at(i).c_str() << endl;
		}

		// LOOP FOR TRAVELING FILE "i"
		while (!feof(fp))
		{
			//Part1:seek the first word
			word_length = Seek1Word(fp, temp);
			//if fp goes to the file's end
			if (word_length == FileEnd)
			{
				break;
			}
			word_prefix_length = SeparateWord(temp, first_elem);

			while (!feof(fp))
			{
				//Part2:insert the second word
				word_length = Seek1Word(fp, temp);
				//if fp goes to the file's end
				if (word_length == FileEnd)
				{
					break;
				}
				word_prefix_length = SeparateWord(temp, second_elem);

				//3:insert the 1st and 2nd words as a phrase
				InsertHashPhrase(phrases, first_elem, second_elem, hashpoint);

				//initialize
				InitElem(first_elem);
				CopyWord(first_elem, second_elem);
				InitElem(second_elem);
			}
		}
		fclose(fp);
		i++;
	}


	// initialize the sums of words for phrases
	for (j = 0; j < 10; j++)
	{
		sums[j] = 0;
	}


	//S3: TRAVEL HASHTABLE FOR FREQUENCY	
	for (j = 0; j < 10; j++)
	{
		max_frequency = 0;
		for (i = 0; i < phrases.hashsize; i++)
		{
			hashpoint = phrases.elem[i];
			while (hashpoint->next != NULL)
			{
				if (hashpoint->next->data_count > max_frequency)
				{
					max_frequency = hashpoint->next->data_count;
					max_frequency_point = hashpoint->next;
				}
				hashpoint = hashpoint->next;
			}
		}
		first_10_nodes[j] = max_frequency_point;
		first_10_nodes[j]->data_count = 0;
		sums[j] = max_frequency;//flag for getting already
	}

	//S7: PRINT THE PHRASE RESULT
	for (j = 0; j < 10; j++)
	{
		cout << first_10_nodes[j]->data.word_prefix;
		cout << first_10_nodes[j]->data.word_suffix << ' ';
		cout << first_10_nodes[j]->next_data.word_prefix;
		cout << first_10_nodes[j]->next_data.word_suffix << ":";
		cout << sums[j] << endl;
		VisitHashPhrase(first_10_nodes[j], rfp);
	}
	fclose(rfp);
	return 1;
	system("pause");
}
