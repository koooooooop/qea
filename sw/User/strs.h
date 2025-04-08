#ifndef _H_STRS_
#define _H_STRS_
#include "define.h"
extern U16 fnSTR_CompareCon(char *src, const char *dest);
extern U16 fnSTR_Compare(char *src, const char * dest);
extern U32 fnSTR_STRToDec(char *str);
extern U16 fnSTR_DecToStr(char *str,U16 offset,U32 value, U8 fixed);
extern U8 fnSTR_Match(char *src, const char *dest);
extern U8 fnSTR_MatchNonCon(char *src, char *dest);
extern void fnSTR_HexToAscii(U8 *src, char *dest, U16 len);
extern void fnSTR_HexToStr(char *dest, U32 value);
extern U16 fnSTR_STRToHex(char *src, U16 maxlen);
extern void fnSTR_ConStrCpy(char *pDst, U16 *pLen, const char *pSrc, U16 maxlen);
extern void fnSTR_Replace(char *buff, char src, char dest);
extern void fnSTR_Trim(char *msg);
extern U8 fnSTR_DeliterText(char *str, char *deliter, U8 offset, char *substr, U8 maxlen);
extern U16 fnSTR_HexToStrZero(char *dest, U32 value);
#endif
