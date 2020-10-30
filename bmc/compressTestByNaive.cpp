#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
using namespace std;
extern unsigned long  g_nMatch, g_misscan;
extern unsigned long long  g_scanned, g_Total, g_compressed;
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
extern std::vector<MemBuffer> m_vecBuf;
extern std::vector<PatternInfo>patList;
int compressTestByNaive(char* szDir,char *patFile,int loop)
{
	
	int count=readText(szDir, Compress, m_vecBuf);
	int* sizeArray = new int[count];
	TokenInfo** infoArray = new TokenInfo * [count];
	processText(m_vecBuf,infoArray, sizeArray, count);

	readPattern(patList,patFile);
	processPatternBm(patList);

	clock_t tmScan = GetTickCount64();
	BMCompressedMatching(patList, infoArray, sizeArray, count, loop, skipTable, shiftTable);
	clock_t tmProcess = GetTickCount64();
	clock_t nTime = (tmProcess - tmScan);

	printf("This is Compress: Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)g_compressed * 8 * loop / nTime / 1000);
	PrintResult(loop,100);

	clearArray(m_vecBuf, infoArray, sizeArray, count);
	return 0;
}

