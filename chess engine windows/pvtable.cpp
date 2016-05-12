#include <cstdio>
#include "def.h"

//this function is to get the best pv line which if possible for the current board position
//upto a certain depth
int GetPvLine(BOARD *board,int depth)
{
	ASSERT(depth<MAXDEPTH);
	int Move =ProbePvMove(board);
	int count=0;
	while(Move!=NOMOVE && count<depth)
	{
		//keep on making the moves until we find nomove or the depth is reached to MAX
		if(MoveExists(board,Move))
		{
			MakeMove(board,Move);
			board->PvArray[count]=Move;
			count++;
		}
		else
			break;
		//move to the best next position	
		Move=ProbePvMove(board);
	}
	//take back all the move that have been made
	while(board->Ply>0)
		TakeMove(board);
	return count;
}



//this function is used to reset the Pvtable by setting everything to 0
void ClearPvTable(PVTABLE *PvTable) 
{
	for (int i=0;i<PvTable->Count;i++) 
	{
		PvTable->PvEntries[i].PosKey = 0ULL;
		PvTable->PvEntries[i].Move = 0;
	}
}

//this  function is to initillaze the count and the clear the pvtable for use
void InitPvTable(PVTABLE *PvTable)
{  
	PvTable->Count=131070;
    ClearPvTable(PvTable);
	
}

//to store a move in the pvtable...usnig hashing function as poskey%count
void StorePvMove(BOARD *board,int Move)
{
	int index=board->PosKey%board->PvTable->Count;
	board->PvTable->PvEntries[index].PosKey=board->PosKey;
	board->PvTable->PvEntries[index].Move=Move;
}

//retrive the pos move if it is present for that given current poskey in the board
int ProbePvMove(BOARD *board)
{
	int index=board->PosKey%board->PvTable->Count;
	if(board->PvTable->PvEntries[index].PosKey==board->PosKey)
		return board->PvTable->PvEntries[index].Move;
	return 0;
}









