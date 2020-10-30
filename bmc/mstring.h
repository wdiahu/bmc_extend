#ifndef __MSTRING_H__
#define __MSTRING_H__

int *make_skip(char *, int, int*);
int *make_shift(char *, int, int*);
int mSearch(const char *, int, const char *, int, int *, int *);
int mContainsSubstr(const char*, int, const char*, int);
int mSearch_original(const char*, int, const char*, int, int*, int*);
//สตั้
int *make_skip(char* pat, int plen, int* skip, int **,int);
#endif  /* __MSTRING_H__ */
