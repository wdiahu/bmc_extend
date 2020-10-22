/***************************************************************************
 * File: MSTRING.C
 * Purpose: Provide a variety of string functions not included in libc.  Makes
 *          up for the fact that the libstdc++ is hard to get reference
 *          material on and I don't want to write any more non-portable c++
 *          code until I have solid references and libraries to use.
 * History:
 * Date:      Author:  Notes:
 * ---------- ------- ----------------------------------------------
 *  08/19/98    MFR    Initial coding begun
 *  03/06/99    MFR    Added Boyer-Moore pattern match routine, don't use
 *                     mContainsSubstr() any more if you don't have to
 *  12/31/99	JGW    Added a full Boyer-Moore implementation to increase
 *                     performance. Added a case insensitive version of mSearch
 *  07/24/01    MFR    Fixed Regex pattern matcher introduced by Fyodor
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mstring.h"
extern unsigned long g_nMatch;

/****************************************************************
 *  Function: mSearch(char *, int, char *, int)
 *  Purpose: Determines if a string contains a (non-regex) substring.
 *  Parameters:
 *      buf => data buffer we want to find the data in
 *      blen => data buffer length
 *      ptrn => pattern to find
 *      plen => length of the data in the pattern buffer
 *      skip => the B-M skip array
 *      shift => the B-M shift array
 *  Returns: 1 on success (str constains substr), 0 on failure (substr not in str)
 ****************************************************************/
int mSearch(const char* buf, int blen, const char* pat, int plen, int* skip, int* shift)
{
	if (plen == 0)
		return 1;

	int matches = 0;
	int cmpcnt = 0;
	int b_idx = plen;
	while (b_idx <= blen)
	{
		int p_idx = plen;
		while (buf[--b_idx] == pat[--p_idx] && p_idx > 0)
		{
			cmpcnt++;
		}

		// 该条件分支放在外边更利于CPU加速内层循环处理
		// 这样的写法如果未优化，可能会多出内存比较次数，多出的次数等于匹配到的模式数
		// 而实际上编译器会优化为一次寄存器比较，那样就快很多
		// 在使用大规模数据时候可以进行比较
		if (buf[b_idx] == pat[p_idx])
		{
			cmpcnt++;
			matches++;
		}
		int offset = (shift[p_idx] > skip[(unsigned char)buf[b_idx]]) ? shift[p_idx] : skip[(unsigned char)buf[b_idx]];
		b_idx += offset;
	}

	printf("mSearch, matches:%d, compares:%d\n", matches, cmpcnt);
	return 0;
}

/****************************************************************
 *  Purpose: Create a Boyer-Moore skip table for a given pattern
 *  Parameters:
 *      ptrn => pattern
 *      plen => length of the data in the pattern buffer
 *  Returns: int* - the skip table, Bad Character
 ****************************************************************/
int* make_skip(char* pat, int plen, int *skip)
{
	
	for (int i = 0; i < 256; i++)
		skip[i] = plen + 1;

	while (plen != 0)
		skip[(unsigned char)*pat++] = plen--;

	return skip;
}
int* make_skip(char* pat, int plen, int* skip,int **table,int i)
{
	for (int i = 0; i < 256; i++)
		skip[i] = plen + 1;

	while (plen != 0)
		skip[(unsigned char)*pat++] = plen--;

	return skip;
}

/****************************************************************
 *  Purpose: Create a Boyer-Moore shift table for a given pattern
 *  Parameters:
 *      ptrn => pattern
 *      plen => length of the data in the pattern buffer
 *  Returns: int* - the shift table, Good Suffix
 ****************************************************************/
