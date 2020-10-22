#pragma once

#include <vector>
// #define  COUNT_MATCH

#pragma pack(2)

enum _CheckStatus
{
	STATUS_INIT = 0,
	STATUS_CHECK,
	STATUS_UNCHECK,
	STATUS_MATCH,
};

typedef struct _TokenInfo
{
	unsigned char token;
	unsigned char status;	// for arch
	unsigned short depth;	// for arch
	unsigned int state;		// for depict
}TokenInfo;

typedef struct _DistInfo
{
	unsigned short dist;
	unsigned short length;
}DistInfo;

typedef union _ByteInfo
{
	TokenInfo t;
	DistInfo d;
}ByteInfo;

typedef std::vector<ByteInfo> ByteInfoList;
typedef std::vector<TokenInfo> SILDE_WIN;

#pragma pack()