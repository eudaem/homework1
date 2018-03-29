#define _CRT_SECURE_NO_WARNINGS
#include <cstring>                              // for strcpy(), strcat()
#include<stdio.h>
#include<malloc.h>

#include<dirent.h>
#include<unistd.h> 
#include<stdlib.h>
#include<iostream>
#define word 400
#define pathlong 200
#define Max 3000000
#define frequecy_count 10000
#define hash_size 456976
int linenumber = 0, wordnum = 0;

int filenumber = 0;
typedef struct data{
  char dir[256];
}data;
data path[2000];


typedef struct _NODE
{
	char danci[word] ;
	int frequency ;
	struct _NODE* next = NULL;
}NODE;

typedef struct _HASH_TABLE 
{
	NODE* value[hash_size];
}HASH_TABLE;

HASH_TABLE*pHashTbl;

HASH_TABLE*pHashTbl_group;

HASH_TABLE* create_hash_table()
{
	HASH_TABLE* pHashTbl = (HASH_TABLE*)malloc(sizeof(HASH_TABLE));
	memset(pHashTbl, 0, sizeof(HASH_TABLE));
	return pHashTbl;
}

int insert_data_into_hash(HASH_TABLE* pHashTbl, int data, int wordlength, char danci[])
{
	NODE* pNode;
	if (NULL == pHashTbl)
		return -1;
	if (NULL == pHashTbl->value[data]) {
		pNode = (NODE*)malloc(sizeof(NODE));
		memset(pNode, 0, sizeof(NODE));
		strcpy(pNode->danci, danci);
		pNode->frequency = 1;
		pNode->next = NULL;
		pHashTbl->value[data] = pNode;
		return 1;
	}

    pNode = pHashTbl->value[data];
	while (NULL != pNode->next)
		pNode = pNode->next;

	pNode->next = (NODE*)malloc(sizeof(NODE));
	memset(pNode->next, 0, sizeof(NODE));
	strcpy(pNode->next->danci, danci);
	pNode->next->frequency = 1;
	pNode->next->next = NULL;
	pNode = pNode->next;
	return 1;
}


int find_data_in_hash(HASH_TABLE* pHashTbl, int data, int wordlength, char danci[], char smallword[], int onlyfind)
{
	NODE* pNode;
	int j, flagend = 0, insertornot = 0;
	
	if (NULL == pHashTbl)
		return -1;
	
	if (NULL == (pNode = pHashTbl->value[data]))
	{
		insertornot = insert_data_into_hash(pHashTbl, data, wordlength, danci);
		if (insertornot == -1)
			printf("insert error");
		return 1;
	}
	
	while (pNode) {
		
		for (j = wordlength - 1; j >= 0; j--) {  
			if ((danci[j] <= 122 && danci[j] >= 97) || (danci[j] <= 90 && danci[j] >= 65))
			{
				flagend = j;
				break;
			}

		}
		
		for (j = word - 1; j > 0; j--) {
			if ((pNode->danci[j] <= 122 && pNode->danci[j] >= 97) || (pNode->danci[j] <= 90 && pNode->danci[j] >= 65))
			{
				if (flagend < j)
				{
					flagend = j;
					break;
				}
			}
		}
		
		if (strncasecmp(pNode->danci, danci, flagend + 1) == 0)
		{
			if (onlyfind != 1)
			{
				pNode->frequency++;
				
			}
			if (strcmp(pNode->danci, danci) > 0)
				strcpy(pNode->danci, danci);

			strcpy(smallword,pNode->danci);
			return 2;
		}
		else if (pNode->next == NULL)
		{
			insertornot = insert_data_into_hash(pHashTbl, data, wordlength, danci);
			if (insertornot == -1)
				printf("insert error");
			return 1;
		}
		
		pNode = pNode->next;
		
	}
	return 1;

}





void scanFile(char *szDir)  
{  
    DIR *pDir = NULL;  
    struct dirent *pFile = NULL;  
    int i=0;
    char dirnew[pathlong]={0};
    pDir = opendir(szDir);  
    if (pDir == NULL) return;  
      
    while ((pFile = readdir(pDir)) != NULL) {  
        if (pFile->d_type & DT_DIR) {  
            if (strcmp(pFile->d_name, ".") == 0   
                || strcmp(pFile->d_name, "..") == 0) continue;  
              
            char Path[pathlong];  
            int len = strlen(szDir);  
            strncpy(Path, szDir, len + 1);  
            if (szDir[len - 1] != '/') strncat(Path, "/", 2);  
            strncat(Path, pFile->d_name, strlen(pFile->d_name) + 1);  
            scanFile(Path);  
        } else {  
           // printf("path:%s     fileName:%s\n", szDir, pFile->d_name);  
            strcpy(dirnew,szDir);
            strcat(dirnew,"/");
            strcat(dirnew,pFile->d_name);
            strcpy(path[filenumber++].dir,dirnew);
           // printf("my path is %s",path[filenumber-1].dir);
           
        }  
    }  
      
    closedir(pDir);  
}

