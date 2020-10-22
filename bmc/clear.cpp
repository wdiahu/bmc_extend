#include "utils.h"

int clearArray(std::vector<MemBuffer>& m_vecBuf, char**infoArray, int* sizeArray, int count) {
	for (int i = 0; i < count; i++)
	{
		delete[] m_vecBuf[i].pBuffer;
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;

	//printf("全部清空完毕");
	return 0;
}


int clearArray(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count) {
	for (int i = 0; i < count; i++)
	{
		delete[] m_vecBuf[i].pBuffer;
	}
	m_vecBuf.clear();
	delete[] sizeArray;
	delete[] infoArray;

	//printf("全部清空完毕");
	return 0;
}