#include "stdafx.h"
#include <iostream>
#include <string.h>        // for strcpy(), strcat()
#include <io.h>

using namespace std;

void listFiles(const char * dir);

int main()
{
	char dir[200];
	cout << "Enter a directory: ";
	cin.getline(dir, 200);

	listFiles(dir);

	system("pause");
	return 0;
}

void listFiles(const char * dir)
{
	char dirNew[200];
	strcpy(dirNew, dir);
	strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst(dirNew, &findData);
	if (handle == -1)        // 检查是否成功
		return;

	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			cout << findData.name << "\t<dir>\n";

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			strcpy(dirNew, dir);
			strcat(dirNew, "\\");
			strcat(dirNew, findData.name);

			listFiles(dirNew);
		}
		else
			//cout << findData.name << "\n";

	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
}
