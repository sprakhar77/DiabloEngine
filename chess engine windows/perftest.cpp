#include "def.h"


long leafNodes;
void Perft(int depth, BOARD *pos) {

    ASSERT(CheckBoard(pos));  

	if(depth == 0) {
        leafNodes++;
        return;
    }	

    MOVELIST list[1];
    GenMoves(pos,list);
      
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->Count; ++MoveNum) {	
       
        if ( !MakeMove(pos,list->MoveNum[MoveNum].Move))  {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }

    return;
}

void PerftTest(int depth, BOARD *pos) {

    ASSERT(CheckBoard(pos));

	PrintBoard(pos);
	printf("\nStarting Test To Depth:%d\n",depth);	
	leafNodes = 0;
	
    MOVELIST list[1];
    GenMoves(pos,list);	
    
    int move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->Count; ++MoveNum) {
        move = list->MoveNum[MoveNum].Move;
        if ( !MakeMove(pos,move))  {
            continue;
        }
        long cumnodes = leafNodes;
        Perft(depth - 1, pos);
        TakeMove(pos);        
        long oldnodes = leafNodes - cumnodes;
        printf("move %d :  ",MoveNum+1);
        PrintMove(move);
        printf(" : %ld\n",oldnodes);
    }
	
	printf("\nTest Complete : %ld nodes visited\n",leafNodes);

}
