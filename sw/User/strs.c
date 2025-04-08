#include "strs.h"
U16 fnSTR_CompareCon(char *src, const char *dest);
U16 fnSTR_Compare(char *src, const char * dest);
U16 fnSTR_CompareCon(char *src, const char *dest)
{
	U16 i = 0;
	U16 j = 0;
	U16 res = 0;
	U8 ret;
	while(src[i] != '\0')
	{
		j = 0;
		ret = TRUE;
		while(dest[j] != '\0')
		{
			if(src[i +j] == '\0')
			{
				ret = FALSE;
				break;
			}
			if(src[i + j] != dest[j])
			{
				ret = FALSE;
				break;
			}
			j++;
		}
		if(dest[j] != '\0')
		{
			ret = FALSE;
		}
		if(ret == TRUE)
		{
			res = i + j + 1;
			break;
		}
		i++;
	}
	return res;
}
U16 fnSTR_Compare(char *src, const char * dest)
{
	U16 i = 0;
	U16 j = 0;
	U16 res = 0;
	U8 ret;
	while(src[i] != '\0')
	{
		j = 0;
		ret = TRUE;
		while(dest[j] != '\0')
		{
			if(src[i +j] == '\0')
			{
				ret = FALSE;
				break;
			}
			if(src[i + j] != dest[j])
			{
				ret = FALSE;
				break;
			}
			j++;
		}
		if(dest[j] != '\0')
		{
			ret = FALSE;
		}
		if(ret == TRUE)
		{
			res = i + j + 1;
		}
		i++;
	}
	return res;
}
U32 fnSTR_STRToDec(char *str)
{
	U8 i = 0;
	U32 value = 0;
	while(str[i] != '\0')
	{
		value = value * 10;
		value += str[i] - '0';
		i++;
	}
	return(value);
} 
U16 fnSTR_DecToStr(char *str,U16 offset,U32 value, U8 fixed)
{
	U32 mul = 10;
	U32 ret, remain, bmul;
	U8 i, digits, nums;
	char temp;
	ret = value;
	digits = 10;
	if(fixed > 0)
	{
		digits = fixed;
	}
	nums = 0;
	for(i = 0; i < digits; i++)
	{
		remain = ret / mul;
		bmul = remain * mul;
		str[offset + i] = ret - bmul + '0';
		nums = i;
		ret = remain;
		if((ret == 0)
			&& (fixed == 0))
			
		{
			break;
		}
	}
	str[offset + i + 1] = '\0';
	digits = (nums + 1) >> 1;
	for(i = 0; i< digits; i++)
	{
		temp = str[i + offset];
		str[i + offset] = str[nums - i + offset];
		str[nums - i + offset] = temp;
		
	}
	return(offset + nums + 1);
	
}

U8 fnSTR_Match(char *src, const char *dest)
{
	U16 i = 0;
	U8 res = TRUE;
	while((src[i] != '\0')
		&& (dest[i] != '\0'))
	{
		if(src[i] != dest[i])
		{
			res = FALSE;
			break;
		}
		i++;
	}
	if(res){
		if(src[i] != dest[i]){
			res = FALSE;
		}
	}
	return res;
}

U8 fnSTR_MatchNonCon(char *src, char *dest)
{
	U16 i = 0;
	U8 res = TRUE;
	while((src[i] != '\0')
		&& (dest[i] != '\0'))
	{
		if(src[i] != dest[i])
		{
			res = FALSE;
			break;
		}
		i++;
	}
	if(res){
		if(src[i] != dest[i]){
			res = FALSE;
		}
	}
	return res;
}

