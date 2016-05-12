#include "def.h"

const int KnDir[8] = {-8,-19,-21,-12,8,19,21,12};
const int RkDir[4] = {-1,-10,1,10};
const int BiDir[4] = {-9,-11,11,9 };
const int KiDir[8] = {-1,-10,1,10,-9,-11,11,9};

int IsSqAttack(int sq,int side,BOARD *board) {

	ASSERT(sq>=0 && sq<120);
	ASSERT(CheckBoard(board));
	int pce,index,t_sq,dir;
	if(board->Pieces[sq]==OFF)
		return FALSE;	
	//make sure that the pce u are looking forward to be attack by is actually present on that square
	//i.e the square is not OFF or EMPTY....
	// pawns	
	if(side == WHITE)
	{
		if(board->Pieces[sq-11]==wP||board->Pieces[sq-9]==wP)
			return TRUE;
	} 
	else 
	{
		if(board->Pieces[sq+11]==bP||board->Pieces[sq+9]==bP)
			return TRUE;	
	}
	// knights
	for(index = 0; index < 8; ++index)
	{		
		pce = board->Pieces[sq + KnDir[index]];
		if(pce>=wP && pce<=bK && IsKnight[pce] && PceCol[pce]==side)
			return TRUE;
	}
	// rooks, queens in rooks(horizontal,vertical) direction
	for(index = 0; index < 4; ++index) 
	{		
		dir = RkDir[index];
		t_sq = sq + dir;
		pce = board->Pieces[t_sq];
		while(pce != OFF) 
		{
			if(pce != EMPTY)
			{
				if(IsRQ[pce] && PceCol[pce] == side)
					return TRUE;
				break;
			}
			t_sq += dir;
			pce = board->Pieces[t_sq];
		}
	}
	// bishops, queens in bishop(diagnol) direction
	for(index = 0; index < 4; ++index) 
	{		
		dir = BiDir[index];
		t_sq = sq + dir;
		pce = board->Pieces[t_sq];
		while(pce != OFF) 
		{
			if(pce != EMPTY)
			{
				if(IsBQ[pce] && PceCol[pce] == side)
					return TRUE;
				break;
			}
			t_sq += dir;
			pce = board->Pieces[t_sq];
		}
	}
	// kings are the same as knights with 8 directions
	for(index = 0; index < 8; ++index) 
	{		
		pce = board->Pieces[sq + KiDir[index]];
		if(pce>=wP && pce<=bK && IsKing[pce] && PceCol[pce]==side)
			return TRUE;
	}
	return FALSE;
}
