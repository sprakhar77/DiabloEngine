#include "def.h"
#include<algorithm>
#define INF 30000
#define MATE 29000

//this function is for sorting the moves in the movelist in descending order...so that we can get better ordering
int cmp(MOVE a,MOVE b)
{
	return a.Score>b.Score;
}

//fucntion to check up if we have run out of time and we need to make a move
static void CheckUp(SEARCHINFO *info)
{
	if(info->timeset==TRUE && GetTime() >info->stoptime)
		info->stopped=TRUE;
	ReadInput(info);
}

//this functions return true if a ceratin board position is repeated...and ofcoures if a pawn is moved or
//a piece is captured...then the fiftymoves resets...because we cannot move a pawn back or bring back
//the captured piece....so from there on we all the positions before that cannot be repeated so we start
//the loop from the last time when the fifty moves was reset 
int IsRepeat(BOARD *board)
{
	for(int i=board->HisPly-board->FiftyMove;i<board->HisPly-1;i++)
		if(board->PosKey==board->History[i].PosKey)
			return TRUE;
	return FALSE;
}

//this functions clears all the previous data beforn starting a search
static void ClearForSearch(BOARD *board, SEARCHINFO *info)
{
	
	for(int i=0;i<13;i++)
		for(int j=0;j<120;j++)
			board->SearchHistory[i][j]=0;
	
	for(int i=0;i<2;i++)
		for(int j=0;j<MAXDEPTH;j++)
			board->SearchKillers[i][j]=0;
			
	ClearPvTable(board->PvTable);
	
	//remember that ply is the half move made in every search so it must be set to 0 at the start of
	//every search
	board->Ply=0;
	info->stopped=0;
	info->nodes=0;
	info->fhf=0;
	info->fh=0;
}

static int Quiescence(int Alpha,int Beta,BOARD *board,SEARCHINFO *info)
{
	ASSERT(CheckBoard(board));
	
	if((info->nodes & 2047)==0)
		CheckUp(info);
	
	info->nodes++;
	if((IsRepeat(board)||board->FiftyMove>=100) && board->Ply)
		return 0;
	if(board->Ply>MAXDEPTH-1)
		return EvalPos(board);
	int Score=EvalPos(board);
	
	if(Score>=Beta)
		return Beta;
	if(Score>Alpha)
		Alpha=Score;
		
	MOVELIST list[1];
	GenCapMoves(board,list);
	
	int Legal=0;
	int OldAlpha=Alpha;
	int BestMove=NOMOVE;
	Score=-INF;
	
	int PvMove=ProbePvMove(board);
	if(PvMove!=0)
	{
		for(int i=0;i<list->Count;i++)
		{
			if(list->MoveNum[i].Move==PvMove)
			{
				list->MoveNum[i].Score=2000000;
				break;
			}	
		}
	}
	
	
	
	if(list->Count>=1)
		std::sort(list->MoveNum,list->MoveNum+list->Count,cmp);

	for(int i=0;i<list->Count;i++)
	{
		if(!MakeMove(board,list->MoveNum[i].Move))
			continue;
		Legal++;
		Score=-Quiescence(-Beta,-Alpha,board,info);
		TakeMove(board);
		
		if(info->stopped==TRUE)
			return 0;
		
		if(Score>Alpha)
		{
			if(Score>=Beta)
			{
				if(Legal==1)
					info->fhf++;
				info->fh++;
				return Beta;
			}
			Alpha=Score;
			BestMove=list->MoveNum[i].Move;
		}
	}	
	if(Alpha!=OldAlpha)
		StorePvMove(board,BestMove);
	
	return Alpha;	
}

//this is our alphabeta search function
static int AlphaBeta(int Alpha,int Beta,int Depth,BOARD *board,SEARCHINFO *info,int Flag)
{
	ASSERT(CheckBoard(board));
	if(Depth==0)
	{
		//return EvalPos(board);
		return Quiescence(Alpha,Beta,board,info);
	}
	
	if((info->nodes & 2047)==0)
		CheckUp(info);
	
	info->nodes++;
	if((IsRepeat(board)||board->FiftyMove>=100) && board->Ply)
		return 0;
	if(board->Ply>MAXDEPTH-1)
		return EvalPos(board);
		
	MOVELIST list[1];
	GenMoves(board,list);
	
	//here now we apply our PvMove  hueristic...i.e we see if for this position we have already 
	//found a best move...then mostlikly this move will give us the most cutoff...so to do so we give it
	//the maximum prioriy..
	int PvMove=ProbePvMove(board);
	if(PvMove!=0)
	{
		for(int i=0;i<list->Count;i++)
		{
			if(list->MoveNum[i].Move==PvMove)
			{
				list->MoveNum[i].Score=2000000;
				break;
			}	
		}
	}
	
	
	
	//sort the moves in the movelist descending order of thier score for better move ordering
	if(list->Count>=1)
		std::sort(list->MoveNum,list->MoveNum+list->Count,cmp);
	
	int Legal=0;
	int OldAlpha=Alpha;
	int BestMove=0;
	int Score=-INF;
	
	for(int i=0;i<list->Count;i++)
	{
		if(!MakeMove(board,list->MoveNum[i].Move))
			continue;
		Legal++;
		Score=-AlphaBeta(-Beta,-Alpha,Depth-1,board,info,TRUE);
		TakeMove(board);
		
		if(info->stopped==TRUE)
			return 0;
		
		
		if(Score>Alpha)
		{
			if(Score>=Beta)
			{
				if(Legal==1)
					info->fhf++;
				info->fh++;
				//add the search killers...i.e non capture moves that cause beta cutoff
				if(!(list->MoveNum[i].Move & FlagCap))
				{
					board->SearchKillers[1][board->Ply]=board->SearchKillers[0][board->Ply];
					board->SearchKillers[0][board->Ply]=list->MoveNum[i].Move;
				}
				return Beta;
			}
			Alpha=Score;
			BestMove=list->MoveNum[i].Move;
			//add the search history...i.e the non capture moves that cause the alpha cutoff
			if(!(list->MoveNum[i].Move & FlagCap))
				board->SearchHistory[board->Pieces[FromSq(BestMove)]][ToSq(BestMove)]+=Depth;
		}
	}
	if(Legal==0)
	{
		if(IsSqAttack(board->KingPos[board->Side],board->Side^1,board))
			return -MATE + board->Ply;
		else
			return 0;
	}
	if(Alpha !=OldAlpha)
		StorePvMove(board,BestMove);
		
	return Alpha;
}

//this function is out iterative deepening function...
void SearchPos(BOARD *board,SEARCHINFO *info)
{
	int BestMove=0;
	int BestScore=-INF;
	ClearForSearch(board,info);
	for(int i=1;i<=info->depth;i++)
	{
		BestScore=AlphaBeta(-INF,INF,i,board,info,TRUE);
		if(info->stopped==TRUE)
			break;
		
		int PvLineCount=GetPvLine(board,i);
		BestMove=board->PvArray[0];
		printf("info score cp %d depth %d nodes %ld time %d ",
			BestScore,i,info->nodes,GetTime()-info->starttime);
		printf("pv");
		for(int j=0;j<PvLineCount;j++)
			printf(" %s",PrintMove(board->PvArray[j]));
		printf("\n");
	}
	printf("bestmove %s\n",PrintMove(BestMove));
}
