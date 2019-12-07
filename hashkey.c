#include "def.h"
#include <stdlib.h>

ULL GenerateHashKeys(const BOARD* board)
{
    ULL FinalKey = 0ULL;
    int Piece = EMPTY;

    // Run a loop for all 120 positions of the board and check if there is any piece on the current square with the help
    // of our piece array if there is a piece present...say a wP is present on sq 43 the we use that hashkey value  which
    // denotes that a white pawn is present on sq 43 for xoring to our final key we do this for all the elements on the
    // board.
    for (int i = 0; i < 120; i++)
    {
        Piece = board->Pieces[i];
        if (Piece != EMPTY && Piece != NONE && Piece != OFF)
        {
            ASSERT(Piece >= wP && Piece <= bK);
            FinalKey ^= HashKey[Piece][i];
        }
    }

    // If it the turn of white we xor our SideKey to the unique value for this arrangement.
    if (board->Side == WHITE)
    {
        FinalKey ^= SideKey;
    }

    // We also xor the castle perminion value in this current board state which are indicated by 4 bits so the value
    // ranges from 0 to 15 and we xor respective random value generated for the bitmasked value of current
    // castlepermissions.
    FinalKey ^= CastleKey[board->CastlePerm];

    // Finally if in this curent move if a EnPas is possible for any pawn we will know at which square it is present and
    // we xor the respective random value to our final key and we pick the random value from EMPTY row because it
    // will never be used.
    if (board->EnPas != NONE)
    {
        ASSERT(board->EnPas >= 0 && board->EnPas < 120)
        FinalKey ^= HashKey[EMPTY][board->EnPas];
    }
    return FinalKey;
}
