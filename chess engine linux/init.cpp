#include "def.h"
#include<cstdlib>

//since our general hash function only generate random numbers upto 0 to 15 bits so to get 64 bit random numbers
//int the second time we generatete first random 15 bits and shift them 15 to the left so the next 15 bits are filled
//and similarly for the third set of fifteen bits and then for the remaning last four bits...fianlly we add them all to get a 64 bit no 
#define rand_64 ((ULL)rand() | \
		(ULL)rand()<<15 | \
		(ULL)rand()<<30 | \
		((ULL)rand()<<45 | \
		((ULL)rand() & 0xf)<<60))

int sq120to64[SQNO];
int sq64to120[64];
ULL HashKey[13][120];  //to store a random 64bit no which states that that type of element is present on that sq no
ULL SideKey;           //to store a random numbeer if it is the turn of white in the game
ULL CastleKey[16];     //to store 64  bits random numbers which represent each possible type of castlepositions which are 0 to 15
int FileNo[SQNO];
int RankNo[SQNO];



//converts the 120 size board squares to respective 0 to 63 suqares for the 64 size board and vice versa
void Convert()
{
	int sq64=0;
	int sq120=21;
	for(int i=0;i<120;i++)
		sq120to64[i]=65; //give some invalid index for outof bound indexes
	for(int i=0;i<64;i++)
		sq64to120[i]=120;  //same as above
	for(int rank=0;rank<8;rank++)
		for(int file=0;file<8;file++)
		{
			sq120=fr2sq(file,rank);  //move across ranks and files and convert the respective indexes to 120 based squares
			sq120to64[sq120]=sq64;     //then at that index give the value of square value at 64 size board and vice versa
			sq64to120[sq64]=sq120;
			sq64++;
		}
}

void InitHashKey()
{
	for(int i=0;i<13;i++)
		for(int j=0;j<120;j++)
			HashKey[i][j]=rand_64;
	SideKey=rand_64;
	for(int i=0;i<16;i++)
		CastleKey[i]=rand_64;
}

//this function initializes our RankNo and FileNo array wihch for any given 12 based square as an index
//will give its rank and file
void InitializeFileAndRanks()
{	
	//first initializes everything to Offboard
	for(int i=0;i<120;i++)
		FileNo[i]=OFF,RankNo[i]=OFF;
	
	//assingn the respective values to the inner board
	for(int rank=RANK_1;rank<=RANK_8;rank++)
		for(int file=FILE_A;file<=FILE_H;file++)
		{
			int sq=fr2sq(file,rank);
			FileNo[sq]=file;
			RankNo[sq]=rank;
		}
}

//initializies everythig
void AllInit()
{
	Convert();
	InitHashKey();
	InitializeFileAndRanks();
	InitMvvLva();
	
}
