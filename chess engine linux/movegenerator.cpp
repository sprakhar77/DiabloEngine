#include "def.h"
#define MkMove(f,t,cap,prom,flag) (f|(t<<7)|(cap<<14)|(prom<<20)|flag)


//these arrays are for the move ordering method of mostvaluable victim least valuabe attacker
//like a pawn captures queen...we initially assign everthing to zero...so that the score
//of non capture moves remain zero
int VictimScore[13]={0,100,200,300,400,500,600,100,200,300,400,500,600};
static int MvvLvaScores[13][13]={0};

void InitMvvLva()
{
	//for all the captue possibilites we find the value of all combinations
	for(int i=wP;i<=bK;i++)
		for(int j=wP;j<=bK;j++)
			MvvLvaScores[j][i]=VictimScore[j] + 6 -(VictimScore[i]/100);
}

//these arrays easly help us to generate moves for sliding pieces of both sides as they define the starting
//index of white and black sliding pieces and when we reach a 0 we have completed one sides sliding pieces
int SlidePce[]={ wB,wR,wQ ,0,bB,bR,bQ,0};
int SlideIndex[]={0,4};


//similar arrays for non sliding pieces...
int NonSlidePce[]={wN,wK,  0 , bN,bK,0};
int NonSlideIndex[]={0,3};

//these store the direction for each piece which need to be added to the current square...to get all the squares
//where a type of piecce indexed by rows can move 
int PceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

//these are the number of directions for each piece type for queen=8,bishp and rook=4
int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};





//function that takes in a movelist say for WHITE and a possible move and add it to the movelist of WHITE
//it takes in all the three types of moves CaptureMove,SimpleMove...and special EnPas move for pawns
static void AddMove(BOARD *board,int Move,MOVELIST *MoveList)
{
	int Score=0;
	if(Move & FlagEnPas)
		Score=105 + 1000000;
	else if(Captured(Move))
		Score=1000000 + MvvLvaScores[Captured(Move)][board->Pieces[FromSq(Move)]];
	else if(Captured(Move)==0)
	{
		if(board->SearchKillers[0][board->Ply]==Move)
			Score=900000;
		else if(board->SearchKillers[1][board->Ply]==Move)
			Score=800000;
		else
			Score=board->SearchHistory[board->Pieces[FromSq(Move)]][ToSq(Move)];
	}
	MoveList->MoveNum[MoveList->Count].Move=Move;
	MoveList->MoveNum[MoveList->Count].Score=Score;
	MoveList->Count++;
}

static void AddwPMove(BOARD *board ,int from,int to,int cap,int flag,MOVELIST *MoveList)
{
	if(RankNo[from]==RANK_7)
	{
		AddMove(board,MkMove(from,to,cap,wR,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,wQ,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,wB,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,wN,flag),MoveList);
	}
	else
		AddMove(board,MkMove(from,to,cap,EMPTY,flag),MoveList);
}

static void AddbPMove(BOARD *board ,int from,int to,int cap,int flag,MOVELIST *MoveList)
{
	if(RankNo[from]==RANK_2)
	{
		AddMove(board,MkMove(from,to,cap,bR,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,bQ,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,bB,flag),MoveList);
		AddMove(board,MkMove(from,to,cap,bN,flag),MoveList);
	}
	else
		AddMove(board,MkMove(from,to,cap,EMPTY,flag),MoveList);
}

