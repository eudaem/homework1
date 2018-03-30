#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
struct Nodeofsameword_tag{
	long unsigned int hasher;
	long unsigned int thewordnum;
	int whatnext;
	struct Nodeofsameword_std *std_next;
	struct Nodeofsameword_lrg *lrg_next;
};

struct Nodeofsameword_std {
	char name[32];
	int whatnext;		//0:std_next,1:lrg_next
	struct Nodeofsameword_std *std_next;
	struct Nodeofsameword_lrg *lrg_next;
};
struct Nodeofsameword_lrg {
	char name[1024];
	int whatnext;
	struct Nodeofsameword_std *std_next;
	struct Nodeofsameword_lrg *lrg_next;
};
long unsigned int howmanystartby[26] = { 0,0,0,0 , 0,0,0,0 , 0,0,0,0 , 0,0,0,0 , 0,0,0,0 , 0,0,0,0,0,0 };
struct Nodeofsameword_tag* startby[26][131072];
int checkmenu(char* menuname);
void doit(char* tgp);
int check_ltr_num_or_else(char a);
void strrefresh(char* a, int b);
long unsigned int charnum = 0, rownum = 0,wordnum=0;
void nwrsystem(char* a, int b);
void stdrize(char* a, int b , char* stdr);
int hash_function(char *p);

int main(int argc,char* argv[]){	//just infile
	//doit(argv[1]);
	char testing[] = "new.txt";
	//while (getchar() != '\n');
	doit(testing);
	//while (getchar() != '\n');
	outputit();
	return(0);
}

int check_menu(char* menuname) {
	return(0);
}

void doit(char* tgp) {
	int i;
	char str[2049],word[1025],tgsp[128];
	strrefresh(tgsp, 128);
	for (i=0;;i++) {
		tgsp[i] = tgp[i];
		if (tgp[i] == '\0')break;
	}
	tgsp[i] = '.';		//a stupid way to do strcat
	tgsp[i + 1] = 'm';
	tgsp[i + 2] = 'i';
	tgsp[i + 3] = 'd';
	tgsp[i + 4] = 'l';
	strrefresh(str,2048);
	strrefresh(word,1024);
	ifstream infile;
	//ofstream midfile;
	//midfile.open(tgsp);		//use middle file to format the words 
	infile.open(tgp);
	int copyloop=0,copywriteloop=0,clnos=0,k,remaindernum=2048,stoploop=0;		//clnos means check_letter_or........
	rownum++;
	for (;;) {		//write the midfile
		infile.get(str, 2048, '\0');
		//printf("%s", str);
		if ((str[2046] == '\0')&& (str[2045] == '\0')&& (str[2044] == '\0')) {		//Here with a stupid way to verify if the text has ended......
			remaindernum = strlen(str);
			stoploop = 1;
		}
		for (copyloop=0;;copyloop++) {		//tThe meaning of this code is to single out the letters that belong to words and write them into the midfile.
			clnos = check_ltr_num_or_else(str[copyloop]);
			if ((str[copyloop] == '\n') && (copyloop != remaindernum - 1))rownum++;
			if (clnos != -1)charnum++;		//Record the number of valid chars
			if ((copywriteloop == 0) && (clnos == 0));
			else if (clnos >= 2) {		//letter?
				word[copywriteloop] = str[copyloop];
				copywriteloop++;
			}
			else if ((copywriteloop != 0) && (clnos == 1)) {		//A number that is not at the beginning of the word?
				word[copywriteloop] = str[copyloop];
				copywriteloop++;
			}
			else if ((copywriteloop >= 4)&&(check_ltr_num_or_else(word[0])>=2) && (check_ltr_num_or_else(word[1]) >= 2) && (check_ltr_num_or_else(word[2]) >= 2) && (check_ltr_num_or_else(word[3]) >= 2)) {	//more than three letters before being recorded?
				//for (k = 0; k <= copywriteloop; k++) midfile.put(word[k]);
				//midfile.put('\t');
				nwrsystem(word , copywriteloop);		//regist the word
				wordnum++;
				strrefresh(word, copywriteloop);
				copywriteloop = 0;
			}
			else {		//nothing but just a char that does no matter to a word
				strrefresh(word, copywriteloop);
				copywriteloop = 0;
			}
			if ((copyloop == remaindernum - 1) && stoploop) {
				//midfile.put('\t');
				wordnum++;
			}
			if (copyloop == remaindernum - 1)break;
		}
		strrefresh(str, 2048);
		if (stoploop)break;
	}
	//printf("\n%dof rows", rownum);		//use testa1 testa2 testa3 three txts to test the num of rows
	infile.close();
}

int check_ltr_num_or_else(char a) {		//literally speaking emmmmmm
	if ((a < 32) || (a > 126))return(-1);
	else if ((a >= 48) && (a <= 57))return(1);
	else if ((a >= 65) && (a <= 90))return(2);
	else if ((a >= 97) && (a <= 122))return(3);
	else return(0);
}

void strrefresh(char* a, int b) {
	int i;
	for (i = 0; i <= (b - 1); i++) {
		a[i] = '\0';
	}
}