int insertgroup(HASH_TABLE* pHashTbl, int key, char group[])
{
	NODE* pNode;
	if (NULL == pHashTbl)
		return -1;
	
	if (NULL == pHashTbl->value[key]) {
		pNode = (NODE*)malloc(sizeof(NODE));
		memset(pNode, 0, sizeof(NODE));
		strcpy(pNode->danci,group);
		pNode->frequency = 1;
		pNode->next = NULL;
		pHashTbl->value[key] = pNode;
		return 1;
	}

	

	pNode = pHashTbl->value[key];
	while (NULL != pNode->next)
		pNode = pNode->next;

	pNode->next = (NODE*)malloc(sizeof(NODE));
	memset(pNode->next, 0, sizeof(NODE));
	strcpy(pNode->next->danci,group);
	pNode->next->frequency = 1;
	pNode->next->next = NULL;
	pNode = pNode->next;
	
	return 1;
}


int findgroup(HASH_TABLE* pHashTbl_group, int key, char group[])
{
	NODE* pNode;
	char smallword[word] = { 0 }, group_new[word] = { 0 };
	int  flagend = 0, insertornot = 0, findornot = 0, data_record = 0;
	int onlyfind = 1,data = 0,qianwei=0, baiwei = 0, gewei = 0, shiwei = 0;
	if (NULL == pHashTbl)
		return -1;
	

    if (NULL == (pNode = pHashTbl_group->value[key]))
	{
		insertornot = insertgroup(pHashTbl_group, key, group);
		if (insertornot == -1)
			printf("insert error");
		return 1;
	}
	while (pNode) {

		if (strcmp(pNode->danci, group) == 0)
		{
			pNode->frequency++;


			return 2;
		}
		else if (pNode->next == NULL)
		{
			insertornot = insertgroup(pHashTbl_group, key, group);
			if (insertornot == -1)
				printf("insert error");
			return 1;
		}
		
		pNode = pNode->next;
		
	}
	return 1;

}

int calculate_key(char temp[]) {
	int qianwei = 0, baiwei = 0, shiwei = 0, gewei = 0,data=0;
	if (temp[0] <= 90 && temp[0] >= 65)
		qianwei = temp[0] + 32;
	else
		qianwei = temp[0];
	if (temp[1] <= 90 && temp[1] >= 65)
		baiwei = temp[1] + 32;
	else
		baiwei = temp[1];
	if (temp[2] <= 90 && temp[2] >= 65)
		shiwei = temp[2] + 32;
	else
		shiwei = temp[2];
	if (temp[3] <= 90 && temp[3] >= 65)
		gewei = temp[3] + 32;
	else
		gewei = temp[3];

	data = (qianwei - 97) * 17576 + (baiwei - 97) * 676 + (shiwei - 97) * 26 + gewei - 97;
	if(data>=0&&data<hash_size)
	return data;
        else
        return -1;
}

int calculate_key_group(char record[], char temp[]) {
	int qianwei_group = 0, baiwei_group = 0,shiwei_group = 0, gewei_group = 0,key=0;
	if (record[0] <= 90 && record[0] >= 65)
		qianwei_group = record[0] + 32;
	else
		qianwei_group = record[0];
	if (record[1] <= 90 && record[1] >= 65)
		baiwei_group = record[1] + 32;
	else
		baiwei_group = record[1];
	if (temp[0] <= 90 && temp[0] >= 65)
		shiwei_group = temp[0] + 32;
	else
		shiwei_group = temp[0];
	if (temp[1] <= 90 && temp[1] >= 65)
		gewei_group = temp[1] + 32;
	else
		gewei_group = temp[1];
	key = (qianwei_group - 97) * 17576 + (baiwei_group - 97) * 676 + (shiwei_group - 97) * 26 + gewei_group - 97;
	if (key>=0&&key<hash_size)
        return key;
        else
        return -1;
}