int* make_shift(char* pat, int plen, int *shift)
{
	int* sptr = shift + plen - 1;
	char* pptr = pat + plen - 1;
	char c = pat[plen - 1];

	*sptr = 1;
	while (sptr-- != shift)
	{
		char* p1 = pat + plen - 2, * p2, * p3;
		do
		{
			while (p1 >= pat && *p1-- != c);

			p2 = pat + plen - 2;
			p3 = p1;

			while (p3 >= pat && *p3-- == *p2-- && p2 >= pptr);
		} while (p3 >= pat && p2 >= pptr);

		*sptr = shift + plen - sptr + p2 - p3;
		pptr--;
	}

// 	for (int i = 0; i < plen; i++)
// 		printf("%d ", shift[i]);
	return shift;
}

/****************************************************************
 *  Function: mContainsSubstr(char *, int, char *, int)
 *  Purpose: Determines if a string contains a (non-regex) substring.
 *  Parameters:
 *      buf => data buffer we want to find the data in
 *      b_len => data buffer length
 *      pat => pattern to find
 *      p_len => length of the data in the pattern buffer
 *  Returns: 1 on success (str constains substr), 0 on failure (substr not in str)
 ****************************************************************/
int mContainsSubstr(const char* buf, int b_len, const char* pat, int p_len)
{
	const char* b_idx;  /* index ptr into the data buffer */
	const char* p_idx;  /* index ptr into the pattern buffer */
	const char* b_end;  /* ptr to the end of the data buffer */
	int m_cnt = 0;      /* number of pattern matches so far... */
	unsigned long loopcnt = 0;
	int matches = 0;

	/* mark the end of the strs */
	b_end = (char*)(buf + b_len);

	/* init the index ptrs */
	b_idx = buf;
	p_idx = pat;

	do
	{
		loopcnt++;
		if (*p_idx == *b_idx)
		{
			if (m_cnt == (p_len - 1))
			{
// 				printf("\n%ld compares for match, m_cnt:%d\n", loopcnt, m_cnt);
				matches++;
				b_idx = b_idx - (m_cnt - 1);
				m_cnt = 0;
				p_idx = pat;
				continue;
			//	return 1;
			}
			m_cnt++;
			b_idx++;
			p_idx++;
		}
		else
		{
			if (m_cnt == 0)
				b_idx++;
			else
				b_idx = b_idx - (m_cnt - 1);

			p_idx = pat;
			m_cnt = 0;
		}
	} while (b_idx < b_end);

	printf("mContainsSubstr, matches:%d, compares:%d", matches, loopcnt);
	/* if we make it here we didn't find what we were looking for */
	return 0;
}

/****************************************************************
 *  Function: mSearch(char *, int, char *, int)
 *  Purpose: Determines if a string contains a (non-regex) substring.
 *  Parameters:
 *      buf => data buffer we want to find the data in
 *      blen => data buffer length
 *      ptrn => pattern to find
 *      plen => length of the data in the pattern buffer
 *      skip => the B-M skip array
 *      shift => the B-M shift array
 *  Returns: 1 on success (str constains substr), 0 on failure (substr not in str)
 ****************************************************************/
int mSearch_original(const char* buf, int blen, const char* pat, int plen, int* skip, int* shift)
{
	printf("mSearch_original");
	if (plen == 0)
		return 1;

	int matches = 0;
	int cmpcnt = 0;
	int b_idx = plen;
	while (b_idx <= blen)
	{
		int p_idx = plen, skip_stride, shift_stride;
		while (buf[--b_idx] == pat[--p_idx])
		{
			cmpcnt++;
			// actually b_idx won't be less than 0
			if (b_idx < 0)
				return 0;

			if (p_idx == 0)
			{
				//matches++;
				g_nMatch++;
				printf("match,b_idx=%d\n", b_idx);
				break;
			//	return 1;
			}
		}
// 		if (buf[b_idx] != pat[p_idx])
// 			cmpcnt++;
		skip_stride = skip[(unsigned char)buf[b_idx]];
		shift_stride = shift[p_idx];
		b_idx += (skip_stride > shift_stride) ? skip_stride : shift_stride;
		printf("%d\n", b_idx);
	}

 	//printf("mSearch_original, matches:%d, compares:%d\n", matches, cmpcnt);
	return 0;
}