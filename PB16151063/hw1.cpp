#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <string> 

using namespace std;

#define MAX 100000
//1.打开文件
//2.读取
//3.统计字符个数
//4.统计行数,构造哈希表
//5.统计单词数量(不区分大小写)
//6.统计单词出现频率
//7.统计词组在一起的频率

struct HaffWordTable {//单词结构体
	string HT;
	unsigned long word_apearnum=0;
}H[100000];


struct HaffPraseTable {//词组结构体
	string HPT;
	unsigned long Prase_apearnum = 0;
}Hp[100000];
struct Praseten {//为了方便直接传回前10的数据
	string str_p[10];
	unsigned long num_p[10];
};
struct Wordten {//为了方便直接传回前十的数据
	string str_w[10];
	unsigned long num_w[10];
};

void dfsFolder(string folderPath, unsigned long &l, unsigned long &c, unsigned long &w);
void fileopen(string filepath, unsigned long &lh, unsigned long &ch, unsigned long &wh);
long WholeWord(char a[]);
long CharWords(char a[]);
void PraseNumber(char f[]);
int Compareword(char e[], string eh);
int ReplaceWord(char f[], string fh);
void BulidHaffWord(char d[], struct HaffWordTable H[]);
int ComparePrase(char ch[], string l);
int ReplacePrase(char ch[], string l);
void BulidHaffPrase(char d[], struct HaffPraseTable Hp1[], unsigned long a);
struct Praseten sorting_p();
struct Wordten sorting_w();

void dfsFolder(string folderPath, unsigned long &l, unsigned long &c, unsigned long &w)//遍历文件
{
	unsigned long j,k,g;
	j = 0; k = 0; g = 0;
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*.*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		return ;
	}
	do {
		
		//判断是否有子目录  
		if (FileInfo.attrib == _A_SUBDIR)
		{
			//这个语句很重要  
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				//printf("1");
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath,l,c,w);
			}
		}
		else
		{
			
			string newPath1 = folderPath + "\\" + FileInfo.name;//printf("1");
			fileopen(newPath1,j,k,g);
			l = l + j;
			c = c + k;
			w = w + g;
			cout << folderPath.c_str() << "\\" << FileInfo.name << endl;//getchar();
		}
	} while (_findnext(Handle, &FileInfo) == 0);

	_findclose(Handle); 
}

void fileopen(string filepath, unsigned long &lh, unsigned long &ch, unsigned long &wh) {//打开文件,并读取   已测试
	unsigned long number1 ;
	unsigned long number2 ;
	ifstream file1(filepath);
	char buffer[100000]="";//一行的字符
	if (file1.is_open())
	{   
		while (!file1.eof())
		{   
			file1.getline(buffer, 100000);
			lh++;          //行数加一,统计行数
			number1 = CharWords(buffer);
			number2 = WholeWord(buffer); 
			ch = ch + number1; 
			wh = wh + number2;
			PraseNumber(buffer);
		}
		
	}
	file1.close();
}

long CharWords(char a[]) {//统计字符的个数,buffer
	unsigned long i = 0;
	unsigned long sum1=0;
	unsigned long temp = strlen(a);
	for (i = 0; i < temp; i++) {
		if (a[i] >= 32 && a[i] <= 126)
		{
			sum1++;
		}
	}
	return sum1;
}