int count(FILE*fp, HASH_TABLE*pHashTbl) {                          

	char  ch, temp[word] = { 0 }, smallword[word] = { 0 };

	int charnumber = 0, i = 0, j = 0, flag = 0;
	int shiwei = 0, gewei = 0, data = 0, wordlength = 0, findornot, baiwei = 0,qianwei=0;
	charnumber = 0;

	while (!feof(fp))
	{
               // printf("char is %c and ASCII is %d\n",ch,ch);
		ch = fgetc(fp);
		
			if ((ch >= 48 && ch <= 57) || (ch >= 97 && ch <= 122) || (ch >= 65 && ch <= 90))
			{
				temp[i++] = ch;
				charnumber++;
			}
			else
			{
				if (ch == '\n')
					linenumber++;
				else if(ch>31&&ch<=127)
					charnumber++;
				if (i<4 || (temp[0] <= 57 && temp[0] >= 48) || (temp[1] <= 57 && temp[1] >= 48) || (temp[2] <= 57 && temp[2] >= 48) || (temp[3] <= 57 && temp[3] >= 48))
				{
					for (j = 0; j < word; j++) 
						temp[j] = '\0';
					i = 0;
				}
				else {
                                        data=calculate_key(temp);
                                        
					wordlength = i;
                                        if(data>=0&&data<hash_size)
					findornot = find_data_in_hash(pHashTbl, data, wordlength, temp, smallword, 0);
					if (findornot == -1)
						printf("hash is empty");
					
					for (j = 0; j < word; j++)
						temp[j] = '\0';
					i = 0;
				}
			}
		
                

	}
	if (i >= 4)
	{
		
		data=calculate_key(temp);
                printf("temp is %s data is %d",temp,data);
		wordlength = i;
                if(data>=0&&data<hash_size)
		findornot = find_data_in_hash(pHashTbl, data, wordlength, temp, smallword, 0);
		if (findornot == -1)
			printf("hash is empty");
		
		for (j = 0; j < word; j++)
			temp[j] = '\0';
	}


	return charnumber;
}


int count_group(FILE*fp, HASH_TABLE*pHashTbl_group) {                             
	char  ch, temp[word] = { 0 }, record[word] = { 0 }, smallword[word] = { 0 }, group[word] = { 0 };

	int charnumber = 0, i = 0, j = 0, flag = 0,  data = 0, wordlength = 0, findornot;
	charnumber = 0;
	
	int key = 0,  flag_group = 0;

	while (!feof(fp))
	{

		ch = fgetc(fp);
		
			if ((ch >= 48 && ch <= 57) || (ch >= 97 && ch <= 122) || (ch >= 65 && ch <= 90))
			{
				temp[i++] = ch;

             }
			else
			{

				if (i<4 || (temp[0] <= 57 && temp[0] >= 48) || (temp[1] <= 57 && temp[1] >= 48) || (temp[2] <= 57 && temp[2] >= 48) || (temp[3] <= 57 && temp[3] >= 48))
				{
					for (j = 0; j < word; j++) 
						temp[j] = '\0';
					i = 0;
				}
				else {
					

					data=calculate_key(temp);
					wordlength = i;
                                        if(data>=0&&data<hash_size);
					findornot = find_data_in_hash(pHashTbl, data, wordlength, temp, smallword, 1);
					if (findornot == 2)
						strcpy(temp, smallword);
					if (flag_group == 0) {

						strcpy(record, temp);

					}


					else {
						key=calculate_key_group(record,temp);
						strcpy(group, record);
						strcat(group, " ");
						strcat(group, temp);
						if(key>=0&&key<hash_size)
						findgroup(pHashTbl_group, key, group);
						strcpy(record, temp);

						for (j = 0; j < word; j++)
							smallword[j] = '\0';
					}
					for (j = 0; j < word; j++)
						temp[j] = '\0';
					i = 0;

					flag_group++;
				}
			}
		}

	
	if (i >= 4)
	{
		data=calculate_key(temp);
		wordlength = i;
                if(data>=0&&data<hash_size)
		findornot = find_data_in_hash(pHashTbl, data, wordlength, temp, smallword, 1);
		if (findornot == -1)
			printf("hash is empty");
		

		if (flag_group != 0) {
			if (findornot == 2)
				strcpy(temp, smallword);
			key=calculate_key_group(record,temp);
			strcpy(group, record);
			strcat(group, " ");
			strcat(group, temp);
			//printf("group is %s\n", group);
                        if(key>=0&&key<hash_size)
			findgroup(pHashTbl_group, key, group);
			strcpy(record, temp);

		}
		for (j = 0; j < word; j++)
			temp[j] = '\0';
	}


	return flag_group;
}
int partition(int arr[], int low, int high) {
	int key;
	key = arr[low];
	
	while (low<high) {
		while (low <high && arr[high] >= key)
			high--;
		if (low<high)
			arr[low++] = arr[high];
		while (low<high && arr[low] <= key)
			low++;
		if (low<high)
			arr[high--] = arr[low];
	}
	arr[low] = key;

	return low;
}