void fnSTR_HexToAscii(U8 *src, char *dest, U16 len)
{
	U16 i;
	U8 val;
	for(i = 0; i< len; i++){
		val = (src[i] >> 4) & 0x0F;
		if(val <= 9)
			dest[2 * i] = '0'+ val;
		else
			dest[2 * i] = 'A'+ val - 0x0A;
		val = src[i] & 0x0F;
		if(val <= 9)
			dest[2 * i + 1] = '0'+ val;
		else
			dest[2 * i + 1] = 'A'+ val - 0x0A;
	}
}
void fnSTR_HexToStr(char *dest, U32 value)
{
	U8 i;
	U8 val;
	for(i = 0; i< 8; i++){
		val = (value >> (28 - i * 4)) & 0x0F;
		if(val <= 9)
			dest[i] = '0'+ val;
		else
			dest[i] = 'A'+ val - 0x0A;
	}
}
U8 fnSTR_NibbleToHex(char val){
	U8 v;
	v = 0;
	if((val >= '0')
			&& (val <= '9'))
			v = ((val - '0')); 
	if((val >= 'a')
		&& (val <= 'f'))
		v = ((val - 'a' + 10));
	if((val >= 'A')
		&& (val <= 'F'))
		v = ((val - 'A' + 10));
	return v;
}
U16 fnSTR_STRToHex(char *src, U16 maxlen){
	U16 i;
	U8 v;
	i = 0;
	while(i < (maxlen - 1)){
		if((src[i] == 0)
			||(src[i + 1]  == 0))
		{
			break;
		}
		v = fnSTR_NibbleToHex(src[i]);
		v = v << 4;
		v |= fnSTR_NibbleToHex(src[i + 1]);
		src[i / 2] = v;
		i += 2;
		
	}
	return(i >> 1);
}
void fnSTR_ConStrCpy(char *pDst, U16 *pLen, const char *pSrc, U16 maxlen)
{
	U8 i;
	i = 0;
	while((pSrc[i] != '\0')
		&& (i< maxlen))
	{
		pDst[i] = pSrc[i];
		i ++;
	}
	*pLen = i;
	pDst[i] = '\0';
}
void fnSTR_Replace(char *buff, char src, char dest){
	U16 i;
	i = 0;
	while((i < 1000)
		&& (buff[i] != 0)){
		if(buff[i] == src){
			buff[i] = dest;
		}
		i++;
	}
}
#define STR_MAXLEN  1024
void fnSTR_Trim(char *msg)
{
	unsigned int i,j;
	i = 0;
	while((i < (STR_MAXLEN - 1))
			&& (msg[i] != '\0'))
	{
		if((msg[i] == 0x0D)
			|| (msg[i] == 0x0A))
		{
			j = i;
			while(j < (STR_MAXLEN - 1))
			{
				msg[j] = msg[j + 1];
				if(msg[j + 1] == '\0')
				{
					break;
				}
				j++;
			}

		}
		else
		{
		    i++;
		}
	}
}
U8 fnSTR_DeliterText(char *str, char *deliter, U8 offset, char *substr, U8 maxlen){
	U16 i;
	U16	infocount = 0;
	U16 deliterind = 0;
	U16 infonum = 0;
	U8 res = FALSE;
	i = 0;
	while(str[i] !='\0')
	{
		
		
		if(str[i] == deliter[deliterind])
		{
			infocount ++;
			infonum = 0;
			deliterind ++;
		}
		else if((str[i] == 0x0D)
			|| (str[i] == 0x0A))
		{
			break;
		}
		else 
		{
			if(infocount == offset){
				if(i <(maxlen - 1)){
					substr[infonum + 1] = 0x00;
					substr[infonum] = str[i];
				}
				res = TRUE;
			}
			infonum++;
		}
		i++;
	}
	return(res);
}

U16 fnSTR_HexToStrZero(char *dest, U32 value)
{
	U8 i;
	U8 val;
	U8 zero = FALSE;
	U8 k = 0;
	for(i = 0; i< 8; i++){
		val = (value >> (28 - i * 4)) & 0x0F;
		if(FALSE == zero)
		{
			if(0 != val)
			{
				k = 0;
				zero = TRUE;
			}
		}
		if(TRUE == zero)
		{
			if(val <= 9)
				dest[k] = '0'+ val;
			else
				dest[k] = 'A'+ val - 0x0A;
			k++;
		}
		
	}
	if(0 == k)
	{
		dest[k] = '0';
		k = 1;
	}
	return((U16)k);
}