long WholeWord(char a[]) {//统计单词的个数,buffer
	unsigned long i = 0;
	char b[1024] = {""};//存放单词
	int k = 0;
	unsigned long j = 0;
	unsigned long temp;
	unsigned long word_number_l=0;
	struct HaffWordTable;
	temp = strlen(a);
	for (i=0; i < temp; i++) {
		if ((a[i] >= 65 && a[i] <= 90) || (a[i] >= 97 && a[i] <= 122)) {
			if (i == 0 || (a[i-1] < 48) || (a[i-1] > 57 && a[i-1] < 65) || (a[i-1] > 90 && a[i-1] < 97) || (a[i-1] > 122)) {
				if ((a[i + 1] >= 65 && a[i + 1] <= 90) || (a[i + 1] >= 97 && a[i + 1] <= 122)) {
					if ((a[i + 2] >= 65 && a[i + 2] <= 90) || (a[i + 2] >= 97 && a[i + 2] <= 122)) {
						if ((a[i + 3] >= 65 && a[i + 3] <= 90) || (a[i + 3] >= 97 && a[i + 3] <= 122)) {//有单词出现
							j = i;
							do {
								k = 0;
								b[k] = a[j];//将一个单词转移到b中
								//printf("%c", b[k]);
								k++;
								j++;
							} while ((a[j] >= 65 && a[j] <= 90) || (a[j] >= 97 && a[j] <= 122)||(a[j] >= 48 && a[j] <= 57));//出现分隔符则停止，b中储存了一个单词;
							word_number_l++;
							BulidHaffWord(b, H);
							i = j;//将j的地址给i
						}
					}
				}
			}
		}

	}
	return word_number_l;
}


int Compareword(char e[],string eh) {//比较新来的与已存入的单词,0为不一样,1为一样   已改
	const char *e1;
	e1 = eh.c_str();
	unsigned long temp_e;
	unsigned long temp_eh;
	unsigned long temp;
	temp_e = strlen(e);
	temp_eh = strlen(e1);
	if (temp_e > temp_eh) temp = temp_e;
	else temp = temp_eh;
	for (int i = 0; i < 20; i++) {
		if (e1[i] -32!= e[i]&& e1[i] + 32 != e[i]&& e1[i] != e[i]) {//不分大小写
			if (i < 3) return 0;
			else if ((e1[i] <= 90 && e1[i]>=65) || (e[i] <= 90 && e[i]>=65)||(e1[i] <= 122 && e1[i] >= 97)||(e[i] <= 122 && e[i] >= 97)) {//在第5个及以后的不相等的是否为字母,是:0;不是1;
				return 0;
			}
		}
	}
	return 1;
}//

int ReplaceWord(char f[], string fh) {//调整两个相同单词的显示顺序,即将靠后的单词替换为靠前的单词   已改 使得相同字符也进行排序
	const char *f1;
	int k,l;
	unsigned long j;
	j = 0;
	unsigned long n=0,m=0;
	unsigned long g = 0;
	char str1[1024], str2[1024];
	char ch1[1024], ch2[1024];
	unsigned long temp,temp1, temp2;//单词长度
	f1 = fh.c_str();
	temp1 = strlen(f);
	temp2 = strlen(f1);
	if (temp1 > temp2) temp = temp1;
	else temp = temp2;
	while (f1[g] - 32 != f[g] && f1[g] + 32 != f[g] && f1[g] != f[g]) {
		g++;
		j = g;
	}
	for (unsigned int i = 0; i < j; i++) {//j之前为相同的字符
		ch1[m] = f[i];
		ch2[m] = fh[i];
		m++;
	}
	l= strcmp(ch1, ch2);
	if (l == 0) {
		for (unsigned int i = j; i < temp; i++) {
			str1[n] = f[i];
			str2[n] = f1[i];
			n++;
		}
		k = strcmp(str1, str2);
		return k;
	}
	else return l;
}

