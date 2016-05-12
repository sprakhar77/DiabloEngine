#include "def.h"
#include<cstdio>

//simple function to print any bitboard which is sent to it...here we revers the ranks as we in our previous calculations we 
//took the ranks in reverse order for simplicity in order for the conversions to be easy and now we revert them back again
//to take them to their orignal form where they start from the user side

void PrintBitBoard(ULL b)
{
	printf("\n");
	for(int rank=7;rank>=0;rank--)
	{
		for(int file=0;file<=7;file++)
		{	
			int sq120=fr2sq(file,rank);
			int sq64=sq120to64[sq120];
			if((1ULL<<sq64)&b)
				printf("X");
			else
				printf("-");			

		}
		printf("\n");
	}	
}

const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

//return the first index of the set bit int the bit boards and sets it to 0 code taken from chessprogramming.wkik
//and also the bitboard is used only for this function....we can also use the simple brute force method but it may be slower
int POP(ULL *bb) {
  ULL b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

//counts the nuber of set bits in a a bitboard
int COUNT(ULL b)
{
	int r;
	for(r=0;b;r++,b&=b-1);
	return r;
	
}
