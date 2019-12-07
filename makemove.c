#include "def.h"
#include <stdio.h>
#include <stdlib.h>
const int CastlePerm[120] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                             15, 13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                             15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                             15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                             15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 7,  15, 15, 15, 3,  15, 15, 11, 15,
                             15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};

void RemovePiece(BOARD* board, int sq)
{
    ASSERT(board->Pieces[sq] != EMPTY);
    int Pce = board->Pieces[sq];
    int Side = PceCol[Pce];
    // Empty that square on the board.
    board->Pieces[sq] = EMPTY;
    // If not pawns
    if (PceBig[Pce])
    {
        board->BigPce[Side]--;
        if (PceMaj[Pce])
            board->MajPce[Side]--;
        else
            board->MinPce[Side]--;
    }
    // iIf they are pawns.
    else
    {
        ClearBit(board->Pawns[Side], sq120to64[sq]);
        ClearBit(board->Pawns[BOTH], sq120to64[sq]);
    }
    // Update the materal value of that key.
    board->ValPce[Side] -= PceVal[Pce];

    // Remove the hashkey value for that type of piece on that square.
    board->PosKey ^= HashKey[Pce][sq];

    // Finally update the picelist.
    int index = -1;
    for (int i = 0; i < board->PceNum[Pce]; i++)
        if (sq == board->PceList[Pce][i])
        {
            index = i;
            break;
        }
    ASSERT(index != -1);
    board->PceNum[Pce]--;
    board->PceList[Pce][index] = board->PceList[Pce][board->PceNum[Pce]];
}

void AddPiece(BOARD* board, int sq, int Pce)
{
    ASSERT(board->Pieces[sq] == EMPTY);
    ASSERT(Pce >= 0 && Pce < 13);
    int Side = PceCol[Pce];

    // Add that Pce to the square on the board.
    board->Pieces[sq] = Pce;

    // If not pawns
    if (PceBig[Pce])
    {
        board->BigPce[Side]++;
        if (PceMaj[Pce])
            board->MajPce[Side]++;
        else
            board->MinPce[Side]++;
    }

    // If they are pawns.
    else
    {
        SetBit(board->Pawns[Side], sq120to64[sq]);
        SetBit(board->Pawns[BOTH], sq120to64[sq]);
    }

    // Update the materal value of that key.
    board->ValPce[Side] += PceVal[Pce];

    // Add the hashkey value for that type of piece on that square.
    board->PosKey ^= HashKey[Pce][sq];

    // Finally update the picelist.
    board->PceList[Pce][board->PceNum[Pce]++] = sq;
}

void MovePiece(BOARD* board, int from, int to)
{
    int Pce = board->Pieces[from];
    ASSERT(board->Pieces[from] != EMPTY && board->Pieces[to] == EMPTY);
    RemovePiece(board, from);
    AddPiece(board, to, Pce);
}