void BulidHaffWord(char d[],struct HaffWordTable H[]) {//构造哈希表       已改将大小写统一用小写计算哈希函数
	unsigned long long k;
	int l,t;
	unsigned long long p;
	unsigned long long q;
	char a[4];
	for (int i=0; i < 4; i++) {//(ch[n] <= 90 || ch[n] >= 65)
		if (d[i] <= 90 && d[i] >= 65) {
			a[i] = d[i] + 32;
		}
		else a[i] = d[i];
	}
	p = a[0] + a[1] + a[2] + a[3] + a[4];//+ d[5] + d[6] + d[7] + d[8] + d[9] + d[10] + d[11] + d[12] + d[1] + d[14] + d[15] + d[16] + d[17] + d[18] + d[19];
	q = p * p;
	k = q % 4096;//(q >> 26) & 0xfff;
	if (H[k].HT.empty()== 1) {
		H[k].HT = d;
		
		H[k].word_apearnum = 1;//单词录入
	}
	else {
		if (Compareword(d, H[k].HT) == 1) {///如果有相同的单词;
			H[k].word_apearnum++;
			l = ReplaceWord(d, H[k].HT);//判断先后
			if (l < 0) {
				H[k].HT = d;
			}
			return;
		}
		if ((p % 2) == 0) {//构造增量
			t = (p + 1) % 4096;
		}
		else {
			t = p % 4096;
		}
		k = (k + t) % 4096;//新的地址
		while (H[k].HT.empty() != 1) {//没有空位 
			
			if (Compareword(d, H[k].HT) == 1) {///如果有相同的单词;
				H[k].word_apearnum++;
				l = ReplaceWord(d, H[k].HT);//判断先后
				if (l < 0) {
						H[k].HT = d;
						
				   }
				return;
				}
			k = (k + t) % 4096;
			}
		H[k].HT = d;//有空位置
		H[k].word_apearnum ++;//单词录入
	}
}

int ComparePrase(char ch[], string l) {//比较两个词组是否相同 已改 改了寻找词组空格点
	const char *ch1;
	unsigned long a;//ch
	unsigned long b;//ch1
	unsigned long n = 0;//ch循环变量
	unsigned long m = 0;//ch1循环变量
	unsigned long i = 0;
	ch1 = l.c_str();
	do {//词组空格点
		
			a = i;
			
		
	} while (ch[i] != ' ');
	i = 0;
	do {//词组空格点
		
			b = i;
			
		
	} while (ch1[i] != ' ');
	
	do {//比较第一个单词是否一样
		if (ch[n] != ch1[m]&& ch[n] +32 != ch1[m]&& ch[n] -32 != ch1[m]) {
			if (n < 3) return 0;
			else if ((ch[n] <= 90 && ch[n] >= 65) || (ch1[m] <= 90 && ch1[m] >= 65) || (ch[n] <= 122 && ch[n] >= 97) || (ch1[m] <= 122 && ch1[m] >= 97)) {
				return 0;
			}
		}
		n++; m++;
	} while (n<a||m<b);
	if (n == a) {//如果ch数组先结束第一个词
		for (; m < b; m++) {
			if ((ch1[m] <= 90 && ch1[m] >= 65) || (ch1[m] <= 122 && ch1[m] >= 97)) {//含有字母
				return 0;
			}
		}
	}
	else if (m == b) {//如果ch1数组先结束第一个词
		for (; n < a; n++) {
			if ((ch[n] <= 90 && ch[n] >= 65) || (ch[n] <= 122 && ch[n] >= 97)) {//含有字母
				return 0;
			}
		}
	}
	n++; m++;
	do {//比较第二个词,此时n=a+1,m=b+1;,为单词首地址
		if (ch[n] != ch1[m]|| ch[n] +32 != ch1[m]|| ch[n] -32 != ch1[m]) {
			if (n-a < 5) return 0;
			else if ((ch[n] <= 90 && ch[n] >= 65) || (ch1[m] <= 90 && ch1[m] >= 65) || (ch[n] <= 122 && ch[n] >= 97) || (ch1[m] <= 122 && ch1[m] >= 97)) {
				return 0;
			}
		}
		n++; m++;
	} while (ch[n]==NULL||ch1[m]==NULL);
	if (ch[n] == NULL) {
		do {
			if ((ch1[m] <= 90 && ch1[m] >= 65) || (ch1[m] <= 122 && ch1[m] >= 97)) {//含有字母
				return 0;
			}
			m++;
		} while (ch1[m] == NULL);
	}
	else if (ch1[m] == NULL) {
		do {
			if ((ch[n] <= 90 && ch[n] >= 65) || (ch[n] <= 122 && ch[n] >= 97)) {//含有字母
				return 0;
			}
			n++;
		} while (ch[n] == NULL);
	}
	return 1;
}

