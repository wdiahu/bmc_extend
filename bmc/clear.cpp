#include "utils.h"

int clearArray(std::vector<MemBuffer>& m_vecBuf, char**infoArray, int* sizeArray, int count) {
	for (int i = 0; i < count; i++)
	{
		delete[] m_vecBuf[i].pBuffer;
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;
	return 0;
}


int clearArray(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count) {
	for (int i = 0; i < count; i++)
	{
// 		delete[] m_vecBuf[i].pBuffer;
		VirtualFree(m_vecBuf[i].pBuffer, m_vecBuf[i].nSize*sizeof(TokenInfo), MEM_RELEASE);
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;

	return 0;
}