int MakeMove(BOARD* board, int Move)
{
    ASSERT(CheckBoard(board));
    int From = FromSq(Move);
    int To = ToSq(Move);
    int Side = board->Side;

    // Assign all the current state values to the history array and increment it.
    board->History[board->HisPly].Move = Move;
    board->History[board->HisPly].PosKey = board->PosKey;
    board->History[board->HisPly].EnPas = board->EnPas;
    board->History[board->HisPly].FiftyMove = board->FiftyMove;
    board->History[board->HisPly].CastlePerm = board->CastlePerm;

    board->HisPly++;
    board->Ply++;

    // If the current move was an EnPas move remove the pieces at the required index.
    if (Move & FlagEnPas)
    {
        if (Side == WHITE)
            RemovePiece(board, To - 10);
        else

            RemovePiece(board, To + 10);
    }
    // Else if it was a castling move swap the king and the rook..here we only move the rook
    // because we will move the king when we reach the final move piece statement.
    else if (Move & FlagCastle)
    {
        switch (To)
        {
        case C1:
            MovePiece(board, A1, D1);
            break;
        case C8:
            MovePiece(board, A8, D8);
            break;
        case G1:
            MovePiece(board, H1, F1);
            break;
        case G8:
            MovePiece(board, H8, F8);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }

    // If the EnPas position was set for the current state we hash it put.
    if (board->EnPas != NONE)
        board->PosKey ^= HashKey[EMPTY][board->EnPas];

    // Also hashout the current CastlePerm state.
    board->PosKey ^= CastleKey[board->CastlePerm];

    // Update the new CastlePerm value to the board and set the EnPas value to NONE.
    board->CastlePerm &= CastlePerm[From];
    board->CastlePerm &= CastlePerm[To];
    board->EnPas = NONE;

    // Hash in the new castleperm.
    board->PosKey ^= CastleKey[board->CastlePerm];
    int Cap = Captured(Move);
    board->FiftyMove++;

    // If any thing is captured then remove that piece and reset the fiftymove.
    if (Cap != EMPTY)
    {
        RemovePiece(board, To);
        board->FiftyMove = 0;
    }

    // Check if it was a pawn start move if it was the nwe need to add and EnPas position
    // and reset the fiftymoves counter.
    if (IsPawn[board->Pieces[From]])
    {
        board->FiftyMove = 0;
        if (Move & FlagPawnSt)
        {
            if (Side == WHITE)
                board->EnPas = From + 10;
            else
                board->EnPas = From - 10;

            // Now hsah in the new EnPas
            board->PosKey ^= HashKey[EMPTY][board->EnPas];
        }
    }

    // Now finally all formalities on the board are don move the piec from 2.
    MovePiece(board, From, To);

    // Now we check if the piece was promoted we need to clear that moved piece and add the promoted
    // piece.
    if (Promoted(Move) != EMPTY)
    {
        int Pce = Promoted(Move);
        RemovePiece(board, To);
        AddPiece(board, To, Pce);
    }

    // Update the kings position if the king is moved.
    if (IsKing[board->Pieces[To]])
        board->KingPos[Side] = To;

    // Change the side to play.
    board->Side ^= 1;

    // If the side will become 1 sidekey will be hashed in if it becomes 0 it will behashed out.
    board->PosKey ^= SideKey;

    // If the king is attaacked then this is ann illigal move and return false.
    ASSERT(CheckBoard(board));
    if (IsSqAttack(board->KingPos[Side], board->Side, board))
    {
        TakeMove(board);
        return FALSE;
    }
    return TRUE;
}

// This function is the exact mirror of makemove.
void TakeMove(BOARD* board)
{
    // Firstly decrease the move made the last time by decreasing the count of ply and hisply.
    board->Ply--;
    board->HisPly--;
    int Move = board->History[board->HisPly].Move;
    int From = FromSq(Move);
    int To = ToSq(Move);

    // Reset all the values of the board from the history board
    // note that we will add the position key at the last because the functions AddKey and RemoveKey will alter
    // with the Pos key so we need to add this value at the last so that it remains unchanged.
    board->EnPas = board->History[board->HisPly].EnPas;
    board->CastlePerm = board->History[board->HisPly].CastlePerm;
    board->FiftyMove = board->History[board->HisPly].FiftyMove;

    // Now reverse back the side.
    board->Side ^= 1;

    // If Enpas capture was made put back that pawn which was captured.
    if (Move & FlagEnPas)
    {
        if (board->Side == WHITE)
            AddPiece(board, To - 10, bP);
        else
            AddPiece(board, To + 10, wP);
    }
    else if (Move & FlagCastle)
    {
        switch (To)
        {
        case C1:
            MovePiece(board, D1, A1);
            break;
        case C8:
            MovePiece(board, D8, A8);
            break;
        case G1:
            MovePiece(board, F1, H1);
            break;
        case G8:
            MovePiece(board, F8, H8);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }

    // At this stage move the piece back.
    MovePiece(board, To, From);

    // If the king had made the movement in this move then we need to reset king position.
    if (IsKing[board->Pieces[From]])
        board->KingPos[board->Side] = From;
    // Now if a piece was captured add tat piece also.
    if (Captured(Move) != EMPTY)
        AddPiece(board, To, Captured(Move));

    // If it was a promotion move then clear the piece and aad a pawn to from.
    if (Promoted(Move) != EMPTY)
    {
        RemovePiece(board, From);
        AddPiece(board, From, (PceCol[Promoted(Move)] == WHITE ? wP : bP));
    }

    board->PosKey = board->History[board->HisPly].PosKey;
    ASSERT(CheckBoard);
}
