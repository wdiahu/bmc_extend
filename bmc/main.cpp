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
	char patFile[] = "D:\\bmc\\Alexa.com\\pat\\patten.txt";
	//char UncompressFileDir[] = "D:\\bmc\\Alexa.com\\test";
	//uncompressTest(UncompressFileDir,patFile,1);
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

	compressTestByNaive(szDir,patFile,1);
	return 0;
}

void InitPattern()
{

}

void PrintResult(int loop)
{
	printf("match:%d, scan:%d, total:%d, ratio:%.4f, mis-scan:%d, mis-ratio:%.4f\n", g_nMatch / loop, g_scanned / loop, g_Total, 100 * double(g_scanned) / g_Total / loop, g_misscan, 100 * double(g_misscan) / g_Total / loop);
}