#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
extern int skipTable[PAT_NUM][PAT_LEN];
extern int shiftTable[PAT_NUM][PAT_LEN];
//��ȡģʽ
void readPattern(std::vector<PatternInfo>& patList,char *strPatFile)
{
	//char strPatFile[] = "D:\\bmc\\pattern\\patten.txt";//��Ŀ¼��
	//char strPatFile[] = "D:\\bmc\\Alexa.com\\pat\\pat.txt";
	FILE* pFile = fopen(strPatFile, "r");
	if (pFile == NULL)
	{
		printf("could not open pattern file\n");
		exit(0);
	}
	while (true)
	{
		PatternInfo pi = { 0 };
		if (fscanf(pFile, "%s", pi.pat) == -1)
			break;
		pi.len = strlen(pi.pat);
		patList.push_back(pi);
	}
}
//����ģʽ
int processPattern(std::vector<PatternInfo>& patList) {

	for (int i = 0; i < PAT_NUM; i++)
	{
		char* pat = patList[i].pat;
		const int plen = patList[i].len;
		make_skip(pat, plen, skipTable[i]);
		make_shift(pat, plen, shiftTable[i]);
	}

	return 0;
}