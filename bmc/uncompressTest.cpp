#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
extern unsigned long g_nMatch, g_Total, g_misscan;
extern unsigned long long g_scanned, g_compressed;
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
using namespace std;
int uncompressTest(char* szDir, char* patFile, int loop) {
	vector<MemBuffer> m_vecBuf;
	vector<PatternInfo> patList;

	int count =readText(szDir, Uncompress, m_vecBuf);
	int* sizeArray = new int[count];
    char** infoArray = new char* [count];
	processText(m_vecBuf,infoArray,sizeArray,count);

	readPattern(patList,patFile);
	processPatternBm(patList);

	clock_t tmScan = GetTickCount64();
	UncompressedMatching(patList, infoArray, sizeArray, count, loop, skipTable, shiftTable);
	clock_t tmProcess = GetTickCount64();
	clock_t nTime = (tmProcess - tmScan);

	printf("This is UnCompress: Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)g_compressed * 8 * loop / nTime / 1000);
	PrintResult(loop,PAT_NUM);

	clearArray(m_vecBuf, infoArray, sizeArray, count);

	return 0;
}
