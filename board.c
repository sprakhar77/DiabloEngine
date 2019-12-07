#include "def.h"
#include <stdio.h>
#include <stdlib.h>

// Function to reset the entire board to 0.
void ResetBoard(BOARD* board)
{
    // Set all 120 indexes to OFFBOARD.
    for (int i = 0; i < 120; i++)
    {
        board->Pieces[i] = OFF;
    }

    // Set the insiede playable 64squares to be EMPTY.
    for (int i = 0; i < 64; i++)
    {
        board->Pieces[sq64to120[i]] = EMPTY;
    }

    // Set all other thing to 0 or NONE or EMPTY.
    for (int i = 0; i < 2; i++)
    {
        board->BigPce[i] = 0;
        board->MajPce[i] = 0;
        board->MinPce[i] = 0;
        board->Pawns[i] = 0ULL;
        board->ValPce[i] = 0;
    }
    for (int i = 0; i < 13; i++)
    {
        board->PceNum[i] = 0;
    }

    board->KingPos[WHITE] = board->KingPos[BLACK] = NONE;

    board->Ply = 0;
    board->HisPly = 0;
    board->Side = BOTH;
    board->FiftyMove = 0;
    board->EnPas = NONE;
    board->CastlePerm = 0;

    board->PosKey = 0ULL;

    // PceList will be reset afterward in other functions.
}

// This function takes in an FEN notation string and sets the board accorting to that. It assigns all the pieces to 120
// size board and then finally generates the uniques PosKey value for that arrangement but it doesnt computes
// PieceList,MajPce,MinPce.
int CalcFen(char* arr, BOARD* board)
{
    ASSERT(arr != NULL)
    ASSERT(board != NULL)
    int rank = RANK_8;
    int file = FILE_A;
    int count = 1;
    int piece = EMPTY;

    ResetBoard(board);

    while (rank >= RANK_1 && *arr)
    {
        count = 1;
        switch (*arr)
        {
        case 'p':
            piece = bP;
            break;
        case 'r':
            piece = bR;
            break;
        case 'n':
            piece = bN;
            break;
        case 'q':
            piece = bQ;
            break;
        case 'k':
            piece = bK;
            break;
        case 'b':
            piece = bB;
            break;

        case 'P':
            piece = wP;
            break;
        case 'R':
            piece = wR;
            break;
        case 'N':
            piece = wN;
            break;
        case 'Q':
            piece = wQ;
            break;
        case 'K':
            piece = wK;
            break;
        case 'B':
            piece = wB;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            piece = EMPTY;
            count = *arr - '0';
            break;

        case '/':
        case ' ':
            rank--;
            file = FILE_A;
            arr++;
            continue;

        default:
            printf("FEN ERROR\n");
            return -1;
        }
        for (int i = 0; i < count; i++)
        {
            int sq64 = rank * 8 + file;
            int sq120 = sq64to120[sq64];
            if (piece != EMPTY)
                board->Pieces[sq120] = piece;
            file++;
        }
        arr++;
    }  // end of while.
    ASSERT(*arr == 'w' || *arr == 'b');
    board->Side = (*arr == 'b') ? BLACK : WHITE;
    arr += 2;
    for (int i = 0; i < 4; i++)
    {
        if (*arr == ' ')
            break;
        switch (*arr)
        {
        case 'K':
            board->CastlePerm |= WKCA;
            break;
        case 'Q':
            board->CastlePerm |= WQCA;
            break;
        case 'k':
            board->CastlePerm |= BKCA;
            break;
        case 'q':
            board->CastlePerm |= BQCA;
            break;
        default:
            break;
        }
        arr++;
    }
    arr++;
    ASSERT(board->CastlePerm >= 0 && board->CastlePerm <= 15);

    if (*arr != '-')
    {
        file = *arr - 'a';
        rank = *(arr + 1) - '1';
        ASSERT(file >= FILE_A && file <= FILE_H);
        ASSERT(rank >= RANK_1 && rank <= RANK_8);
        board->EnPas = fr2sq(file, rank);
    }
    board->PosKey = GenerateHashKeys(board);
    UpdateMaterial(board);
    return 0;
}

// Function to print out current chess board on the screen.
void PrintBoard(BOARD* board)
{
    // remember we print the chess board in revers order of ranks.
    for (int rank = 7; rank >= 0; rank--)
    {
        printf("%c  ", RankChar[rank]);
        for (int file = 0; file < 8; file++)
        {
            int sq = fr2sq(file, rank);
            int piece = board->Pieces[sq];
            printf("%3c", PceChar[piece]);
        }
        printf("\n");
    }
    // Our FileChar,SideChar,PceChar and RankChar array are useful now for indexing as we can see.
    printf("\n  ");
    for (int i = 0; i < 8; i++)
    {
        printf("%3c", FileChar[i]);
    }

    printf("\n");
    printf("Side - %c\n", SideChar[board->Side]);
    printf("EnPas Side - %d\n", board->EnPas);

    // print CastlePerm by checking if each of the four respective bits are set or not.
    printf("castle - %c%c%c%c\n",
           (board->CastlePerm & WKCA) ? 'K' : '-',
           (board->CastlePerm & WQCA) ? 'Q' : '-',
           (board->CastlePerm & BKCA) ? 'k' : '-',
           (board->CastlePerm & BQCA) ? 'q' : '-');
    printf("PosKey - %llX\n", board->PosKey);
}

