#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
using namespace std;
extern unsigned long g_scanned, g_nMatch, g_Total, g_compressed, g_misscan;
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
int compressTestByNaive(char* szDir,char *patFile,int loop)
{
	vector<MemBuffer> m_vecBuf;//定义一个容器 存放MemBuffer类型
	vector<PatternInfo> patList;

	int count=readText(szDir, Compress, m_vecBuf);
	int* sizeArray = new int[count];
	TokenInfo** infoArray = new TokenInfo * [count];
	processText(m_vecBuf,infoArray, sizeArray, count);

	readPattern(patList,patFile);
	processPattern(patList);

	clock_t tmScan = GetTickCount64();
	BMCompressedMatching(patList, infoArray, sizeArray, count, loop, skipTable, shiftTable);
	clock_t tmProcess = GetTickCount64();
	clock_t nTime = (tmProcess - tmScan);

	printf("This is Compress: Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)g_compressed * 8 * loop / nTime / 1000);
	PrintResult(loop);

	clearArray(m_vecBuf, infoArray, sizeArray, count);
	return 0;
}