int ReplacePrase(char ch[], string l) { //调整两个相同词组的显示顺序,即将靠后的词组替换为靠前的单词  已改 排除了pass234 good234和Pass234 good234等的排序问题
	const char *l1;
	int  k,g,s;
	unsigned long j;
	unsigned long n = 0;
	unsigned long a, b;
	char str1[1024], str2[1024], str3[1024], str4[1024];
	char ch1[1024], ch2[1024], ch3[1024], ch4[1024];
	unsigned long p, q;
	unsigned long i=0;
	unsigned long temp, temp1, temp2;
	unsigned long m = 0;
	l1 = l.c_str();
	do {//词组空格点

		a = i;


	} while (ch[i] != ' ');
	i = 0;
	do {//词组空格点

		b = i;


	} while (l1[i] != ' ');
	temp1 = strlen(ch);
	temp2 = strlen(l1);
	if (temp1 > temp2) temp = temp1;
	else temp = temp2;
	do {
		i++;
		j = i;
	} while (l1[i] - 32 != ch[i] && l1[i] + 32 != ch[i] && l1[i] != ch[i]);//找到不相等的地址j
	for (unsigned int o = 0; o < j; o++) {//j之前为相同的字符
		ch1[m] = ch[i];
		ch2[m] = l1[i];
		m++;
	}
	g = strcmp(ch1, ch2);
	if (g == 0) {
		p = j;
		q = j;
		do {
			str1[n] = ch[p];
			str2[n] = l1[q];
			n++; p++; q++;
		} while (p < a || q < b);
		if (strcmp(str1, str2) == 0) {//如果相同则判断哪个先结束
			if (p == a) return -1;
			else if (q == b) return 1;
			else {//说明前面的完全一样
				
				
				
				for (unsigned int i = a + 1; i < temp; i++) {//对第二个单词进行比较
					if (l1[i] - 32 != ch[i] && l1[i] + 32 != ch[i] && l1[i] != ch[i]) {
						j = i;//找到不相同的地方
						break;
					}
				}
				m = 0;
				for (unsigned int o = a+1; o < j; o++) {//j之前为相同的字符
					ch3[m] = ch[i];
					ch4[m] = l1[i];
					m++;
				}
				s = strcmp(ch3, ch4);
				if (s == 0) {
					n = 0;
					for (unsigned int i = j; i < temp; i++) {//将不同的字符串进行比较
						str3[n] = ch[i];
						str4[n] = l1[i];
						n++;
					}
					k = strcmp(str1, str2);
					return k;
				}
				return s;
			}
		}
		k = strcmp(str1, str2);
		return k;
	}
	
	return g;
}