// Function to update the remaning items i.e PceList,MajPce,MinPce,etc.
void UpdateMaterial(BOARD* board)
{
    for (int i = 0; i < SQNO; i++)
    {
        int piece = board->Pieces[i];
        if (piece != EMPTY && piece != OFF)
        {
            // Get the color of this piece.
            int color = PceCol[piece];

            // Now increment the type of piece it is along indexed with its color.
            if (PceBig[piece])
                board->BigPce[color]++;
            if (PceMaj[piece])
                board->MajPce[color]++;
            if (PceMin[piece])
                board->MinPce[color]++;
            // Add the materialistice value of this piece to the total material value of that color.
            board->ValPce[color] += PceVal[piece];

            // Now finally update the PieceList. To do so we simpy use basic indexing.
            // we already have the number of pieces of current types stored in our board structure
            // which is initially set to 0 so now as we go on adding a type of piece we increase its
            // count and use it next time as the index.
            board->PceList[piece][board->PceNum[piece]] = i;
            board->PceNum[piece]++;

            // Finally check if the current piece is a king or it. If it is then store its position
            if (piece == wK)
                board->KingPos[WHITE] = i;
            if (piece == bK)
                board->KingPos[BLACK] = i;

            // We update our bitboard for white and black and both pawns as soon sa we encounter them
            // by setting that position with respect to 64 based indexing.
            if (piece == wP)
                SetBit(board->Pawns[WHITE], sq120to64[i]);
            if (piece == bP)
                SetBit(board->Pawns[BLACK], sq120to64[i]);
            if (piece == wP || piece == bP)
                SetBit(board->Pawns[BOTH], sq120to64[i]);
        }
    }
}

// This function crosscheks everthing we have done to our board so far with various assert statemnts. It ensure
// that what we have stored on the board structutre is actually present in the board.
int CheckBoard(BOARD* board)
{
    // Initiliaze a temporary board like sturcture with all its contents set to 0.
    int tPceNum[13] = {0};
    int tMajPce[2] = {0};
    int tMinPce[2] = {0};
    int tBigPce[2] = {0};
    int tValPce[2] = {0};

    ULL tPawns[3] = {0ULL};

    // Assign temporary pawns the value from the board structure then crosscheck it each pawns position
    // from PceList.
    tPawns[WHITE] = board->Pawns[WHITE];
    tPawns[BLACK] = board->Pawns[BLACK];
    tPawns[BOTH] = board->Pawns[BOTH];

    // Check the PceList by moving across PceList of each type of piece and taking out the square stored for it
    // and then check the board->Picece[sq] for the occurance of that same piece.
    for (int i = wP; i <= bK; i++)
    {
        for (int j = 0; j < board->PceNum[i]; j++)
        {
            int sq = board->PceList[i][j];
            ASSERT(board->Pieces[sq] == i);
        }
    }

    // Check the PceNum of various types of Pieces such ans Maj,Min,Big pieces and Piece value.
    for (int i = 0; i < 64; i++)
    {
        int sq = sq64to120[i];
        int piece = board->Pieces[sq];
        tPceNum[piece]++;
        int color = PceCol[piece];

        if (PceBig[piece])
            tBigPce[color]++;
        if (PceMaj[piece])
            tMajPce[color]++;
        if (PceMin[piece])
            tMinPce[color]++;
        tValPce[color] += PceVal[piece];
    }
    // Assert for all the count of various pieces.
    for (int i = wP; i <= bK; i++)
    {
        ASSERT(tPceNum[i] == board->PceNum[i]);
    }

    // Now check for the the pawns counts of each type from the bitboards.
    int count;
    count = COUNT(tPawns[WHITE]);
    ASSERT(count == board->PceNum[wP]);
    count = COUNT(tPawns[BLACK]);
    ASSERT(count == board->PceNum[bP]);
    count = COUNT(tPawns[BOTH]);
    ASSERT(count == board->PceNum[wP] + board->PceNum[bP]);

    // Check the bitboard squares they should be set in both the bitboards a the same position.
    int sq;
    while (tPawns[WHITE])
    {
        sq = POP(&tPawns[WHITE]);
        ASSERT(board->Pieces[sq64to120[sq]] == wP);
    }
    while (tPawns[BLACK])
    {
        sq = POP(&tPawns[BLACK]);
        ASSERT(board->Pieces[sq64to120[sq]] == bP);
    }
    while (tPawns[BOTH])
    {
        sq = POP(&tPawns[BOTH]);
        ASSERT((board->Pieces[sq64to120[sq]] == wP) || (board->Pieces[sq64to120[sq]] == bP));
    }

    // ASSERT the maj, min, big pieces and valuepieces.
    ASSERT(tMajPce[WHITE] == board->MajPce[WHITE] && tMajPce[BLACK] == board->MajPce[BLACK]);
    ASSERT(tMinPce[WHITE] == board->MinPce[WHITE] && tMinPce[BLACK] == board->MinPce[BLACK]);
    ASSERT(tBigPce[WHITE] == board->BigPce[WHITE] && tBigPce[BLACK] == board->BigPce[BLACK]);
    ASSERT(tValPce[WHITE] == board->ValPce[WHITE] && tValPce[BLACK] == board->ValPce[BLACK]);

    ASSERT(board->Side == WHITE || board->Side == BLACK);
    ASSERT(GenerateHashKeys(board) == board->PosKey);

    ASSERT(board->EnPas == NONE || (RankNo[board->EnPas] == RANK_6 && board->Side == WHITE)
           || (RankNo[board->EnPas] == RANK_3 && board->Side == BLACK));
    ASSERT(board->Pieces[board->KingPos[WHITE]] == wK);
    ASSERT(board->Pieces[board->KingPos[BLACK]] == bK);
    return TRUE;
}