//generates all the the possible moves to be added to a particular sides movelist
void GenMoves(BOARD *board,MOVELIST *MoveList)
{
	ASSERT(CheckBoard(board));
	MoveList->Count=0;
	int Side=board->Side;
	
	if(Side==WHITE)
	{
		//add moves for WHITE pawns
		for(int i=0;i<board->PceNum[wP];i++)
		{
			int sq=board->PceList[wP][i];
			ASSERT(sq!=OFF);
			
			//this are no capture straigh moves for pawns
			if(board->Pieces[sq+10]==EMPTY)
				AddwPMove(board,sq,sq+10,0,0,MoveList); //this move is a simple move so no cap,prom,flag value
			if(RankNo[sq]==RANK_2 && board->Pieces[sq+10]==EMPTY && board->Pieces[sq+20]==EMPTY)
				AddMove(board,MkMove(sq,sq+20,0,0,FlagPawnSt),MoveList);//this move is again a simple but starting
				//move for a pawns so we need to set the flag for PawnSt and so we use a normal move addition
			
			//now we add diagnol move or capture move for the pawns..and as we know wP can move +9 and +11 for black capture
			if(board->Pieces[sq+9]!=OFF && PceCol[board->Pieces[sq+9]]==BLACK)
				AddwPMove(board,sq,sq+9,board->Pieces[sq+9],0,MoveList);
			if(board->Pieces[sq+11]!=OFF && PceCol[board->Pieces[sq+11]]==BLACK)
				AddwPMove(board,sq,sq+11,board->Pieces[sq+11],0,MoveList);
				
				
			//now we add if any EnPas moves for Pawns are present
			if(board->EnPas==sq+9 && board->EnPas!=NONE)
				AddwPMove(board,sq,sq+9,0,FlagEnPas,MoveList);
			if(board->EnPas==sq+11 && board->EnPas!=NONE)
				AddwPMove(board,sq,sq+11,0,FlagEnPas,MoveList);
		}
		
		//if the permission for castling is there for this side then add the castling move
		if(board->CastlePerm & WKCA)
			if(board->Pieces[F1]==EMPTY && board->Pieces[G1]==EMPTY)
				if(!IsSqAttack(E1,BLACK,board) && !IsSqAttack(F1,BLACK,board))
					AddMove(board,MkMove(E1,G1,0,0,FlagCastle),MoveList);
	

		if(board->CastlePerm & WQCA)
			if(board->Pieces[D1]==EMPTY && board->Pieces[C1]==EMPTY && board->Pieces[B1]==EMPTY)
				if(!IsSqAttack(E1,BLACK,board) && !IsSqAttack(D1,BLACK,board))
					AddMove(board,MkMove(E1,C1,0,0,FlagCastle),MoveList);
	}
	//and similarly copy the above code for black...which will be the exact same with AddbPMove function
	else
	{
		for(int i=0;i<board->PceNum[bP];i++)
		{
			int sq=board->PceList[bP][i];
			ASSERT(sq!=OFF);

			if(board->Pieces[sq-10]==EMPTY)
				AddbPMove(board,sq,sq-10,0,0,MoveList); 
			if(RankNo[sq]==RANK_7 && board->Pieces[sq-10]==EMPTY && board->Pieces[sq-20]==EMPTY)
				AddMove(board,MkMove(sq,sq-20,0,0,FlagPawnSt),MoveList);
				
			if(board->Pieces[sq-9]!=OFF && PceCol[board->Pieces[sq-9]]==WHITE)
				AddbPMove(board,sq,sq-9,board->Pieces[sq-9],0,MoveList);
			if(board->Pieces[sq-11]!=OFF && PceCol[board->Pieces[sq-11]]==WHITE)
				AddbPMove(board,sq,sq-11,board->Pieces[sq-11],0,MoveList);

			if(board->EnPas==sq-9 && board->EnPas!=NONE)
				AddbPMove(board,sq,sq-9,0,FlagEnPas,MoveList);
			if(board->EnPas==sq-11 && board->EnPas!=NONE)
				AddbPMove(board,sq,sq-11,0,FlagEnPas,MoveList);
		}
		//if the permission for castling is there for this side then add the castling move
		if(board->CastlePerm & BKCA)
			if(board->Pieces[F8]==EMPTY && board->Pieces[G8]==EMPTY)
				if(!IsSqAttack(E8,WHITE,board) && !IsSqAttack(F8,WHITE,board))
					AddMove(board,MkMove(E8,G8,0,0,FlagCastle),MoveList);
	

		if(board->CastlePerm & BQCA)
			if(board->Pieces[D8]==EMPTY && board->Pieces[C8]==EMPTY && board->Pieces[B8]==EMPTY)
				if(!IsSqAttack(E8,WHITE,board) && !IsSqAttack(D8,WHITE,board))
					AddMove(board,MkMove(E8,C8,0,0,FlagCastle),MoveList);
	}
	//now we generate moves for sliding type pieces and the same code for both white and black....
	int PceIndex=SlideIndex[Side];
	int Pce=SlidePce[PceIndex++];
	while(Pce!=0)
	{
		for(int i=0;i<board->PceNum[Pce];i++)
		{
			int sq=board->PceList[Pce][i];
			ASSERT(sq>=0 && sq<120);
			for(int j=0;j<NumDir[Pce];j++)
			{
				int Dir=PceDir[Pce][j];
				int NewSq=sq+Dir;
				while(board->Pieces[NewSq]!=OFF)
				{
					if(board->Pieces[NewSq]!=EMPTY)
					{
						if(PceCol[board->Pieces[NewSq]]==Side^1)
							AddMove(board,MkMove(sq,NewSq,board->Pieces[NewSq],0,0),MoveList);
						break;
					}
					else if(board->Pieces[NewSq]==EMPTY)
						AddMove(board,MkMove(sq,NewSq,0,0,0),MoveList);
					NewSq+=Dir;
				}
			}
		}
		Pce=SlidePce[PceIndex++];
	}
	
	
	//now similar way to generate moves for non slide pieces
	PceIndex=NonSlideIndex[Side];
	Pce=NonSlidePce[PceIndex++];
	while(Pce!=0)
	{
		for(int i=0;i<board->PceNum[Pce];i++)
		{
			int sq=board->PceList[Pce][i];
			ASSERT(sq>=0 && sq<120);
			for(int j=0;j<NumDir[Pce];j++)
			{
				int Dir=PceDir[Pce][j];
				int NewSq=sq+Dir;
				if(board->Pieces[NewSq]!=OFF && board->Pieces[NewSq]!=EMPTY)
				{
					if(PceCol[board->Pieces[NewSq]]==Side^1)
						AddMove(board,MkMove(sq,NewSq,board->Pieces[NewSq],0,0),MoveList);
				}
				else if(board->Pieces[NewSq]!=OFF && board->Pieces[NewSq]==EMPTY)
					AddMove(board,MkMove(sq,NewSq,0,0,0),MoveList);
			}
		}
		Pce=NonSlidePce[PceIndex++];
	}
}


