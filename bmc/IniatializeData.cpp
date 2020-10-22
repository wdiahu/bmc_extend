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
int InitializeData(char* szDir, bool option) {

	/*vector<MemBuffer> m_vecBuf;//定义一个容器 存放MemBuffer类型
	vector<PatternInfo> patList;
	int count = readText(szDir, option, m_vecBuf);
	int* sizeArray = new int[count];
	TokenInfo** infoArray = new TokenInfo * [count];
	processText(m_vecBuf, infoArray, sizeArray, count);
	//readPattern(patList);
	processPattern(patList);*/
	return 0;
}