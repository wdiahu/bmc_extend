#include "Inflate.h"
#include "utils.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>

int main(int argc, char **argv)
{
// 	char szDir[256] = "E:\\Document\\Alexa\\Alexa.com-20170509\\compress";
// 	char szDir[256] = "E:\\Document\\Alexa\\Alexa.cn-20170222\\compress";
//	char szDir[256] = "E:\\Document\\Alexa\\test"; //待操作文件路径
	//char szDir[256] = "D:\\bmc\\test\\comp";
	char szDir[256] = "D:\\bmc\\Alexa.com\\compress";
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
			sprintf(szFile, "%s\\%s", szDir, ffd.cFileName);
			HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				continue;

			ReadFile(hFile, pBuf, ffd.nFileSizeLow, &dwRead, NULL);//读文件
			CloseHandle(hFile);

			Inflate decode;
			ByteInfoList infoList;
			int nSize = decode.DecodeBuffer(pBuf, dwRead, &infoList);
			ScanFirst(infoList, nSize, ffd.cFileName);
			delete pBuf;
		}
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	printf("Prepare Data Finished\n");
	return 0;
}

void ScanFirst(ByteInfoList &infoList, int desize, char *strFile)
{
	char fileSaved[256] = {0};
	//sprintf(fileSaved, "E:\\Document\\Alexa\\decomp-test/%s", strFile);
	sprintf(fileSaved, "D:\\bmc\\Alexa.com\\decompress2/%s", strFile);
	FILE *pfile = fopen(fileSaved, "wb");
	if(pfile == NULL)
	{
		printf("open saved file %s error\n", strFile);
		return;
	}

	int k = 0;
	int nSize = infoList.size();
	StoredInfo *info = new StoredInfo[desize];
	memset(info, 0, sizeof(StoredInfo)* desize);
	for (int i=0; i < nSize; i++)
	{
		if (infoList[i].d.length == 0)	// Scan Literal
		{
			info[k].token = infoList[i].t.token;
			k++;
		}
		else							// Scan DistCode
		{
			int offset = k - infoList[i].d.dist;
// 			info[k].length = infoList[i].d.length;
// 			info[k+1].dist = infoList[i].d.dist;

			int len = infoList[i].d.length;
			for(int j=0; j<len; j++, k++)
			{
				info[k].length = infoList[i].d.length-j;
				info[k].dist = infoList[i].d.dist;
				info[k].token = info[offset+j].token;
			}

		}
	}

	fwrite(info, sizeof(StoredInfo), k, pfile);
	fclose(pfile);
	delete info;
}
