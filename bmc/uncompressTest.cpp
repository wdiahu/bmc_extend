#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
extern unsigned long g_scanned, g_nMatch, g_Total, g_compressed, g_misscan;
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
using namespace std;
int uncompressTest(char* szDir, char* patFile, int loop) {
	//char szDir[256] = { 0 };
	//strcpy(szDir, "E:\\Document\\Alexa\\Alexa.com-20170509\\decompress");//D:\bmc\Alexa.com\decompress
// 	strcpy(szDir, "E:\\Document\\Alexa\\test");//D:\bmc\Alexa.com\decompress
 	//strcpy(szDir, "D:\\bmc\\test\\test");//D:\bmc\Alexa.com\decompress
	//strcpy(szDir, "D:\\bmc\\Alexa.com\\test");
	//strcpy(szDir, "D:\\bmc\\Alexa.com\\test");
// 	CHAR szFindDirStr[MAX_PATH];
// 	StringCchCopy(szFindDirStr, MAX_PATH, szDir);
// 	StringCchCat(szFindDirStr, MAX_PATH, TEXT("\\*"));
// 	WIN32_FIND_DATA ffd;
// 	HANDLE hFind = FindFirstFile(szFindDirStr, &ffd);
// 	if (INVALID_HANDLE_VALUE == hFind)
// 		return 0;
// 	char szFile[256];
// 	int file_index = 0;
// 	std::vector<MemBuffer> m_vecBuf;
// 	do
// 	{
// 		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 			continue;
// 		else
// 		{
// 			unsigned long dwRead = 0;
// 			char* pBuf = new char[ffd.nFileSizeLow];
// 			sprintf(szFile, "%s\\%s", szDir, ffd.cFileName);
// 			HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
// 			if (hFile == INVALID_HANDLE_VALUE)
// 				continue;
// 
// 			ReadFile(hFile, pBuf, ffd.nFileSizeLow, &dwRead, NULL);
// 			CloseHandle(hFile);
// 
// 			//  			printf("%d:%s\n", file_index++, ffd.cFileName);
// 						//MemBuffer mb = { pBuf, ffd.nFileSizeLow / sizeof(TokenInfo) };
// 			MemBuffer mb = { pBuf, ffd.nFileSizeLow };
// 			m_vecBuf.push_back(mb);
// 		}
// 	} while (FindNextFile(hFind, &ffd) != 0);
// 	FindClose(hFind);
// 	g_Total = 73657183;//4294967296
// 	int count = m_vecBuf.size();//count记录文件数量
// 	int* sizeArray = new int[count];//记录每个文件的大小
// 	char** infoArray = new char* [count];
// 	for (int i = 0; i < count; i++) {
// 		sizeArray[i] = m_vecBuf[i].nSize;
// 		infoArray[i] = (char*)m_vecBuf[i].pBuffer;
// 	}
// 	vector<PatternInfo> patList;//定义一个存放PatternInfo的容器
// 	readPattern(patList);//读取模式串
// 	int loop = 1;
// 	for (int i = 0; i < PAT_NUM; i++)//对传入的所有模式进行操作 预操作
// 	{
// 		char* pat = patList[i].pat;
// 		const int plen = patList[i].len;
// 		make_skip(pat, plen, skipTable[i]);
// 		make_shift(pat, plen, shiftTable[i]);
// 
// 	}
	vector<MemBuffer> m_vecBuf;//定义一个容器 存放MemBuffer类型
	vector<PatternInfo> patList;
	int count =readText(szDir, false, m_vecBuf);
	int* sizeArray = new int[count];
    char** infoArray = new char* [count];
	processText(m_vecBuf,infoArray,sizeArray,count);
	readPattern(patList,patFile);
	processPattern(patList);
	clock_t tmScan = GetTickCount64();
	UncompressedMatching(patList, infoArray, sizeArray, count, loop, skipTable, shiftTable);
	clock_t tmProcess = GetTickCount64();
	clock_t nTime = (tmProcess - tmScan);
	printf("This is UnCompress: Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)g_compressed * 8 * loop / nTime / 1000);
	PrintResult(loop);
	clearArray(m_vecBuf, infoArray, sizeArray, count);

	// 清理工作
	/*for (int i = 0; i < count; i++)
	{
		delete[] m_vecBuf[i].pBuffer;
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;*/

	return 0;
}