void PraseNumber(char f[]) {//判断单词后是否有单词,即判断词组  已改取消了2pass类似的情况
	unsigned long i;//总循环
	unsigned long j;//第二个单词的循环
	unsigned long k;//第二个单词在f数组的首地址
	unsigned long n;//第一个单词在f数组的首地址 
	unsigned long m;//控制数组b的循环
	unsigned long l;//第一次分隔符b数组的地址
	unsigned long o;//第二次分隔符b数组的地址
	struct HaffPraseTable;
	unsigned long temp;
	temp = strlen(f);
	k = 0; l = 0;
	char b[4096] = "";
	for (i = 0; i < temp; i++) {
		m = 0;
		if ((f[i] >= 65 && f[i] <= 90) || (f[i] >= 97 && f[i] <= 122)) {
			if (i == 0 || ((f[i-1] < 48) || (f[i-1] > 57 && f[i-1] < 65) || (f[i-1] > 90 && f[i-1] < 97) || (f[i-1] > 122))) {
				if ((f[i + 1] >= 65 && f[i + 1] <= 90) || (f[i + 1] >= 97 && f[i + 1] <= 122)) {
					if ((f[i + 2] >= 65 && f[i + 2] <= 90) || (f[i + 2] >= 97 && f[i + 2] <= 122)) {
						if ((f[i + 3] >= 65 && f[i + 3] <= 90) || (f[i + 3] >= 97 && f[i + 3] <= 122)) {//有单词出现
							j = i + 3; l = 3;
							do {
								j++;
								l++;//第一个单词后分隔符下标
							} while ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122));//出现分隔符则停止,j为分隔符的坐标
																								//k=j;//第一个单词后分隔符下标
																								//b[k] = ' ';
							for (j; j < 1024; j++) {
								if ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122)) {//下一个单词可能要出现了
									break;
								}
							}
							k = j;//将第二个单词首地址给k
							if ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122)) {
								j++;
								if ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122)) {
									j++;
									if ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122)) {
										j++;
										if ((f[j] >= 65 && f[j] <= 90) || (f[j] >= 97 && f[j] <= 122)) {//单词之后出现单词,发现词组
																										//b[k] = ' ';
											n = i;//第一个单词首地址
											do {
												j++;
											} while ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122));//出现分隔符则停止,j为分隔符的坐标;
											o = j - i + 1;//第二个单词的分隔符在b数组中的地址
											do {
												b[m] = f[n];
												n++; m++;
											} while (m < l);//当为第一次遇见分隔符时;
											b[m] = ' '; m++;
											do {
												b[m] = f[k];
												k++; m++;
											} while (m < o);
											//b数组里存好了词组;
											BulidHaffPrase(b, Hp, l);
										}
										else {//词组查找失败,将第一个单词之后的地址返回i
											if ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122)) {//如果是分隔符就返回
												i = j;
											}
											do {
												j++;
											} while ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122));//寻找到下一个分隔符地址
											i = j;
										}
									}
									else {//词组查找失败,将第一个单词之后的地址返回i
										if ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122)) {//如果是分隔符就返回
											i = j;
										}
										do {
											j++;
										} while ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122));//寻找到下一个分隔符地址
										i = j;
									}
								}
								else {//词组查找失败,将第2个单词之后的分隔符地址返回i
									if ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122)) {//如果是分隔符就返回
										i = j;
									}
									do {
										j++;
									} while ((f[j] < 48) || (f[j] > 57 && f[j] < 65) || (f[j] > 90 && f[j] < 97) || (f[j] > 122));//寻找到下一个分隔符地址
									i = j;
								}
							}
						}
					}
				}
			}
		}
	}//for




	return;
}

void BulidHaffPrase(char d[], struct HaffPraseTable Hp1[], unsigned long a) { //构造哈希表      已改 将大小写的字母统一为小写进行哈希函数
	unsigned long long k;
	unsigned long  t;
	int l;
	unsigned long long p;
	unsigned long long q;
	char b[8];
	for (int i=0; i < 4; i++) {
		if(d[i]>=65&&d[i]<=90)  b[i] = d[i] + 32;
		else b[i] = d[i];
	}
	for (int i = 0; i < 4; i++) {
		if (d[i+4] >= 65 && d[i+4] <= 90) b[i+4] = d[a+i+1] + 32;
		else b[i+4] = d[a+i+1];

	}
	p = b[0] + b[1] + b[2] + b[3] +b[4] + b[5] + b[6] + b[7] + b[8];
	q = p * p;
	k = q % 4096;
	if (Hp1[k].HPT.empty() == 1 ) {//|| Hp1[k].HPT == "-1"
		Hp1[k].HPT = d;
		Hp1[k].Prase_apearnum = 1;//词组录入
	}
	else {
		if ((p % 2) == 0) {//构造增量
			t = (p + 1) % 4096;
		}
		else {
			t = p % 4096;
		}
		k = (k + t) % 4096;//新的地址
		while (Hp1[k].HPT.empty() != 1 ) {//没有空位&& Hp1[k].HPT != "-1"

			if (ComparePrase(d, Hp1[k].HPT) == 1) {///如果有相同的词组;      已改
				Hp1[k].Prase_apearnum++;
				l = ReplacePrase(d, Hp1[k].HPT);//判断先后     已改
				if (l < 0) {
					Hp1[k].HPT = d;
				}
				return;
			}
			k = (k + t) % 4096;
		}
		Hp1[k].HPT = d;//有空位置
		Hp1[k].Prase_apearnum ++;//词组录入
	}
}