void quick_sort(int arr[], int start, int end) {
	int pos;
	
	if (start<end) {
		//printf("start quick sort\n");
		pos = partition(arr, start, end);
		quick_sort(arr, start, pos - 1);
		quick_sort(arr, pos + 1, end);
	}

	return;
}

void printTOP10word(HASH_TABLE*pHashTbl, int frequency_value[], int k, FILE*fp_result) {

	int j = 0, m = 0, i = 0, count = 0;
	NODE * pNode = NULL;
	char TOPword[word] = { 0 };
	fprintf(fp_result, "top 10 words are :\n");
	
	for (i = k; i > k - 11 && i >= 0 && count <= 10; i--)
	{
		for (j = 0; j < hash_size && count <= 10; j++) {
			if (pHashTbl->value[j] != NULL) {
				pNode = pHashTbl->value[j];
				while (pNode != NULL) {
					if (pNode->frequency == frequency_value[i]) {

						if (count >= 10)
							break;
						else
						{


							strcpy(TOPword, pNode->danci);
							printf(" %s  : %d\n\n", TOPword, frequency_value[i]);
							fprintf(fp_result, " \n%s :  %d\n", TOPword, frequency_value[i]);
							count++;
						}

					}
					pNode = pNode->next;
				}
			}
		}
	}


}

