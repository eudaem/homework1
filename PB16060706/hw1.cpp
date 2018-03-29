#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string>
#include<io.h>
#include"targetver.h"
#include<fstream>
#include<vector>
#include<iostream>
#include<tchar.h>
#include<numeric>
#include<functional>
#include<unordered_map>
using namespace std;
int  char_num = 0, line_num = 1, word_num = 0;
#define lnode struct node
struct node { 
int num; 
char word[200]; 
struct node* next;
};
void GetAllFiles(string path, vector<string>& files,int &char_num,int &line_num,int &word_num)
{ 
  FILE *f1 =NULL;
  int count = 0, i = 0;
  long  handle = 0;
  struct _finddata_t fileinfo;  // File Information 
  string p;
  char * location;  // location converts char * into string
  