struct Praseten sorting_p() {//对哈希表里的数据进行排序
	unsigned long b[100000] = {0};
	struct Praseten sp;
	string c[100000] = { "" };
	unsigned long temp_b;
	string temp_c;
	for (int i = 0; i < 100000; i++) {
		b[i] = H[i].word_apearnum;
	}
	for (int i = 0; i < 10000; i++) {
		c[i] = H[i].HT;
	}
	for (int i = 0; i < 10000 - 1; i++) {
		for (int j = 0; j < 100000 - 1 - j; j++) {
			if (b[j]< b[j+1]) {
				temp_b = b[j];
				b[j] = b[j + 1];
				b[j + 1] = temp_b;
				temp_c = c[j];
				c[j] = c[j + 1];
				c[j + 1] = temp_c;
			}
		}
	}//将b与c数组进行排序
	for (int i = 0; i < 10; i++) {
		sp.str_p[i] = c[i];
	}
	for (int i = 0; i < 10; i++) {
		sp.num_p[i] = b[i];
	}


	return sp;
}


struct Wordten sorting_w() {//对哈希表里的数据进行排序
	unsigned long b[100000] = { 0 };
	struct Wordten sp;
	string c[100000] = { "" };
	int temp_b;
	string temp_c;
	for (int i = 0; i < 100000; i++) {
		b[i] = Hp[i].Prase_apearnum;
	}
	for (int i = 0; i < 100000; i++) {
		c[i] = Hp[i].HPT;
	}
	for (int i = 0; i < 100000 - 1; i++) {
		for (int j = 0; j < 100000 - 1 - j; j++) {
			if (b[j]< b[j + 1]) {
				temp_b = b[j];
				b[j] = b[j + 1];
				b[j + 1] = temp_b;
				temp_c = c[j];
				c[j] = c[j + 1];
				c[j + 1] = temp_c;
			}
		}
	}//将b与c数组进行排序
	for (int i = 0; i < 10; i++) {
		sp.str_w[i] = c[i];
	}
	for (int i = 0; i < 10; i++) {
		sp.num_w[i] = b[i];
	}


	return sp;
}

//int argc, const string *argv[]

int main(int argc, const string *argv[])
{
	struct Praseten P;
	struct Wordten W;
	string filename;
	filename = *argv[1];
	ofstream outfile("result.txt");
	unsigned long word_number = 0;
	unsigned long char_number = 0;
	unsigned long line_number = 0;
	dfsFolder(filename,line_number,char_number,word_number);//统计行数统计字符
	outfile <<" char_number:"<< char_number << endl;
	outfile <<"line_number:"<< line_number << endl;
	outfile <<"word_number:"<<word_number << endl;
	P = sorting_p();
	W = sorting_w();
	outfile << "the top ten frequency of word :" << endl;
	for (int i = 0; i < 10; i++) {
		outfile << P.str_p[i]<< ": ";
		outfile << P.num_p[i]<< endl;
	}
	outfile << "the top ten frequency of phrase :" << endl;
	for (int i = 0; i < 10; i++) {
		outfile << W.str_w[i]<< ": "; 
		outfile << W.num_w[i]<< endl; 
	}
	system("pause");
    return 0;
	
}
