#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>

#pragma warning (disable: 4996)
//extern int g_next[];
//extern int g_skip[];
//extern int g_shift[];
int skipTable[PAT_NUM][PAT_LEN] = { 0 };
int shiftTable[PAT_NUM][PAT_LEN] = { 0 };
unsigned long g_misscan = 0;
unsigned long g_nMatch = 0;
unsigned long g_scanned = 0;
unsigned long g_Total = 73657183;	// 1248850022   73657183
unsigned long g_compressed = 16290012;	// 237977515 16290012
using namespace std;

int main(int argc, char** argv)
{
	//uncompressTest();
	//return 0;
	if (argc < 3)
		return 0;
	int sets = atoi(argv[2]);		// 使用哪个数据集
	int loop = atoi(argv[1]);		// 循环多少次

	char szDir[256] = { 0 };
	if (sets == 1)
	{
		g_Total = 73657183;
		g_compressed = 16290012;
		strcpy(szDir, "E:\\Document\\Alexa\\decomp-com");
		//strcpy(szDir, "D:\\bmc\\Alexa.com\\decompress1");
	}
	else if (sets == 2)
	{
		g_Total = 1248850022;
		g_compressed = 237977515;
		strcpy(szDir, "E:\\Document\\Alexa\\decomp-cn");
	}
	else
	{
		g_Total = 1289106;//4294967296
		g_compressed = 80;
		strcpy(szDir, "D:\\bmc\\Alexa.com\\testDec");

		
	}

	CHAR szFindDirStr[MAX_PATH];
	StringCchCopy(szFindDirStr, MAX_PATH, szDir);
	StringCchCat(szFindDirStr, MAX_PATH, TEXT("\\*"));

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(szFindDirStr, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		return 0;

	char szFile[256];
	int file_index = 0;
	std::vector<MemBuffer> m_vecBuf;
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

			ReadFile(hFile, pBuf, ffd.nFileSizeLow, &dwRead, NULL);
			CloseHandle(hFile);

// 			printf("%d:%s\n", file_index++, ffd.cFileName);
			MemBuffer mb = { pBuf, ffd.nFileSizeLow / sizeof(TokenInfo) };
			m_vecBuf.push_back(mb);
		}
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	int count = m_vecBuf.size();//count记录文件数量
	int* sizeArray = new int[count];//记录每个文件的大小
	TokenInfo** infoArray = new TokenInfo * [count];//记录每个文件里的内容
	for (int i = 0; i < count; i++)
	{
		sizeArray[i] = m_vecBuf[i].nSize;
		infoArray[i] = (TokenInfo*)m_vecBuf[i].pBuffer;
	}

	vector<PatternInfo> patList;
 	readPatten(patList);//读取模式串
	for (int i=0;i<PAT_NUM;i++)
	{
		char *pat=patList[i].pat;
		const int plen = patList[i].len;
		make_skip(pat, plen, skipTable[i]);
		make_shift(pat, plen, shiftTable[i]);
		
	}
	clock_t tmScan = GetTickCount();
	//模式集  模式长度集 文本信息 文本大小 待匹配文本个数  储存两个表的二维数组
 	BMCompressedMatching(patList,infoArray, sizeArray, count, loop, skipTable, shiftTable);
	//BMUnCompressMatching(pat, plen, infoArray, sizeArray, count, loop);
	clock_t tmProcess = GetTickCount();

	clock_t nTime = (tmProcess - tmScan);
	printf("This is Compress: Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)g_compressed * 8 * loop / nTime / 1000);

	PrintResult(loop);

	// 清理工作
	for (int i = 0; i < count; i++)
	{
		delete[] m_vecBuf[i].pBuffer;
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;

	return 0;
}

void InitPattern()
{

}

void PrintResult(int loop)
{
	printf("match:%d, scan:%d, total:%d, ratio:%.4f, mis-scan:%d, mis-ratio:%.4f\n", g_nMatch / loop, g_scanned / loop, g_Total, 100 * double(g_scanned) / g_Total / loop, g_misscan, 100 * double(g_misscan) / g_Total / loop);
}