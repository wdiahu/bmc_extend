#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>

using namespace std;

extern unsigned long g_nMatch, g_misscan;
extern unsigned long long g_Total, g_compressed;
extern unsigned long long g_scanned;
extern int nextTable[PAT_NUM][PAT_LEN];
clock_t tmScan;
clock_t tmProcess;
clock_t nTime;
vector<MemBuffer> m_vecBuf;
vector<PatternInfo> patList;

int compressByKmp(char* szDir, char* patFile, int loop) {
	int count = readText(szDir, Compress, m_vecBuf);
	int* sizeArray = new int[count];

	TokenInfo** infoArray = new TokenInfo * [count];

	processText(m_vecBuf, infoArray, sizeArray, count);

	readPattern(patList, patFile);

	processPatternKmp(patList);
	
	KMPCompressedMatching(patList, infoArray,sizeArray, nextTable, count, loop);

	nTime = tmProcess - tmScan;

	printf("This is Compress(KMP): Process:%d ms, throughtput:%.2f Mbps\n", nTime, (double)(g_compressed * PAT_NUM) * 8 * loop / nTime / 1000);

	PrintResult(loop,PAT_NUM);

	clearArray(m_vecBuf, infoArray, sizeArray, count);
	return 0;

}