void printTOP10group(HASH_TABLE*pHashTbl_group, int frequency_value[], int k, FILE*fp_result) {

	int j = 0, m = 0, i = 0, count = 0;
	NODE * pNode = NULL;
	char TOPword[word] = { 0 };
	fprintf(fp_result, "top 10 phrases are :\n");
	for (i = k; i > k - 11 && i >= 0 && count <= 10; i--)
	{
		for (j = 0; j < hash_size; j++) {
			if (pHashTbl_group->value[j] != NULL) {
				pNode = pHashTbl_group->value[j];
				while (pNode != NULL) {
					if (pNode->frequency == frequency_value[i]) {

						if (count >= 10)
							break;
						else
						{


							strcpy(TOPword, pNode->danci);
							printf(" %s  : %d\n\n", TOPword, frequency_value[i]);
							fprintf(fp_result, " \n%s :  %d\n", TOPword, frequency_value[i]);
							count++;
						}

					}
					pNode = pNode->next;
				}
			}
		}
	}


}
int main(int argc, char *argv[])
{
	
	char dir[500] = { 0 };                           //要读取的初始路径
	char buffer[100];                //用于存放当前目录路径
	int i = 0, charnumber = 0, j = 0, m = 0, Fileornot = 0, k = 0, k_group = 0;    //用于循环
	int frequecy[frequecy_count] = { 0 }, frequecy_group[frequecy_count] = { 0 };
	int filenum = 0, total_charnumber = 0, total_word = 0, total_group = 0, groupnumber = 0;            //用于记录，filenumber变量会在WriteAllFile后变为-1
	FILE *fp_dir, *fp_test, *fp_single, *fp_result, *fp_single2, *fp_test2, *fp_dir2;              //用于打开dir.txt，其中存放了文件夹下所有文件名字
	char file_path[pathlong] = { 0 };                     //用于存放dir.txt中每一行所代表的文件路径
	char file_path_new[pathlong] = { 0 };
	char file_path_test[pathlong] = { 0 };
	char TOPword[word] = { 0 }, TOPgroup[word] = { 0 };
	pHashTbl = create_hash_table();
	pHashTbl_group = create_hash_table();
	NODE * nodeprint, *nodecount, *nodeprint_group,*nodeprint_group1;
	//printf("%d", argc);
	strcpy(dir, argv[1]);
        scanFile(dir);
	//Fileornot = FileSearch(dir);
        printf("filenumberis %d",filenumber);
	if (filenumber == 0) {                        //-1说明输入路径是一个文档而非文件夹

		fp_single = fopen(dir, "r");
		fp_single2 = fopen(dir, "r");
		if (fp_single == NULL)
			printf("fail to open the file\n");
		else
			printf("open success");
		charnumber = count(fp_single, pHashTbl);
		groupnumber = count_group(fp_single2, pHashTbl_group);
		if (charnumber != 0)
			linenumber++;
		total_charnumber = charnumber;
		printf("total char is %d  total line is%d ", charnumber, linenumber);
		fclose(fp_single);
		fclose(fp_single2);

	}


	else {
		

		for (i = 0; i < filenumber; i++)
		{

			         


                       printf("path: %s\n",path[i].dir);
			fp_test = fopen(path[i].dir, "r+");                     //打开成功返回0

			if (fp_test == NULL) {

				printf("open file failed");


			}

			charnumber = count(fp_test, pHashTbl);

			if (charnumber != 0)
				linenumber++;//统计处理
			total_charnumber = total_charnumber + charnumber;


			fclose(fp_test);
			}


		for (i = 0; i < filenumber; i++)
		{
                        printf("path: %s\n",path[i].dir);
			fp_test2= fopen(path[i].dir, "r+");
														

			
			

			if (fp_test == NULL) {

				printf("open file failed");


			}


			groupnumber = count_group(fp_test2, pHashTbl_group);


			fclose(fp_test2);
			
			

		}


	}

	for (j = 0; j < hash_size; j++) {
		if (pHashTbl->value[j] != NULL)
		{
			nodeprint = pHashTbl->value[j];
			while (nodeprint != NULL)
			{

				//printf("we have word %s conut is %d\n", nodeprint->danci, nodeprint->frequency);
				total_word = total_word + nodeprint->frequency;
				nodeprint = nodeprint->next;
			}

		}

	}

	for (j = 0; j < hash_size; j++) {
		if (pHashTbl->value[j] != NULL) {
			nodecount = pHashTbl->value[j];
			while (nodecount != NULL) {
				for (m = 0; m < frequecy_count; m++)
					if (nodecount->frequency == frequecy[m])
						break;
				if (m == frequecy_count)
					frequecy[k++] = nodecount->frequency;
				nodecount = nodecount->next;
			}
		}
	}


	//for (m = 0; m < k; m++)
	//printf("frequecy is %d \n ", frequecy[m]);
	quick_sort(frequecy, 0, k - 1);
	//printf("how many frequecy? %d\n", k);
	//for (m = 0; m < k; m++)
	//printf("frequecy is %d \n ", frequecy[m]);

	for (j = 0; j < hash_size; j++) {
		if (pHashTbl_group->value[j] != NULL) {
			nodeprint_group = pHashTbl_group->value[j];
			while (nodeprint_group != NULL) {
				for (m = 0; m < frequecy_count; m++)
					if (nodeprint_group->frequency == frequecy_group[m])
						break;
				if (m == frequecy_count)
					frequecy_group[k_group++] = nodeprint_group->frequency;
				nodeprint_group = nodeprint_group->next;
			}
		}
	}
	quick_sort(frequecy_group, 0, k_group - 1);
	fp_result = fopen("result.txt", "w");
	if (fp_result == NULL)
		printf("open result.txt failed\n");
	fprintf(fp_result, "char_number :%d\n", total_charnumber);
	fprintf(fp_result, "line_number :%d\n", linenumber);
	fprintf(fp_result, "word_number :%d\n", total_word);
	//fprintf(fp_result, "phrase_number :%d\n", total_group);
	printf("top 10 words are:\n");



	printTOP10word(pHashTbl, frequecy, k, fp_result);


	printf("top 10 phrases are:\n");



	printTOP10group(pHashTbl_group, frequecy_group, k_group, fp_result);


	fclose(fp_result);


	
	for (j = 0; j < hash_size; j++) {
	if (pHashTbl_group->value[j] != NULL)
	{
	nodeprint_group1 = pHashTbl_group->value[j];
	while (nodeprint_group1!= NULL)
	{

	//printf("we have group %s conut is %d\n", nodeprint_group1->danci, nodeprint_group1->frequency);
	total_group = total_group + nodeprint_group1->frequency;
	nodeprint_group1 = nodeprint_group1->next;
	}

	}
	}




	printf("total char is %d,line number is %d total word is %d\n\n", total_charnumber, linenumber, total_word);


	
	return 0;
}