//generates all the capture moves possible
void GenCapMoves(BOARD *board,MOVELIST *MoveList)
{
	ASSERT(CheckBoard(board));
	MoveList->Count=0;
	int Side=board->Side;
	
	if(Side==WHITE)
	{
		//add moves for WHITE pawns
		for(int i=0;i<board->PceNum[wP];i++)
		{
			int sq=board->PceList[wP][i];
			ASSERT(sq!=OFF);
			
			//now we add diagnol move or capture move for the pawns..and as we know wP can move +9 and +11 for black capture
			if(board->Pieces[sq+9]!=OFF && PceCol[board->Pieces[sq+9]]==BLACK)
				AddwPMove(board,sq,sq+9,board->Pieces[sq+9],0,MoveList);
			if(board->Pieces[sq+11]!=OFF && PceCol[board->Pieces[sq+11]]==BLACK)
				AddwPMove(board,sq,sq+11,board->Pieces[sq+11],0,MoveList);
				
				
			//now we add if any EnPas moves for Pawns are present
			if(board->EnPas==sq+9 && board->EnPas!=NONE)
				AddwPMove(board,sq,sq+9,0,FlagEnPas,MoveList);
			if(board->EnPas==sq+11 && board->EnPas!=NONE)
				AddwPMove(board,sq,sq+11,0,FlagEnPas,MoveList);
		}
	}
	//and similarly copy the above code for black...which will be the exact same with AddbPMove function
	else
	{
		for(int i=0;i<board->PceNum[bP];i++)
		{
			int sq=board->PceList[bP][i];
			ASSERT(sq!=OFF);
			
			if(board->Pieces[sq-9]!=OFF && PceCol[board->Pieces[sq-9]]==WHITE)
				AddbPMove(board,sq,sq-9,board->Pieces[sq-9],0,MoveList);
			if(board->Pieces[sq-11]!=OFF && PceCol[board->Pieces[sq-11]]==WHITE)
				AddbPMove(board,sq,sq-11,board->Pieces[sq-11],0,MoveList);

			if(board->EnPas==sq-9 && board->EnPas!=NONE)
				AddbPMove(board,sq,sq-9,0,FlagEnPas,MoveList);
			if(board->EnPas==sq-11 && board->EnPas!=NONE)
				AddbPMove(board,sq,sq-11,0,FlagEnPas,MoveList);
		}
	}
	//now we generate moves for sliding type pieces and the same code for both white and black....
	int PceIndex=SlideIndex[Side];
	int Pce=SlidePce[PceIndex++];
	while(Pce!=0)
	{
		for(int i=0;i<board->PceNum[Pce];i++)
		{
			int sq=board->PceList[Pce][i];
			ASSERT(sq>=0 && sq<120);
			for(int j=0;j<NumDir[Pce];j++)
			{
				int Dir=PceDir[Pce][j];
				int NewSq=sq+Dir;
				while(board->Pieces[NewSq]!=OFF)
				{
					if(board->Pieces[NewSq]!=EMPTY)
					{
						if(PceCol[board->Pieces[NewSq]]==Side^1)
							AddMove(board,MkMove(sq,NewSq,board->Pieces[NewSq],0,0),MoveList);
						break;
					}
					NewSq+=Dir;
				}
			}
		}
		Pce=SlidePce[PceIndex++];
	}
	
	
	//now similar way to generate moves for non slide pieces
	PceIndex=NonSlideIndex[Side];
	Pce=NonSlidePce[PceIndex++];
	while(Pce!=0)
	{
		for(int i=0;i<board->PceNum[Pce];i++)
		{
			int sq=board->PceList[Pce][i];
			ASSERT(sq>=0 && sq<120);
			for(int j=0;j<NumDir[Pce];j++)
			{
				int Dir=PceDir[Pce][j];
				int NewSq=sq+Dir;
				if(board->Pieces[NewSq]!=OFF && board->Pieces[NewSq]!=EMPTY)
				{
					if(PceCol[board->Pieces[NewSq]]==Side^1)
						AddMove(board,MkMove(sq,NewSq,board->Pieces[NewSq],0,0),MoveList);
				}
			}
		}
		Pce=NonSlidePce[PceIndex++];
	}
}





//checks if the given move exists on the board or not....we need to make the move because say a move is illegal
//(gives the king in check)..then that move cannot be made...so we need to make the move an then check if it is legal 
int MoveExists(BOARD *board,int Move)
{
	MOVELIST list[1];
	GenMoves(board,list);
	for(int i=0;i<list->Count;i++)
	{
		if(!MakeMove(board,list->MoveNum[i].Move))
			continue;
		TakeMove(board);
		if(Move==list->MoveNum[i].Move)
			return TRUE;
	}
	return FALSE;
}