void nwrsystem(char *a, int b) {
	char stdr[1025];
	struct Nodeofsameword_std *rear_std=NULL,*node_std=NULL;
	struct Nodeofsameword_lrg *rear_lrg=NULL,*node_lrg=NULL;
	struct Nodeofsameword_tag *node_tag=NULL;
	int j,t,k,check=0,big=0,n,nowrear=0;
	if (strlen(a) >= 32)big = 1;
	long unsigned int i,hax,p;
	stdrize(a,b, stdr);
	hax = hash_function(stdr);
	k = stdr[0] - 97;
	p = howmanystartby[k];
	for (i = 0; i < p; i++) {
		if (hax == ((startby[k][i])->hasher)) {
			n = startby[k][i]->thewordnum;
			startby[k][i]->thewordnum++;
			if (big) {
				if (startby[k][i]->whatnext) {
					rear_lrg = startby[k][i]->lrg_next;
					nowrear = 1;
				}
				else {
					rear_std = startby[k][i]->std_next;
					nowrear = 0;
				}
				node_lrg = (Nodeofsameword_lrg *)malloc(sizeof(Nodeofsameword_lrg));
				node_lrg->lrg_next = NULL;
				node_lrg->std_next = NULL;
				for (t = 0; t <= b; t++)(node_lrg->name)[t] = a[t];
				for (j = 0; j < (n-1); j++) {
					if (nowrear) {
						if (rear_lrg->whatnext) {
							rear_lrg = rear_lrg->lrg_next;
							nowrear = 1;
						}
						else {
							rear_std = rear_lrg->std_next;
							nowrear = 0;
						}
					}
					else {
						if (rear_std->whatnext) {
							rear_lrg = rear_std->lrg_next;
							nowrear = 1;
						}
						else {
							rear_std = rear_std->std_next;
							nowrear = 0;
						}
					}
				}
				if (nowrear) {
					rear_lrg->whatnext = 1;
					rear_lrg->lrg_next = node_lrg;
				}
				else {
					rear_std->whatnext = 1;
					rear_std->lrg_next = node_lrg;
				}
			}
			else {
				if (startby[k][i]->whatnext) {
					rear_lrg = startby[k][i]->lrg_next;
					nowrear = 1;
				}
				else {
					rear_std = startby[k][i]->std_next;
					nowrear = 0;
				}
				node_std = (Nodeofsameword_std *)malloc(sizeof(Nodeofsameword_std));
				node_std->lrg_next = NULL;
				node_std->std_next = NULL;
				for (t = 0; t <= b; t++)(node_std->name)[t] = a[t];
				for (j = 0; j < (n - 1); j++) {
					if (nowrear) {
						if (rear_lrg->whatnext) {
							rear_lrg = rear_lrg->lrg_next;
							nowrear = 1;
						}
						else {
							rear_std = rear_lrg->std_next;
							nowrear = 0;
						}
					}
					else {
						if (rear_std->whatnext) {
							rear_lrg = rear_std->lrg_next;
							nowrear = 1;
						}
						else {
							rear_std = rear_std->std_next;
							nowrear = 0;
						}
					}
				}
				if (nowrear) {
					rear_lrg->whatnext = 0;
					rear_lrg->std_next = node_std;
				}
				else {
					rear_std->whatnext = 0;
					rear_std->std_next = node_std;
				}
			}
			return;
		}
	}
	node_tag = (Nodeofsameword_tag*)malloc(sizeof(Nodeofsameword_tag));
	node_tag->hasher = hax;
	node_tag->thewordnum = 1;
	if (big) {
		node_lrg = (Nodeofsameword_lrg*)malloc(sizeof(Nodeofsameword_lrg));
		node_tag->lrg_next = node_lrg;
		node_tag->whatnext = 1;
		node_lrg->lrg_next = NULL;
		node_lrg->std_next = NULL;
		for (t = 0; t <= b; t++)(node_lrg->name)[t] = a[t];
	}
	else {
		node_std = (Nodeofsameword_std*)malloc(sizeof(Nodeofsameword_std));
		node_tag->std_next = node_std;
		node_tag->whatnext = 0;
		node_std->lrg_next = NULL;
		node_std->std_next = NULL;
		for (t = 0; t <= b; t++)(node_std->name)[t] = a[t];
	}
	startby[k][p] = node_tag;
	howmanystartby[k]++;
	return;
}

int hash_function(char *p){
	long unsigned int value = 0;
	while (*p != '\0')
	{
		value = value * 31 + *p++;
		if (value > (64*131072))
			value = value % (64*131072);
	}
	return value;
}

void stdrize(char* a,int b, char* stdr) {		//standradrize a to b
	int i;
	for (i = 0; i <= b; i++)stdr[i] = a[i];
	for (i = b - 1;;) {
		if (check_ltr_num_or_else(stdr[i]) == 1) {
			stdr[i] = '\0';
			i--;
		}
		else break;
	}
	for (;;) {
		if (check_ltr_num_or_else(stdr[i]) == 2)stdr[i] = stdr[i] + 32;
		if (i == 0)break;
		i--;
	}
	return;
}
