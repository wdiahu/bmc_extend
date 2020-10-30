#include "utils.h"
#include "mstring.h"
#include <windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
using namespace std;
int readText(char* szDir,bool option, vector<MemBuffer> &m_vecBuf) {
	CHAR szFindDirStr[MAX_PATH];
	StringCchCopy(szFindDirStr, MAX_PATH, szDir);
	StringCchCat(szFindDirStr, MAX_PATH, TEXT("\\*"));
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(szFindDirStr, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		return 0;
	char szFile[256];
	int file_index = 0;
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		else
		{
			unsigned long dwRead = 0;
 			char* pBuf = new char[ffd.nFileSizeLow];
			//char* pBuf = (char*)VirtualAlloc(NULL, ffd.nFileSizeLow, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
			//printf("%s\n", ffd.cFileName);
			wsprintf(szFile, "%s\\%s", szDir, ffd.cFileName);
			HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				continue;

			ReadFile(hFile, pBuf, ffd.nFileSizeLow, &dwRead, NULL);
			CloseHandle(hFile);

			if (option) {
				MemBuffer mb = { pBuf, ffd.nFileSizeLow / sizeof(TokenInfo) };
				m_vecBuf.push_back(mb);
			}
			else
			{
				MemBuffer mb = { pBuf, ffd.nFileSizeLow };
				m_vecBuf.push_back(mb);
			}
	
			
		}
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	return m_vecBuf.size();
}

int processText(std::vector<MemBuffer>& m_vecBuf, char** infoArray, int* sizeArray,int count) {

	for (int i = 0; i < count; i++) {
		sizeArray[i] = m_vecBuf[i].nSize;
		infoArray[i] = m_vecBuf[i].pBuffer;
	}
	return 0;
}
int processText(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count) {

	for (int i = 0; i < count; i++) {
		sizeArray[i] = m_vecBuf[i].nSize;
		infoArray[i] = (TokenInfo*)m_vecBuf[i].pBuffer;
	}
	return 0;
}


	


