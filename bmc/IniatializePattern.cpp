/****************************************************************
 *  Purpose: read text and process text
 *  
 ****************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
extern int nextTable[PAT_NUM][PAT_LEN];



void readPattern(std::vector<PatternInfo>& patList,char *strPatFile)
{
	FILE* pFile = fopen(strPatFile, "r");
	if (pFile == NULL)
	{
		printf("could not open pattern file\n");
		exit(0);
	}
	while (true)
	{
		PatternInfo pi = { 0 };
		if (fscanf(pFile, "%[^\n]\n", pi.pat) == -1)
			break;
		pi.len = strlen(pi.pat);
		patList.push_back(pi);
	}
}
int processPatternBm(std::vector<PatternInfo>& patList) {

	for (int i = 0; i < patList.size(); i++)
	{
		char* pat = patList[i].pat;
		const int plen = patList[i].len;
		make_skip(pat, plen, skipTable[i]);
		make_shift(pat, plen, shiftTable[i]);
	}

	return 0;
}
int processPatternKmp(std::vector<PatternInfo>& patList) {

	for (int i = 0; i < patList.size(); i++)
	{
		char* pat = patList[i].pat;   
		GetNext(pat,nextTable[i]);
	}

	return 0;


}