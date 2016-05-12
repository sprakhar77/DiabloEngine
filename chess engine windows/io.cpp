#include "def.h"

//a simple function to print the file and rank of a current square using the file and rank arrays
void PrintSq(int sq)
{
	ASSERT(sq>=0 && sq<120);
	printf("%c",FileChar[FileNo[sq]]);
	printf("%c\n",RankChar[RankNo[sq]]);
} 

//simple funtion to print the from,to and promoted bit values of the move using fille and ranks
char *PrintMove(int move) {

	static char MvStr[6];
	
	int ff = FileNo[FromSq(move)];
	int rf = RankNo[FromSq(move)];
	int ft = FileNo[ToSq(move)];
	int rt = RankNo[ToSq(move)];
	
	int promoted = Promoted(move);
	
	if(promoted) {
		char pchar = 'q';
		if(IsKnight[promoted]) {
			pchar = 'n';
		} else if(IsRQ[promoted] && !IsBQ[promoted])  {
			pchar = 'r';
		} else if(!IsRQ[promoted] && IsBQ[promoted])  {
			pchar = 'b';
		}
		sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
	} else {
		sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
	}
	
	return MvStr;
}

//this fuction takes in a move in form of files and rank and return the move in integer
int ParseMove(char *ptrChar, BOARD *pos)
{

	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

	int f1=(ptrChar[0] - 'a');
	int r1=(ptrChar[1] - '1');
	int f2=(ptrChar[2] - 'a');
	int r2=(ptrChar[3] - '1');
	
    int from = fr2sq(f1, r1);
    int to = fr2sq(f2, r2);	
    
	MOVELIST list[1];
    GenMoves(pos,list);      
    int MoveNum = 0;
	int Move = 0;
	int PromPce = EMPTY;
	
	for(MoveNum = 0; MoveNum < list->Count; ++MoveNum) {	
		Move = list->MoveNum[MoveNum].Move;
		if(FromSq(Move)==from && ToSq(Move)==to) {
			PromPce = Promoted(Move);
			if(PromPce!=EMPTY) {
				if(IsRQ[PromPce] && !IsBQ[PromPce] && ptrChar[4]=='r') {
					return Move;
				} else if(!IsRQ[PromPce] && IsBQ[PromPce] && ptrChar[4]=='b') {
					return Move;
				} else if(IsRQ[PromPce] && IsBQ[PromPce] && ptrChar[4]=='q') {
					return Move;
				} else if(IsKnight[PromPce]&& ptrChar[4]=='n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
    }
    return NOMOVE;	
}


//again a simple function to print a move list for a given piece
void PrintMoveList(MOVELIST *MoveList)
{
	printf("COUNT - %d\n",MoveList->Count);
	for(int i=0;i<MoveList->Count;i++)
	{
		int Move=MoveList->MoveNum[i].Move;
		int Score=MoveList->MoveNum[i].Score;
		printf("%d %s\n",Score,PrintMove(Move));
		
	}
}
