#define _CRT_SECURE_NO_WARNINGS 1
#include "Inflate.h"
#include "utils.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
int count = 0;
int g_char = 0;
int g_pointer = 0;
int all=0;
int main(int argc, char **argv)
{
// 	char szDir[256] = "E:\\Document\\Alexa\\Alexa.com-20170509\\compress";
// 	char szDir[256] = "E:\\Document\\Alexa\\Alexa.cn-20170222\\compress";
//	char szDir[256] = "E:\\Document\\Alexa\\test"; //待操作文件路径
	//char szDir[256] = "D:\\bmc\\test\\comp";
	//char szDir[256] = "D:\\bmc\\Alexa.com\\compress";
	//char szDir[256] = "D:\\bmc\\test\\comp";   //"HERE IS A..."
	//char szDir[256] = "D:\\bmc\\comp";
	char szDir[256] = "D:\\bmc\\Alexa.cn-20170222\\compress";
	char szFile[256];
	
	// Find the first file in the directory.
	WIN32_FIND_DATA ffd;
	CHAR szFindDirStr[MAX_PATH];
	StringCchCopy(szFindDirStr, MAX_PATH, szDir);
	StringCchCat(szFindDirStr, MAX_PATH, TEXT("\\*"));
	HANDLE hFind = FindFirstFile(szFindDirStr, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		return 0;

	int file_index = 0;
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		else
		{
			unsigned long dwRead = 0;
			char* pBuf = new char[ffd.nFileSizeLow];
			wsprintf(szFile, "%s\\%s", szDir, ffd.cFileName);
			HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				continue;

			ReadFile(hFile, pBuf, ffd.nFileSizeLow, &dwRead, NULL);//读文件
			CloseHandle(hFile);

			Inflate decode;
			ByteInfoList infoList;
			int nSize = decode.DecodeBuffer(pBuf, dwRead, &infoList);//解码后的大小 infoList代表文本信息
			int temp = nSize;//表示的是普通字符加上二元组表示的总的个数
		    //printf("%0.4f\n", (double)(nSize - commonSize) / nSize);
			//g_total += nSize - commonSize;
			//nSize - commonSize    pointer表示的字节
			
			g_char = 0;

			ScanFirst(infoList, nSize, ffd.cFileName);

			g_pointer += temp - g_char;
			all += temp;
			delete []pBuf;
		}
		//nSize是文本原来的大小 
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	
	printf("Prepare Data Finished\n");
	printf("PointerNumber:%d\n", count);
	printf("pointer表示的字节数:%d\n", g_pointer);
	printf("总的的字节数:%d\n", all);
	printf("pointer radio:%.6f\n",(double)g_pointer /all );
	printf("AveragePointerLength:%.2f\n", (double)(g_pointer) / count);
	return 0;
}
//strFile 写入的地址 desize解压之后的大小 
void ScanFirst(ByteInfoList &infoList, int desize, char *strFile)
{
	char fileSaved[256] = {0};
	//sprintf(fileSaved, "E:\\Document\\Alexa\\decomp-test/%s", strFile);
	wsprintf(fileSaved, "D:\\bmc\\Alexa.cn-20170222\\decompress1/%s", strFile);
	//sprintf(fileSaved, "D:\\bmc\\test\\decomp/%s", strFile);
	FILE *pfile = fopen(fileSaved, "wb");
	if(pfile == NULL)
	{
		printf("open saved file %s error\n", strFile);
		return;
	}

	int k = 0;
	int nSize = infoList.size();
	StoredInfo *info = new StoredInfo[desize];//存储信息的数组
	memset(info, 0, sizeof(StoredInfo)* desize);
	for (int i=0; i < nSize; i++) 
	{
		if (infoList[i].d.length == 0)	// Scan Lite
		{
			g_char++;
			info[k].token = infoList[i].t.token;
			k++;
		}
		else							// Scan DistCode 
		{
			
			
			int offset = k - infoList[i].d.dist;
 			info[k].length = infoList[i].d.length;
			info[k+1].dist = infoList[i].d.dist;

			int len = infoList[i].d.length;
			for(int j=0; j<len; j++, k++)//拷贝
			{		
				/*info[k].length = infoList[i].d.length-j;
				info[k].dist = infoList[i].d.dist;*/
				info[k].token = info[offset+j].token;
				if (j == len - 1) {
				
					count++;
				}
			}

			
			

		}
	}

	fwrite(info, sizeof(StoredInfo), k, pfile);
	fclose(pfile);
	delete []info;
}
