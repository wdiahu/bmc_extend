#ifndef _UTILS_H_
#define _UTILS_H_
#include "Common.h"

enum StateCate
{
	CATE_NORMAL = 0,
	CATE_INIT = 1,
	CATE_NON_BEGIN = 2,
	CATE_ANC_BEGIN = 4,
	CATE_END = 8,
};

#define MAX_LEN 3000000
typedef struct _MemBuffer
{
	char *pBuffer;
	size_t nSize;
}MemBuffer;

typedef struct _Pointer
{
	unsigned short dist;
	unsigned short length;
}Pointer;


typedef struct _StoredInfo
{
	unsigned short token;
// 	union
// 	{
		unsigned short dist;
		unsigned short length;
// 	};
}StoredInfo;

void ScanFirst(ByteInfoList &infoList, int size, char *file);

int gettimeofday(struct timeval* tp, void* tzp);

#endif	// _UTILS_H_
